#include "PagamentoDAO.h"

#include <algorithm>
#include <cctype>
#include <cstdio>

#include "Ticket.h"

using namespace std;

PagamentoDAO::PagamentoDAO(BancoDados& banco)
{
    conexao = banco.getConexao();
}

void PagamentoDAO::inserir(Pagamento& pagamento)
{
    // valida antes de preparar, pq se lancar depois o comando ficaria sem finalizar
    validar(pagamento);

    sqlite3_stmt* comando = preparar("INSERT INTO pagamento (ticket_id, valor, data, metodo, status) VALUES (?, ?, ?, ?, ?);");

    vincularCampos(comando, pagamento);

    if (sqlite3_step(comando) != SQLITE_DONE)
    {
        throw finalizarComErro(comando);
    }

    sqlite3_finalize(comando);

    pagamento.setId((int) sqlite3_last_insert_rowid(conexao));
}

Pagamento PagamentoDAO::buscarPorTicket(Ticket* ticket) const
{
    if (ticket == NULL)
    {
        throw runtime_error("Ticket invalido para buscar pagamento");
    }

    sqlite3_stmt* comando = preparar("SELECT id, valor, data, metodo, status FROM pagamento WHERE ticket_id = ?;");

    sqlite3_bind_int(comando, 1, ticket->getId());

    int resultado = sqlite3_step(comando);

    // DONE sem nenhuma linha quer dizer que nao achou
    if (resultado == SQLITE_DONE)
    {
        sqlite3_finalize(comando);
        throw runtime_error("Pagamento nao encontrado: ticket " + to_string(ticket->getId()));
    }

    if (resultado != SQLITE_ROW)
    {
        throw finalizarComErro(comando);
    }

    try
    {
        Pagamento pagamento = montarPagamento(comando, ticket);
        sqlite3_finalize(comando);
        return pagamento;
    }
    catch (...)
    {
        // se a data ou o status vierem estranhos do banco, ainda precisa finalizar
        sqlite3_finalize(comando);
        throw;
    }
}

vector<Pagamento> PagamentoDAO::listarTodos(vector<Ticket>& tickets) const
{
    // ticket_id vem por ultimo para nao mudar as posicoes que o montarPagamento le
    return listar("SELECT id, valor, data, metodo, status, ticket_id FROM pagamento ORDER BY id;", tickets);
}

bool PagamentoDAO::atualizar(const Pagamento& pagamento)
{
    validar(pagamento);

    sqlite3_stmt* comando = preparar("UPDATE pagamento SET ticket_id = ?, valor = ?, data = ?, metodo = ?, status = ? WHERE id = ?;");

    vincularCampos(comando, pagamento);
    sqlite3_bind_int(comando, 6, pagamento.getId());

    if (sqlite3_step(comando) != SQLITE_DONE)
    {
        throw finalizarComErro(comando);
    }

    sqlite3_finalize(comando);

    // se nenhuma linha mudou e pq nao existe pagamento com esse id
    return sqlite3_changes(conexao) > 0;
}

bool PagamentoDAO::remover(int id)
{
    sqlite3_stmt* comando = preparar("DELETE FROM pagamento WHERE id = ?;");

    sqlite3_bind_int(comando, 1, id);

    if (sqlite3_step(comando) != SQLITE_DONE)
    {
        throw finalizarComErro(comando);
    }

    sqlite3_finalize(comando);

    return sqlite3_changes(conexao) > 0;
}

sqlite3_stmt* PagamentoDAO::preparar(const string& sql) const
{
    sqlite3_stmt* comando = NULL;

    if (sqlite3_prepare_v2(conexao, sql.c_str(), -1, &comando, NULL) != SQLITE_OK)
    {
        throw runtime_error("Erro ao preparar SQL: " + string(sqlite3_errmsg(conexao)));
    }

    return comando;
}

// copia a mensagem antes de finalizar e devolve a excecao para quem chamou dar o throw
runtime_error PagamentoDAO::finalizarComErro(sqlite3_stmt* comando) const
{
    string erro = sqlite3_errmsg(conexao);

    sqlite3_finalize(comando);

    return runtime_error("Erro no banco: " + erro);
}

void PagamentoDAO::validar(const Pagamento& pagamento) const
{
    if (pagamento.getTicket() == NULL)
    {
        throw runtime_error("Pagamento sem ticket");
    }

    // id 0 e de ticket que ainda nao foi gravado, entao nao tem o que referenciar
    if (pagamento.getTicket()->getId() == 0)
    {
        throw runtime_error("O ticket do pagamento ainda nao foi gravado no banco");
    }

    // a coluna data e NOT NULL, entao so da pra gravar depois do confirmar()
    if (pagamento.getData() == 0)
    {
        throw runtime_error("Pagamento pendente sem data: confirme antes de gravar");
    }

    // so pra lancar aqui se metodo ou status forem invalidos
    metodoParaBanco(pagamento.getMetodo());
    statusParaBanco(pagamento.getStatus());
}

// vincula ticket_id, valor, data, metodo e status nas posicoes 1 a 5
void PagamentoDAO::vincularCampos(sqlite3_stmt* comando, const Pagamento& pagamento) const
{
    sqlite3_bind_int(comando, 1, pagamento.getTicket()->getId());
    sqlite3_bind_double(comando, 2, pagamento.getValor());
    // TRANSIENT faz o sqlite copiar o texto, pq as funcoes devolvem string temporaria
    sqlite3_bind_text(comando, 3, formatarData(pagamento.getData()).c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(comando, 4, metodoParaBanco(pagamento.getMetodo()).c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(comando, 5, statusParaBanco(pagamento.getStatus()).c_str(), -1, SQLITE_TRANSIENT);
}

// as colunas precisam vir na ordem id, valor, data, metodo, status
Pagamento PagamentoDAO::montarPagamento(sqlite3_stmt* comando, Ticket* ticket) const
{
    int id = sqlite3_column_int(comando, 0);
    double valor = sqlite3_column_double(comando, 1);
    time_t data = lerData((const char*) sqlite3_column_text(comando, 2));
    string metodo = (const char*) sqlite3_column_text(comando, 3);
    string status = statusDoBanco((const char*) sqlite3_column_text(comando, 4));

    return Pagamento(ticket, valor, data, metodo, status, id);
}

// alem das colunas do montarPagamento, o sql precisa trazer o ticket_id na posicao 5
vector<Pagamento> PagamentoDAO::listar(const string& sql, vector<Ticket>& tickets) const
{
    sqlite3_stmt* comando = preparar(sql);

    vector<Pagamento> pagamentos;
    int resultado = sqlite3_step(comando);

    try
    {
        while (resultado == SQLITE_ROW)
        {
            int ticketId = sqlite3_column_int(comando, 5);
            Ticket* ticket = NULL;

            // procura no vetor de quem chamou o ticket dessa linha
            for (unsigned int i = 0; i < tickets.size() && ticket == NULL; i++)
            {
                if (tickets[i].getId() == ticketId)
                {
                    ticket = &tickets[i];
                }
            }

            if (ticket == NULL)
            {
                throw runtime_error("Ticket do pagamento nao foi carregado: id " + to_string(ticketId));
            }

            pagamentos.push_back(montarPagamento(comando, ticket));
            resultado = sqlite3_step(comando);
        }
    }
    catch (...)
    {
        sqlite3_finalize(comando);
        throw;
    }

    if (resultado != SQLITE_DONE)
    {
        throw finalizarComErro(comando);
    }

    sqlite3_finalize(comando);

    return pagamentos;
}

// aceita qualquer caixa (pix, PIX, Pix) e grava como o CHECK da tabela espera
string PagamentoDAO::metodoParaBanco(const string& metodo) const
{
    string minusculo = metodo;

    for (unsigned int i = 0; i < minusculo.length(); i++)
    {
        minusculo[i] = (char) tolower(minusculo[i]);
    }

    if (minusculo == "dinheiro")
    {
        return "Dinheiro";
    }

    if (minusculo == "cartao")
    {
        return "Cartao";
    }

    if (minusculo == "pix")
    {
        return "Pix";
    }

    throw runtime_error("Metodo de pagamento invalido: " + metodo);
}

// a classe diz confirmado, o banco diz Pago
string PagamentoDAO::statusParaBanco(const string& status) const
{
    if (status == "pendente")
    {
        return "Pendente";
    }

    if (status == "confirmado")
    {
        return "Pago";
    }

    throw runtime_error("Status de pagamento invalido: " + status);
}

string PagamentoDAO::statusDoBanco(const string& status) const
{
    if (status == "Pendente")
    {
        return "pendente";
    }

    if (status == "Pago")
    {
        return "confirmado";
    }

    throw runtime_error("Status de pagamento invalido no banco: " + status);
}

string PagamentoDAO::formatarData(time_t data) const
{
    tm* local = localtime(&data);

    if (local == NULL)
    {
        throw runtime_error("Data invalida para gravar no banco");
    }

    char texto[20];
    strftime(texto, sizeof(texto), "%Y-%m-%d %H:%M:%S", local);

    return texto;
}

time_t PagamentoDAO::lerData(const string& texto) const
{
    int ano, mes, dia, hora, minuto, segundo;

    if (sscanf(texto.c_str(), "%d-%d-%d %d:%d:%d", &ano, &mes, &dia, &hora, &minuto, &segundo) != 6)
    {
        throw runtime_error("Data invalida no banco: " + texto);
    }

    tm partes = tm();
    partes.tm_year = ano - 1900;
    partes.tm_mon = mes - 1;
    partes.tm_mday = dia;
    partes.tm_hour = hora;
    partes.tm_min = minuto;
    partes.tm_sec = segundo;
    // -1 deixa o mktime descobrir sozinho se era horario de verao
    partes.tm_isdst = -1;

    time_t resultado = mktime(&partes);

    if (resultado == (time_t) -1)
    {
        throw runtime_error("Data invalida no banco: " + texto);
    }

    return resultado;
}
#include "TicketDAO.h"

#include <cstdio>

using namespace std;

TicketDAO::TicketDAO(BancoDados& banco)
{
    conexao = banco.getConexao();
}

void TicketDAO::inserir(Ticket& ticket)
{
    sqlite3_stmt* comando = preparar("INSERT INTO ticket (veiculo_id, vaga_id, entrada, saida, valor) VALUES (?, ?, ?, ?, ?);");

    vincularCampos(comando, ticket);

    if (sqlite3_step(comando) != SQLITE_DONE)
    {
        throw finalizarComErro(comando);
    }

    sqlite3_finalize(comando);

    ticket.setId((int) sqlite3_last_insert_rowid(conexao));
}

Ticket TicketDAO::buscarPorId(int id) const
{
    sqlite3_stmt* comando = preparar("SELECT id, veiculo_id, vaga_id, entrada, saida, valor FROM ticket WHERE id = ?;");

    sqlite3_bind_int(comando, 1, id);

    int resultado = sqlite3_step(comando);

    // DONE sem nenhuma linha quer dizer que nao achou
    if (resultado == SQLITE_DONE)
    {
        sqlite3_finalize(comando);
        throw runtime_error("Ticket nao encontrado: id " + to_string(id));
    }

    if (resultado != SQLITE_ROW)
    {
        throw finalizarComErro(comando);
    }

    try
    {
        Ticket ticket = montarTicket(comando);
        sqlite3_finalize(comando);
        return ticket;
    }
    catch (...)
    {
        // se a data do banco estiver errada o montarTicket lanca, e ainda precisa finalizar
        sqlite3_finalize(comando);
        throw;
    }
}

Ticket TicketDAO::buscarAbertoPorVeiculo(int veiculoId) const
{
    sqlite3_stmt* comando = preparar("SELECT id, veiculo_id, vaga_id, entrada, saida, valor FROM ticket "
                                     "WHERE veiculo_id = ? AND saida IS NULL;");

    sqlite3_bind_int(comando, 1, veiculoId);

    int resultado = sqlite3_step(comando);

    if (resultado == SQLITE_DONE)
    {
        sqlite3_finalize(comando);
        throw runtime_error("Ticket aberto nao encontrado: veiculo id " + to_string(veiculoId));
    }

    if (resultado != SQLITE_ROW)
    {
        throw finalizarComErro(comando);
    }

    try
    {
        Ticket ticket = montarTicket(comando);
        sqlite3_finalize(comando);
        return ticket;
    }
    catch (...)
    {
        sqlite3_finalize(comando);
        throw;
    }
}

bool TicketDAO::temTicketAberto(int veiculoId) const
{
    sqlite3_stmt* comando = preparar("SELECT 1 FROM ticket WHERE veiculo_id = ? AND saida IS NULL LIMIT 1;");

    sqlite3_bind_int(comando, 1, veiculoId);

    int resultado = sqlite3_step(comando);

    if (resultado != SQLITE_ROW && resultado != SQLITE_DONE)
    {
        throw finalizarComErro(comando);
    }

    sqlite3_finalize(comando);

    // ROW quer dizer que achou pelo menos uma linha
    return resultado == SQLITE_ROW;
}

vector<Ticket> TicketDAO::listarTodos() const
{
    return listar("SELECT id, veiculo_id, vaga_id, entrada, saida, valor FROM ticket ORDER BY id;");
}

vector<Ticket> TicketDAO::listarAbertos() const
{
    return listar("SELECT id, veiculo_id, vaga_id, entrada, saida, valor FROM ticket WHERE saida IS NULL ORDER BY entrada;");
}

bool TicketDAO::atualizar(const Ticket& ticket)
{
    sqlite3_stmt* comando = preparar("UPDATE ticket SET veiculo_id = ?, vaga_id = ?, entrada = ?, saida = ?, valor = ? WHERE id = ?;");

    vincularCampos(comando, ticket);
    sqlite3_bind_int(comando, 6, ticket.getId());

    if (sqlite3_step(comando) != SQLITE_DONE)
    {
        throw finalizarComErro(comando);
    }

    sqlite3_finalize(comando);

    // se nenhuma linha mudou e pq nao existe ticket com esse id
    return sqlite3_changes(conexao) > 0;
}

bool TicketDAO::remover(int id)
{
    sqlite3_stmt* comando = preparar("DELETE FROM ticket WHERE id = ?;");

    sqlite3_bind_int(comando, 1, id);

    // ticket com pagamento cai aqui, a chave estrangeira bloqueia o delete
    if (sqlite3_step(comando) != SQLITE_DONE)
    {
        throw finalizarComErro(comando);
    }

    sqlite3_finalize(comando);

    return sqlite3_changes(conexao) > 0;
}

sqlite3_stmt* TicketDAO::preparar(const string& sql) const
{
    sqlite3_stmt* comando = NULL;

    if (sqlite3_prepare_v2(conexao, sql.c_str(), -1, &comando, NULL) != SQLITE_OK)
    {
        throw runtime_error("Erro ao preparar SQL: " + string(sqlite3_errmsg(conexao)));
    }

    return comando;
}

// copia a mensagem antes de finalizar e devolve a excecao para quem chamou dar o throw
runtime_error TicketDAO::finalizarComErro(sqlite3_stmt* comando) const
{
    string erro = sqlite3_errmsg(conexao);

    sqlite3_finalize(comando);

    return runtime_error("Erro no banco: " + erro);
}

// vincula veiculo_id, vaga_id, entrada, saida e valor nas posicoes 1 a 5
void TicketDAO::vincularCampos(sqlite3_stmt* comando, const Ticket& ticket) const
{
    sqlite3_bind_int(comando, 1, ticket.getVeiculoId());
    sqlite3_bind_int(comando, 2, ticket.getVagaId());
    // TRANSIENT faz o sqlite copiar o texto, pq o formatarData devolve uma string temporaria
    sqlite3_bind_text(comando, 3, formatarData(ticket.getEntrada()).c_str(), -1, SQLITE_TRANSIENT);

    // ticket aberto grava NULL e nao 0 ou "", pq e o NULL que marca no banco que o veiculo nao saiu
    if (ticket.estaAberto())
    {
        sqlite3_bind_null(comando, 4);
        sqlite3_bind_null(comando, 5);
    }
    else
    {
        sqlite3_bind_text(comando, 4, formatarData(ticket.getSaida()).c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(comando, 5, ticket.getValor());
    }
}

// as colunas precisam vir na ordem id, veiculo_id, vaga_id, entrada, saida, valor
Ticket TicketDAO::montarTicket(sqlite3_stmt* comando) const
{
    int id = sqlite3_column_int(comando, 0);
    int veiculoId = sqlite3_column_int(comando, 1);
    int vagaId = sqlite3_column_int(comando, 2);
    time_t entrada = lerData((const char*) sqlite3_column_text(comando, 3));

    // com NULL o sqlite devolveria "" e 0.0, entao testa antes de ler.
    // saida 0 e o jeito da classe Ticket dizer que esta aberto
    time_t saida = 0;
    double valor = 0.0;

    if (sqlite3_column_type(comando, 4) != SQLITE_NULL)
    {
        saida = lerData((const char*) sqlite3_column_text(comando, 4));
    }

    if (sqlite3_column_type(comando, 5) != SQLITE_NULL)
    {
        valor = sqlite3_column_double(comando, 5);
    }

    return Ticket(veiculoId, vagaId, entrada, saida, valor, id);
}

vector<Ticket> TicketDAO::listar(const string& sql) const
{
    sqlite3_stmt* comando = preparar(sql);

    vector<Ticket> tickets;
    int resultado = sqlite3_step(comando);

    try
    {
        while (resultado == SQLITE_ROW)
        {
            tickets.push_back(montarTicket(comando));
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

    return tickets;
}

// grava no horario local, que e o que aparece pro usuario
string TicketDAO::formatarData(time_t data) const
{
    char texto[20];

    strftime(texto, sizeof(texto), "%Y-%m-%d %H:%M:%S", localtime(&data));

    return texto;
}

time_t TicketDAO::lerData(const string& texto) const
{
    tm partes = {};

    if (sscanf(texto.c_str(), "%d-%d-%d %d:%d:%d", &partes.tm_year, &partes.tm_mon, &partes.tm_mday,
               &partes.tm_hour, &partes.tm_min, &partes.tm_sec) != 6)
    {
        throw runtime_error("Data invalida no banco: " + texto);
    }

    // no struct tm o ano conta a partir de 1900 e o mes vai de 0 a 11
    partes.tm_year -= 1900;
    partes.tm_mon -= 1;
    // -1 deixa o mktime descobrir sozinho se e horario de verao
    partes.tm_isdst = -1;

    return mktime(&partes);
}

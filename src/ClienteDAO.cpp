#include "ClienteDAO.h"

using namespace std;

ClienteDAO::ClienteDAO(BancoDados& banco)
{
    conexao = banco.getConexao();
}

void ClienteDAO::inserir(Cliente& cliente)
{
    sqlite3_stmt* comando = preparar("INSERT INTO cliente (nome, cpf, telefone) VALUES (?, ?, ?);");

    vincularCampos(comando, cliente);

    if (sqlite3_step(comando) != SQLITE_DONE)
    {
        throw finalizarComErro(comando);
    }

    sqlite3_finalize(comando);

    cliente.setId((int) sqlite3_last_insert_rowid(conexao));
}

Cliente ClienteDAO::buscarPorId(int id) const
{
    sqlite3_stmt* comando = preparar("SELECT id, nome, cpf, telefone FROM cliente WHERE id = ?;");

    sqlite3_bind_int(comando, 1, id);

    int resultado = sqlite3_step(comando);

    // DONE sem nenhuma linha quer dizer que nao achou
    if (resultado == SQLITE_DONE)
    {
        sqlite3_finalize(comando);
        throw runtime_error("Cliente nao encontrado: id " + to_string(id));
    }

    if (resultado != SQLITE_ROW)
    {
        throw finalizarComErro(comando);
    }

    try
    {
        Cliente cliente = montarCliente(comando);
        sqlite3_finalize(comando);
        return cliente;
    }
    catch (...)
    {
        // se o construtor do Cliente lancar, ainda precisa finalizar
        sqlite3_finalize(comando);
        throw;
    }
}

Cliente ClienteDAO::buscarPorCpf(const string& cpf) const
{
    sqlite3_stmt* comando = preparar("SELECT id, nome, cpf, telefone FROM cliente WHERE cpf = ?;");

    sqlite3_bind_text(comando, 1, cpf.c_str(), -1, SQLITE_TRANSIENT);

    int resultado = sqlite3_step(comando);

    if (resultado == SQLITE_DONE)
    {
        sqlite3_finalize(comando);
        throw runtime_error("Cliente nao encontrado: cpf " + cpf);
    }

    if (resultado != SQLITE_ROW)
    {
        throw finalizarComErro(comando);
    }

    try
    {
        Cliente cliente = montarCliente(comando);
        sqlite3_finalize(comando);
        return cliente;
    }
    catch (...)
    {
        sqlite3_finalize(comando);
        throw;
    }
}

vector<Cliente> ClienteDAO::listarTodos() const
{
    sqlite3_stmt* comando = preparar("SELECT id, nome, cpf, telefone FROM cliente ORDER BY nome;");

    vector<Cliente> clientes;
    int resultado = sqlite3_step(comando);

    try
    {
        while (resultado == SQLITE_ROW)
        {
            clientes.push_back(montarCliente(comando));
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

    return clientes;
}

bool ClienteDAO::atualizar(const Cliente& cliente)
{
    sqlite3_stmt* comando = preparar("UPDATE cliente SET nome = ?, cpf = ?, telefone = ? WHERE id = ?;");

    vincularCampos(comando, cliente);
    sqlite3_bind_int(comando, 4, cliente.getId());

    if (sqlite3_step(comando) != SQLITE_DONE)
    {
        throw finalizarComErro(comando);
    }

    sqlite3_finalize(comando);

    // se nenhuma linha mudou e pq nao existe cliente com esse id
    return sqlite3_changes(conexao) > 0;
}

bool ClienteDAO::remover(int id)
{
    sqlite3_stmt* comando = preparar("DELETE FROM cliente WHERE id = ?;");

    sqlite3_bind_int(comando, 1, id);

    // cliente com veiculo cadastrado cai aqui, a chave estrangeira bloqueia o delete
    if (sqlite3_step(comando) != SQLITE_DONE)
    {
        throw finalizarComErro(comando);
    }

    sqlite3_finalize(comando);

    return sqlite3_changes(conexao) > 0;
}

sqlite3_stmt* ClienteDAO::preparar(const string& sql) const
{
    sqlite3_stmt* comando = NULL;

    if (sqlite3_prepare_v2(conexao, sql.c_str(), -1, &comando, NULL) != SQLITE_OK)
    {
        throw runtime_error("Erro ao preparar SQL: " + string(sqlite3_errmsg(conexao)));
    }

    return comando;
}

// copia a mensagem antes de finalizar e devolve a excecao para quem chamou dar o throw
runtime_error ClienteDAO::finalizarComErro(sqlite3_stmt* comando) const
{
    string erro = sqlite3_errmsg(conexao);

    sqlite3_finalize(comando);

    return runtime_error("Erro no banco: " + erro);
}

// vincula nome, cpf e telefone nas posicoes 1, 2 e 3
void ClienteDAO::vincularCampos(sqlite3_stmt* comando, const Cliente& cliente) const
{
    // TRANSIENT faz o sqlite copiar o texto, pq os getters devolvem string temporaria
    sqlite3_bind_text(comando, 1, cliente.getNome().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(comando, 2, cliente.getCpf().c_str(), -1, SQLITE_TRANSIENT);

    // sem telefone vai NULL no banco em vez de string vazia
    if (cliente.temTelefone())
    {
        sqlite3_bind_text(comando, 3, cliente.getTelefone().c_str(), -1, SQLITE_TRANSIENT);
    }
    else
    {
        sqlite3_bind_null(comando, 3);
    }
}

// as colunas precisam vir na ordem id, nome, cpf, telefone
Cliente ClienteDAO::montarCliente(sqlite3_stmt* comando) const
{
    int id = sqlite3_column_int(comando, 0);
    string nome = (const char*) sqlite3_column_text(comando, 1);
    string cpf = (const char*) sqlite3_column_text(comando, 2);

    // telefone NULL vira string vazia, que e como o Cliente guarda "sem telefone"
    const char* texto = (const char*) sqlite3_column_text(comando, 3);
    string telefone = texto ? texto : "";

    return Cliente(nome, cpf, telefone, id);
}
#include "VagaDAO.h"

using namespace std;

VagaDAO::VagaDAO(BancoDados& banco)
{
    conexao = banco.getConexao();
}

void VagaDAO::inserir(Vaga& vaga)
{
    sqlite3_stmt* comando = preparar("INSERT INTO vaga (numero, tipo, ocupada) VALUES (?, ?, ?);");

    sqlite3_bind_int(comando, 1, vaga.getNumero());
    // TRANSIENT faz o sqlite copiar o texto, pq o getTipo() devolve uma string temporaria
    sqlite3_bind_text(comando, 2, vaga.getTipo().c_str(), -1, SQLITE_TRANSIENT);
    // no banco ocupada e 0 ou 1
    sqlite3_bind_int(comando, 3, vaga.estaOcupada() ? 1 : 0);

    if (sqlite3_step(comando) != SQLITE_DONE)
    {
        throw finalizarComErro(comando);
    }

    sqlite3_finalize(comando);

    vaga.setId((int) sqlite3_last_insert_rowid(conexao));
}

Vaga VagaDAO::buscarPorId(int id) const
{
    sqlite3_stmt* comando = preparar("SELECT id, numero, tipo, ocupada FROM vaga WHERE id = ?;");

    sqlite3_bind_int(comando, 1, id);

    int resultado = sqlite3_step(comando);

    // DONE sem nenhuma linha quer dizer que nao achou
    if (resultado == SQLITE_DONE)
    {
        sqlite3_finalize(comando);
        throw runtime_error("Vaga nao encontrada: id " + to_string(id));
    }

    if (resultado != SQLITE_ROW)
    {
        throw finalizarComErro(comando);
    }

    try
    {
        Vaga vaga = montarVaga(comando);
        sqlite3_finalize(comando);
        return vaga;
    }
    catch (...)
    {
        // se o construtor da Vaga lancar, ainda precisa finalizar
        sqlite3_finalize(comando);
        throw;
    }
}

Vaga VagaDAO::buscarPorNumero(int numero) const
{
    sqlite3_stmt* comando = preparar("SELECT id, numero, tipo, ocupada FROM vaga WHERE numero = ?;");

    sqlite3_bind_int(comando, 1, numero);

    int resultado = sqlite3_step(comando);

    if (resultado == SQLITE_DONE)
    {
        sqlite3_finalize(comando);
        throw runtime_error("Vaga nao encontrada: numero " + to_string(numero));
    }

    if (resultado != SQLITE_ROW)
    {
        throw finalizarComErro(comando);
    }

    try
    {
        Vaga vaga = montarVaga(comando);
        sqlite3_finalize(comando);
        return vaga;
    }
    catch (...)
    {
        sqlite3_finalize(comando);
        throw;
    }
}

vector<Vaga> VagaDAO::listarTodas() const
{
    return listar("SELECT id, numero, tipo, ocupada FROM vaga ORDER BY numero;");
}

vector<Vaga> VagaDAO::listarLivres() const
{
    return listar("SELECT id, numero, tipo, ocupada FROM vaga WHERE ocupada = 0 ORDER BY numero;");
}

bool VagaDAO::atualizar(const Vaga& vaga)
{
    sqlite3_stmt* comando = preparar("UPDATE vaga SET numero = ?, tipo = ?, ocupada = ? WHERE id = ?;");

    sqlite3_bind_int(comando, 1, vaga.getNumero());
    sqlite3_bind_text(comando, 2, vaga.getTipo().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(comando, 3, vaga.estaOcupada() ? 1 : 0);
    sqlite3_bind_int(comando, 4, vaga.getId());

    if (sqlite3_step(comando) != SQLITE_DONE)
    {
        throw finalizarComErro(comando);
    }

    sqlite3_finalize(comando);

    // se nenhuma linha mudou e pq nao existe vaga com esse id
    return sqlite3_changes(conexao) > 0;
}

bool VagaDAO::remover(int id)
{
    sqlite3_stmt* comando = preparar("DELETE FROM vaga WHERE id = ?;");

    sqlite3_bind_int(comando, 1, id);

    if (sqlite3_step(comando) != SQLITE_DONE)
    {
        throw finalizarComErro(comando);
    }

    sqlite3_finalize(comando);

    return sqlite3_changes(conexao) > 0;
}

sqlite3_stmt* VagaDAO::preparar(const string& sql) const
{
    sqlite3_stmt* comando = NULL;

    if (sqlite3_prepare_v2(conexao, sql.c_str(), -1, &comando, NULL) != SQLITE_OK)
    {
        throw runtime_error("Erro ao preparar SQL: " + string(sqlite3_errmsg(conexao)));
    }

    return comando;
}

// copia a mensagem antes de finalizar e devolve a excecao para quem chamou dar o throw
runtime_error VagaDAO::finalizarComErro(sqlite3_stmt* comando) const
{
    string erro = sqlite3_errmsg(conexao);

    sqlite3_finalize(comando);

    return runtime_error("Erro no banco: " + erro);
}

// as colunas precisam vir na ordem id, numero, tipo, ocupada
Vaga VagaDAO::montarVaga(sqlite3_stmt* comando) const
{
    int id = sqlite3_column_int(comando, 0);
    int numero = sqlite3_column_int(comando, 1);
    string tipo = (const char*) sqlite3_column_text(comando, 2);
    int ocupada = sqlite3_column_int(comando, 3);

    Vaga vaga(numero, tipo, id);

    // a vaga acabou de ser criada livre, entao o ocupar() nao lanca logic_error
    if (ocupada == 1)
    {
        vaga.ocupar();
    }

    return vaga;
}

vector<Vaga> VagaDAO::listar(const string& sql) const
{
    sqlite3_stmt* comando = preparar(sql);

    vector<Vaga> vagas;
    int resultado = sqlite3_step(comando);

    try
    {
        while (resultado == SQLITE_ROW)
        {
            vagas.push_back(montarVaga(comando));
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

    return vagas;
}

#include "VeiculoDAO.h"
#include "Carro.h"
#include "Moto.h"
#include "Caminhao.h"

using namespace std;

VeiculoDAO::VeiculoDAO(BancoDados& banco)
{
    conexao = banco.getConexao();
}

void VeiculoDAO::inserir(Veiculo& veiculo)
{
    sqlite3_stmt* comando = preparar("INSERT INTO veiculo (placa, modelo, tipo, cliente_id) VALUES (?, ?, ?, ?);");

    vincularCampos(comando, veiculo);

    if (sqlite3_step(comando) != SQLITE_DONE)
    {
        throw finalizarComErro(comando);
    }

    sqlite3_finalize(comando);

    veiculo.setId((int) sqlite3_last_insert_rowid(conexao));
}

unique_ptr<Veiculo> VeiculoDAO::buscarPorId(int id) const
{
    sqlite3_stmt* comando = preparar("SELECT id, placa, modelo, tipo, cliente_id FROM veiculo WHERE id = ?;");

    sqlite3_bind_int(comando, 1, id);

    int resultado = sqlite3_step(comando);

    // DONE sem nenhuma linha quer dizer que nao achou
    if (resultado == SQLITE_DONE)
    {
        sqlite3_finalize(comando);
        throw runtime_error("Veiculo nao encontrado: id " + to_string(id));
    }

    if (resultado != SQLITE_ROW)
    {
        throw finalizarComErro(comando);
    }

    try
    {
        unique_ptr<Veiculo> veiculo = montarVeiculo(comando);
        sqlite3_finalize(comando);
        return veiculo;
    }
    catch (...)
    {
        // se o construtor do Veiculo lancar, ainda precisa finalizar
        sqlite3_finalize(comando);
        throw;
    }
}

unique_ptr<Veiculo> VeiculoDAO::buscarPorPlaca(const string& placa) const
{
    sqlite3_stmt* comando = preparar("SELECT id, placa, modelo, tipo, cliente_id FROM veiculo WHERE placa = ?;");

    sqlite3_bind_text(comando, 1, placa.c_str(), -1, SQLITE_TRANSIENT);

    int resultado = sqlite3_step(comando);

    if (resultado == SQLITE_DONE)
    {
        sqlite3_finalize(comando);
        throw runtime_error("Veiculo nao encontrado: placa " + placa);
    }

    if (resultado != SQLITE_ROW)
    {
        throw finalizarComErro(comando);
    }

    try
    {
        unique_ptr<Veiculo> veiculo = montarVeiculo(comando);
        sqlite3_finalize(comando);
        return veiculo;
    }
    catch (...)
    {
        sqlite3_finalize(comando);
        throw;
    }
}

vector<unique_ptr<Veiculo>> VeiculoDAO::listarTodos() const
{
    sqlite3_stmt* comando = preparar("SELECT id, placa, modelo, tipo, cliente_id FROM veiculo ORDER BY placa;");

    vector<unique_ptr<Veiculo>> veiculos;
    int resultado = sqlite3_step(comando);

    try
    {
        while (resultado == SQLITE_ROW)
        {
            veiculos.push_back(montarVeiculo(comando));
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

    return veiculos;
}

bool VeiculoDAO::atualizar(const Veiculo& veiculo)
{
    sqlite3_stmt* comando = preparar("UPDATE veiculo SET placa = ?, modelo = ?, tipo = ?, cliente_id = ? WHERE id = ?;");

    vincularCampos(comando, veiculo);
    sqlite3_bind_int(comando, 5, veiculo.getId());

    if (sqlite3_step(comando) != SQLITE_DONE)
    {
        throw finalizarComErro(comando);
    }

    sqlite3_finalize(comando);

    // se nenhuma linha mudou e pq nao existe veiculo com esse id
    return sqlite3_changes(conexao) > 0;
}

bool VeiculoDAO::remover(int id)
{
    sqlite3_stmt* comando = preparar("DELETE FROM veiculo WHERE id = ?;");

    sqlite3_bind_int(comando, 1, id);

    // veiculo com ticket cai aqui, a chave estrangeira bloqueia o delete
    if (sqlite3_step(comando) != SQLITE_DONE)
    {
        throw finalizarComErro(comando);
    }

    sqlite3_finalize(comando);

    return sqlite3_changes(conexao) > 0;
}

sqlite3_stmt* VeiculoDAO::preparar(const string& sql) const
{
    sqlite3_stmt* comando = NULL;

    if (sqlite3_prepare_v2(conexao, sql.c_str(), -1, &comando, NULL) != SQLITE_OK)
    {
        throw runtime_error("Erro ao preparar SQL: " + string(sqlite3_errmsg(conexao)));
    }

    return comando;
}

// copia a mensagem antes de finalizar e devolve a excecao para quem chamou dar o throw
runtime_error VeiculoDAO::finalizarComErro(sqlite3_stmt* comando) const
{
    string erro = sqlite3_errmsg(conexao);

    sqlite3_finalize(comando);

    return runtime_error("Erro no banco: " + erro);
}

// vincula placa, modelo, tipo e cliente_id nas posicoes 1, 2, 3 e 4
void VeiculoDAO::vincularCampos(sqlite3_stmt* comando, const Veiculo& veiculo) const
{
    // TRANSIENT faz o sqlite copiar o texto, pq os getters devolvem string temporaria
    sqlite3_bind_text(comando, 1, veiculo.getPlaca().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(comando, 2, veiculo.getModelo().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(comando, 3, veiculo.getTipo().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(comando, 4, veiculo.getClienteId());
}

// as colunas precisam vir na ordem id, placa, modelo, tipo, cliente_id
unique_ptr<Veiculo> VeiculoDAO::montarVeiculo(sqlite3_stmt* comando) const
{
    int id = sqlite3_column_int(comando, 0);
    string placa = (const char*) sqlite3_column_text(comando, 1);
    string modelo = (const char*) sqlite3_column_text(comando, 2);
    string tipo = (const char*) sqlite3_column_text(comando, 3);
    int clienteId = sqlite3_column_int(comando, 4);

    // o tipo do banco decide qual subclasse criar
    if (tipo == "Carro")
    {
        return unique_ptr<Veiculo>(new Carro(placa, modelo, clienteId, id));
    }

    if (tipo == "Moto")
    {
        return unique_ptr<Veiculo>(new Moto(placa, modelo, clienteId, id));
    }

    if (tipo == "Caminhao")
    {
        return unique_ptr<Veiculo>(new Caminhao(placa, modelo, clienteId, id));
    }
    throw runtime_error("Tipo de veiculo ainda nao suportado: " + tipo);
}

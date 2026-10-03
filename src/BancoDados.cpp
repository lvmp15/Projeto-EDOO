#include "BancoDados.h"

#include <stdexcept>

using namespace std;

BancoDados::BancoDados(const string& caminho)
{
    conexao = NULL;

    if (sqlite3_open(caminho.c_str(), &conexao) != SQLITE_OK)
    {
        string erro = sqlite3_errmsg(conexao);

        // mesmo falhando o sqlite aloca a conexao, e o destrutor nao roda
        // se o construtor lanca excecao, entao fecha aq
        sqlite3_close(conexao);
        throw runtime_error("Erro ao abrir o banco: " + erro);
    }

    // se outro programa estiver com o banco aberto, espera ate 2s em vez de falhar na hora
    sqlite3_busy_timeout(conexao, 2000);

    // o sqlite ignora chave estrangeira por padrao, precisa ligar em toda conexao
    try
    {
        executar("PRAGMA foreign_keys = ON;");
    }
    catch (runtime_error&)
    {
        sqlite3_close(conexao);
        throw;
    }
}

BancoDados::~BancoDados()
{
    sqlite3_close(conexao);
}

sqlite3* BancoDados::getConexao() const
{
    return conexao;
}

void BancoDados::executar(const string& sql)
{
    char* mensagem = NULL;

    if (sqlite3_exec(conexao, sql.c_str(), NULL, NULL, &mensagem) != SQLITE_OK)
    {
        string erro = mensagem;

        // a mensagem e alocada pelo sqlite, entao tem que liberar antes de lancar
        sqlite3_free(mensagem);
        throw runtime_error("Erro ao executar SQL: " + erro);
    }
}

bool BancoDados::tabelasExistem() const
{
    string sql = "SELECT COUNT(*) FROM sqlite_master WHERE type = 'table' "
                 "AND name IN ('cliente', 'veiculo', 'vaga', 'ticket', 'pagamento');";

    sqlite3_stmt* consulta = NULL;

    if (sqlite3_prepare_v2(conexao, sql.c_str(), -1, &consulta, NULL) != SQLITE_OK)
    {
        throw runtime_error("Erro ao verificar tabelas: " + string(sqlite3_errmsg(conexao)));
    }

    int quantidade = 0;

    if (sqlite3_step(consulta) == SQLITE_ROW)
    {
        quantidade = sqlite3_column_int(consulta, 0);
    }

    sqlite3_finalize(consulta);

    return quantidade == 5;
}

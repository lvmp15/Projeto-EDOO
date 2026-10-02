#ifndef PAGAMENTODAO_H
#define PAGAMENTODAO_H

#include <string>
#include <ctime>
#include <stdexcept>
#include <sqlite3.h>

#include "Pagamento.h"
#include "BancoDados.h"

using namespace std;

class PagamentoDAO
{

private:
    // so usa a conexao, quem abre e fecha e o BancoDados
    sqlite3* conexao;

    sqlite3_stmt* preparar(const string& sql) const;
    runtime_error finalizarComErro(sqlite3_stmt* comando) const;
    void validar(const Pagamento& pagamento) const;
    void vincularCampos(sqlite3_stmt* comando, const Pagamento& pagamento) const;
    Pagamento montarPagamento(sqlite3_stmt* comando, Ticket* ticket) const;

    // a classe e o banco escrevem metodo e status de formas diferentes, aqui traduz
    string metodoParaBanco(const string& metodo) const;
    string statusParaBanco(const string& status) const;
    string statusDoBanco(const string& status) const;

    // no banco a data fica como 'YYYY-MM-DD HH:MM:SS' em hora local
    string formatarData(time_t data) const;
    time_t lerData(const string& texto) const;

public:
    PagamentoDAO(BancoDados& banco);

    void inserir(Pagamento& pagamento);

    // o Ticket ja vem carregado de fora, o DAO so liga ele ao pagamento lido
    Pagamento buscarPorTicket(Ticket* ticket) const;

    bool atualizar(const Pagamento& pagamento);
    bool remover(int id);
};

#endif
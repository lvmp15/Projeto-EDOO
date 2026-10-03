#ifndef TICKETDAO_H
#define TICKETDAO_H

#include <string>
#include <vector>
#include <ctime>
#include <stdexcept>
#include <sqlite3.h>

#include "Ticket.h"
#include "BancoDados.h"

using namespace std;

class TicketDAO
{

private:
    // so usa a conexao, quem abre e fecha e o BancoDados, por isso o DAO nao tem destrutor
    sqlite3* conexao;

    sqlite3_stmt* preparar(const string& sql) const;
    runtime_error finalizarComErro(sqlite3_stmt* comando) const;
    void vincularCampos(sqlite3_stmt* comando, const Ticket& ticket) const;
    Ticket montarTicket(sqlite3_stmt* comando) const;
    vector<Ticket> listar(const string& sql) const;

    // no banco as datas sao texto 'YYYY-MM-DD HH:MM:SS', na classe sao time_t
    string formatarData(time_t data) const;
    time_t lerData(const string& texto) const;

public:
    TicketDAO(BancoDados& banco);

    // referencia sem const pq o id gerado pelo banco e gravado de volta no ticket
    void inserir(Ticket& ticket);

    Ticket buscarPorId(int id) const;
    Ticket buscarAbertoPorVeiculo(int veiculoId) const;
    bool temTicketAberto(int veiculoId) const;

    vector<Ticket> listarTodos() const;
    vector<Ticket> listarAbertos() const;

    bool atualizar(const Ticket& ticket);
    bool remover(int id);
};

#endif

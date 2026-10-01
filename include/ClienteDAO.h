#ifndef CLIENTEDAO_H
#define CLIENTEDAO_H

#include <string>
#include <vector>
#include <stdexcept>
#include <sqlite3.h>

#include "Cliente.h"
#include "BancoDados.h"

using namespace std;

class ClienteDAO
{

private:
    // so usa a conexao, quem abre e fecha e o BancoDados
    sqlite3* conexao;

    sqlite3_stmt* preparar(const string& sql) const;
    runtime_error finalizarComErro(sqlite3_stmt* comando) const;
    void vincularCampos(sqlite3_stmt* comando, const Cliente& cliente) const;
    Cliente montarCliente(sqlite3_stmt* comando) const;

public:
    ClienteDAO(BancoDados& banco);

    void inserir(Cliente& cliente);

    Cliente buscarPorId(int id) const;
    Cliente buscarPorCpf(const string& cpf) const;

    vector<Cliente> listarTodos() const;

    bool atualizar(const Cliente& cliente);
    bool remover(int id);
};

#endif
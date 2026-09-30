#ifndef VAGADAO_H
#define VAGADAO_H

#include <string>
#include <vector>
#include <stdexcept>
#include <sqlite3.h>

#include "Vaga.h"
#include "BancoDados.h"

using namespace std;

class VagaDAO
{

private:
    // so usa a conexao, quem abre e fecha e o BancoDados
    sqlite3* conexao;

    sqlite3_stmt* preparar(const string& sql) const;
    runtime_error finalizarComErro(sqlite3_stmt* comando) const;
    Vaga montarVaga(sqlite3_stmt* comando) const;
    vector<Vaga> listar(const string& sql) const;

public:
    VagaDAO(BancoDados& banco);

    void inserir(Vaga& vaga);

    Vaga buscarPorId(int id) const;
    Vaga buscarPorNumero(int numero) const;

    vector<Vaga> listarTodas() const;
    vector<Vaga> listarLivres() const;

    bool atualizar(const Vaga& vaga);
    bool remover(int id);
};

#endif

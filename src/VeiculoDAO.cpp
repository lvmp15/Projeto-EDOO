#ifndef VEICULODAO_H
#define VEICULODAO_H

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <sqlite3.h>

#include "Veiculo.h"
#include "BancoDados.h"

using namespace std;

class VeiculoDAO
{

private:
    // so usa a conexao, quem abre e fecha e o BancoDados
    sqlite3* conexao;

    sqlite3_stmt* preparar(const string& sql) const;
    runtime_error finalizarComErro(sqlite3_stmt* comando) const;
    void vincularCampos(sqlite3_stmt* comando, const Veiculo& veiculo) const;
    unique_ptr<Veiculo> montarVeiculo(sqlite3_stmt* comando) const;

public:
    VeiculoDAO(BancoDados& banco);

    void inserir(Veiculo& veiculo);

    // Veiculo e abstrato, entao devolve ponteiro pra subclasse certa (Carro, Moto...)
    unique_ptr<Veiculo> buscarPorId(int id) const;
    unique_ptr<Veiculo> buscarPorPlaca(const string& placa) const;

    vector<unique_ptr<Veiculo>> listarTodos() const;

    bool atualizar(const Veiculo& veiculo);
    bool remover(int id);
};

#endif
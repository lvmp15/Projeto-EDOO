#ifndef BANCODADOS_H
#define BANCODADOS_H

#include <string>
#include <sqlite3.h>

using namespace std;

class BancoDados
{

private:
    sqlite3* conexao;

public:
    BancoDados(const string& caminho = "estacionamento.db");
    ~BancoDados();

    // proibe copia: duas copias teriam o mesmo ponteiro e o sqlite3_close
    // seria chamado duas vezes na mesma conexao
    BancoDados(const BancoDados& outro) = delete;
    BancoDados& operator=(const BancoDados& outro) = delete;

    sqlite3* getConexao() const;

    void executar(const string& sql);

    bool tabelasExistem() const;
};

#endif

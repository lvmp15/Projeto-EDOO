#ifndef BANCODADOS_H
#define BANCODADOS_H

#include <string>
#include <sqlite3.h>

using namespace std;

// dona da conexao com o sqlite: abre no construtor e fecha no destrutor
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

    // empresta o ponteiro pros DAOs, mas quem fecha a conexao continua sendo o BancoDados
    sqlite3* getConexao() const;

    void executar(const string& sql);

    // confere se as 5 tabelas do schema.sql ja foram criadas
    bool tabelasExistem() const;
};

#endif

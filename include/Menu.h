#ifndef MENU_H
#define MENU_H

#include <string>

using namespace std;

class Menu
{

private:
    void exibir() const;
    void executarOpcao(int opcao);

    // as onze opcoes do menu, por enquanto so avisam que ainda nao foram ligadas ao banco
    void cadastrarCliente() const;
    void cadastrarVeiculo() const;
    void cadastrarVaga() const;
    void registrarEntrada() const;
    void registrarSaida() const;
    void consultarVagas() const;
    void consultarVeiculos() const;
    void consultarTickets() const;
    void alterarCadastro() const;
    void excluirCadastro() const;
    void consultarPagamentos() const;

    // submenu de alterar e excluir: devolve 1 cliente, 2 veiculo, 3 vaga ou 0 para voltar
    int escolherCadastro(const string& acao) const;
    void emConstrucao(const string& nome) const;

    bool lerInteiro(const string& pergunta, int minimo, int maximo, int& valor) const;

public:
    Menu();

    void executar();
};

#endif
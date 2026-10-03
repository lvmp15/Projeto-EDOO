#ifndef MENU_H
#define MENU_H

#include <string>
#include <stdexcept>
#include <ctime>

#include "BancoDados.h"
#include "ClienteDAO.h"
#include "VeiculoDAO.h"
#include "VagaDAO.h"
#include "TicketDAO.h"
#include "Ticket.h"
#include "Pagamento.h"

using namespace std;

class Menu
{

private:
    // guardado so para abrir e fechar transacao, o resto passa pelos DAOs
    BancoDados& banco;
    ClienteDAO clienteDAO;
    VeiculoDAO veiculoDAO;
    VagaDAO vagaDAO;
    TicketDAO ticketDAO;

    void registrarPagamento(Ticket& ticket);

    void exibir() const;
    void executarOpcao(int opcao);

    // as onze opcoes do menu, as const ainda nao foram ligadas ao banco
    void cadastrarCliente();
    void cadastrarVeiculo();
    void cadastrarVaga();
    void registrarEntrada();
    void registrarSaida() const;
    void consultarVagas() const;
    void consultarVeiculos() const;
    void consultarTickets() const;
    void alterarCadastro();
    void excluirCadastro();
    void consultarPagamentos() const;

    void alterarCliente();
    void alterarVeiculo();
    void alterarVaga();
    void excluirCliente();
    void excluirVeiculo();
    void excluirVaga();

    // submenu de alterar e excluir: devolve 1 cliente, 2 veiculo, 3 vaga ou 0 para voltar
    int escolherCadastro(const string& acao) const;
    void emConstrucao(const string& nome) const;
    string traduzirErro(const string& mensagem) const;
    bool ehErroDeChaveEstrangeira(const runtime_error& erro) const;
    string formatarData(time_t data) const;

    bool lerInteiro(const string& pergunta, int minimo, int maximo, int& valor) const;
    bool lerTexto(const string& pergunta, string& valor, bool podeVazio = false) const;
    bool confirmar(const string& pergunta) const;

public:
    Menu(BancoDados& banco);

    void executar();
};

#endif

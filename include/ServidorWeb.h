#ifndef SERVIDORWEB_H
#define SERVIDORWEB_H

#include <string>
#include <mutex>
#include <functional>

#include "BancoDados.h"
#include "ClienteDAO.h"
#include "VeiculoDAO.h"
#include "VagaDAO.h"
#include "TicketDAO.h"
#include "PagamentoDAO.h"

using namespace std;

// so o nome das classes, o httplib.h e gigante e fica incluido so no .cpp
namespace httplib
{
    struct Request;
    struct Response;
}

class ServidorWeb
{

private:
    BancoDados& banco;
    ClienteDAO clienteDAO;
    VeiculoDAO veiculoDAO;
    VagaDAO vagaDAO;
    TicketDAO ticketDAO;
    PagamentoDAO pagamentoDAO;

    // o servidor atende varias requisicoes ao mesmo tempo, a trava deixa so uma mexer no banco por vez
    mutex trava;

    string estado() const;
    string cadastrarCliente(const string& nome, const string& cpf, const string& telefone);
    string cadastrarVeiculo(const string& placa, const string& modelo, const string& tipo, int clienteId);
    string cadastrarVaga(int numero, const string& tipo);
    string removerVaga(int vagaId);
    string alocarVeiculo(int vagaId, int veiculoId);
    string registrarSaida(int vagaId, const string& metodo);

    void responder(httplib::Response& resposta, const function<string()>& acao);
    void emTransacao(const function<void()>& gravar);

    int lerInteiro(const httplib::Request& requisicao, const string& campo) const;
    string lerTexto(const httplib::Request& requisicao, const string& campo, bool podeVazio = false) const;

    string traduzirErro(const string& mensagem) const;
    string formatarData(time_t data) const;
    string formatarPermanencia(time_t entrada, time_t saida) const;

public:
    ServidorWeb(BancoDados& banco);

    void iniciar(int porta);
};

#endif

#ifndef TICKET_H
#define TICKET_H

#include <ctime>

using namespace std;

class Veiculo;

class Ticket{

private:
    int id;
    int veiculoId;
    int vagaId;
    time_t entrada;
    time_t saida;
    double valor;

public:
    Ticket(int veiculoId, int vagaId, time_t entrada, int id = 0);

    // usado pelo DAO pra remontar um ticket que ja esta no banco
    Ticket(int veiculoId, int vagaId, time_t entrada, time_t saida, double valor, int id);

    int getId() const;
    void setId(int id);

    int getVeiculoId() const;
    int getVagaId() const;
    time_t getEntrada() const;
    time_t getSaida() const;
    double getValor() const;

    bool estaAberto() const;
    void registrarSaida(time_t saida, const Veiculo& veiculo);
};

#endif
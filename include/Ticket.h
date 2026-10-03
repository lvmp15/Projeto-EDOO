#ifndef TICKET_H
#define TICKET_H

#include <ctime>
#include <string>
#include <iostream>

using namespace std;

// aqui o Veiculo so aparece como referencia, entao basta declarar e o include fica no .cpp
class Veiculo;

class Ticket{

private:
    int id;
    int veiculoId;
    int vagaId;
    time_t entrada;
    time_t saida;
    double valor;

    string formatarData(time_t data) const;

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

    // fluxo e nao saida, pq saida ja e o atributo da hora de saida
    void exibir(ostream& fluxo) const;
};

ostream& operator<<(ostream& fluxo, const Ticket& ticket);

#endif
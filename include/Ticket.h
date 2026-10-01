#ifndef TICKET_H
#define TICKET_H

#include <ctime>

using namespace std;

class Veiculo;
class Vaga;

class Ticket{

private:
    int id;
    Veiculo* veiculo;
    Vaga* vaga;
    time_t entrada;
    time_t saida;
    double valor;

public:
    Ticket(Veiculo* veiculo, Vaga* vaga, time_t entrada, int id = 0);

    int getId() const;
    void setId(int id);

    Veiculo* getVeiculo() const;
    Vaga* getVaga() const;
    time_t getEntrada() const;
    time_t getSaida() const;
    double getValor() const;

    bool estaAberto() const;
    void registrarSaida(time_t saida);
    double calcularValor() const;
};

#endif
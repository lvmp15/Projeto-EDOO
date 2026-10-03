#ifndef PAGAMENTO_H
#define PAGAMENTO_H

#include <ctime>
#include <string>
#include <iostream>

using namespace std;

class Ticket;

class Pagamento{

private:
    int id;
    Ticket* ticket;
    double valor;
    time_t data;
    string metodo;
    string status;

public:
    Pagamento(Ticket* ticket, const string& metodo, int id = 0);

    // usado pelo DAO pra remontar um pagamento que ja estava salvo no banco
    Pagamento(Ticket* ticket, double valor, time_t data,
              const string& metodo, const string& status, int id = 0);

    int getId() const;
    void setId(int id);

    Ticket* getTicket() const;
    double getValor() const;
    time_t getData() const;
    string getMetodo() const;
    string getStatus() const;

    void confirmar();

    void exibir(ostream& saida) const;
};

ostream& operator<<(ostream& saida, const Pagamento& pagamento);

#endif
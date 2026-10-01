#ifndef PAGAMENTO_H
#define PAGAMENTO_H

#include <ctime>
#include <string>

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

    int getId() const;
    void setId(int id);

    Ticket* getTicket() const;
    double getValor() const;
    time_t getData() const;
    string getMetodo() const;
    string getStatus() const;

    void confirmar();
};

#endif
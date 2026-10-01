#include "Pagamento.h"
#include "Ticket.h"

// o valor e copiado do ticket, entao ele precisa estar fechado antes
Pagamento::Pagamento(Ticket* ticket, const string& metodo, int id)
    : id(id), ticket(ticket), valor(ticket ? ticket->getValor() : 0.0),
      data(0), metodo(metodo), status("pendente") {}

int Pagamento::getId() const { return id; }
void Pagamento::setId(int id) { this->id = id; }

Ticket* Pagamento::getTicket() const { return ticket; }
double Pagamento::getValor() const { return valor; }
time_t Pagamento::getData() const { return data; }
string Pagamento::getMetodo() const { return metodo; }
string Pagamento::getStatus() const { return status; }

void Pagamento::confirmar() {
    data = time(nullptr);
    status = "confirmado";
}
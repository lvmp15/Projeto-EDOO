#include "Pagamento.h"
#include "Ticket.h"
#include <stdexcept>

// o valor e copiado do ticket, entao ele precisa estar fechado antes
Pagamento::Pagamento(Ticket* ticket, const string& metodo, int id)
    : id(id), ticket(ticket), valor(ticket ? ticket->getValor() : 0.0),
      data(0), metodo(metodo), status("pendente") {
         if (ticket == nullptr || ticket->estaAberto()){
            throw invalid_argument("Ticket invalido ou ainda aberto"); //checa se o ticket ta aberto ou se é nulo
            }
        valor = ticket->getValor();
     }

Pagamento::Pagamento(Ticket* ticket, double valor, time_t data,
                     const string& metodo, const string& status, int id)
    : id(id), ticket(ticket), valor(valor), data(data), metodo(metodo), status(status) {}

void Pagamento::confirmar() {
    if (status == "confirmado") return;   // caso tenha sido confirmado nada acontece 
    data = time(nullptr);
    status = "confirmado";
}

int Pagamento::getId() const { return id; }
void Pagamento::setId(int id) { this->id = id; }

Ticket* Pagamento::getTicket() const { return ticket; }
double Pagamento::getValor() const { return valor; }
time_t Pagamento::getData() const { return data; }
string Pagamento::getMetodo() const { return metodo; }
string Pagamento::getStatus() const { return status; }
#include "Pagamento.h"
#include "Ticket.h"
#include <stdexcept>

// o valor e copiado do ticket, entao ele precisa estar fechado antes
Pagamento::Pagamento(Ticket* ticket, const string& metodo, int id)
    : id(id), ticket(ticket), valor(0.0), data(0), metodo(metodo), status("pendente") {
    // testa o ponteiro antes de usar, pq chamar ticket->getValor() com nullptr derruba o programa
    if (ticket == nullptr || ticket->estaAberto()) {
        throw invalid_argument("Ticket invalido ou ainda aberto");
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

void Pagamento::exibir(ostream& saida) const {
    saida << "Id: " << id << " | Ticket: ";

    if (ticket != nullptr) {
        saida << ticket->getId();
    }
    else {
        saida << "-";
    }

    saida << " | Valor: " << valor << " | Data: ";

    // data 0 e de pagamento pendente, que ainda nao foi confirmado
    if (data != 0) {
        char texto[20];
        strftime(texto, sizeof(texto), "%Y-%m-%d %H:%M:%S", localtime(&data));
        saida << texto;
    }
    else {
        saida << "-";
    }

    saida << " | Metodo: " << metodo << " | Status: " << status;
}

ostream& operator<<(ostream& saida, const Pagamento& pagamento) {
    pagamento.exibir(saida);
    return saida;
}
#include "Ticket.h"
#include "Veiculo.h"

Ticket::Ticket(Veiculo* veiculo, Vaga* vaga, time_t entrada, int id)
    : id(id), veiculo(veiculo), vaga(vaga), entrada(entrada), saida(0), valor(0.0) {}

int Ticket::getId() const { return id; }
void Ticket::setId(int id) { this->id = id; }

Veiculo* Ticket::getVeiculo() const { return veiculo; }
Vaga* Ticket::getVaga() const { return vaga; }
time_t Ticket::getEntrada() const { return entrada; }
time_t Ticket::getSaida() const { return saida; }
double Ticket::getValor() const { return valor; }

// saida 0 significa que o ticket ainda esta aberto
bool Ticket::estaAberto() const { return saida == 0; }

void Ticket::registrarSaida(time_t saida) {
    this->saida = saida;
    valor = calcularValor();
}

double Ticket::calcularValor() const {
    if (estaAberto() || veiculo == nullptr) return 0.0;
    double horas = difftime(saida, entrada) / 3600.0;
    return veiculo->calcularTarifa(horas);
}
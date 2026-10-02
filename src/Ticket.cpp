#include "Ticket.h"
#include "Veiculo.h"

#include <stdexcept>

Ticket::Ticket(int veiculoId, int vagaId, time_t entrada, int id)
    : id(id), veiculoId(veiculoId), vagaId(vagaId), entrada(entrada), saida(0), valor(0.0) {}

Ticket::Ticket(int veiculoId, int vagaId, time_t entrada, time_t saida, double valor, int id)
    : id(id), veiculoId(veiculoId), vagaId(vagaId), entrada(entrada), saida(saida), valor(valor) {}

int Ticket::getId() const { return id; }
void Ticket::setId(int id) { this->id = id; }

int Ticket::getVeiculoId() const { return veiculoId; }
int Ticket::getVagaId() const { return vagaId; }
time_t Ticket::getEntrada() const { return entrada; }
time_t Ticket::getSaida() const { return saida; }
double Ticket::getValor() const { return valor; }

// saida 0 significa que o ticket ainda esta aberto
bool Ticket::estaAberto() const { return saida == 0; }

// recebe o veiculo pq a tarifa depende do tipo dele
void Ticket::registrarSaida(time_t saida, const Veiculo& veiculo) {
    if (veiculo.getId() != veiculoId) {
        throw invalid_argument("Veiculo nao corresponde ao ticket");
    }

    this->saida = saida;
    double horas = difftime(saida, entrada) / 3600.0;
    valor = veiculo.calcularTarifa(horas);
}
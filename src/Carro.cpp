#include "Carro.h"

using namespace std;

// taxa por hora específica de Carro
namespace{
    const double taxaHoraCarro = 2.5;
    const double taxaFixa = 10.0;
}

Carro::Carro(const string& placa, const string& modelo, int clienteId, int id)
    : Veiculo(placa, modelo, clienteId, id){}

double Carro::calcularTarifa(double horas) const{
    return calcularComTolerancia(horas, taxaFixa, taxaHoraCarro);
}
string Carro::getTipo() const{
    return "Carro";
}
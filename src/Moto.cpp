#include "Moto.h"

using namespace std;

// taxas da Moto; o namespace sem nome deixa elas visiveis so neste arquivo
namespace{
    const double taxaHoraMoto = 1.5;
    const double taxaFixa = 5.0;
}

Moto::Moto(const string& placa, const string& modelo, int clienteId, int id)
    : Veiculo(placa, modelo, clienteId, id){}

double Moto::calcularTarifa(double horas) const{
    return calcularComTolerancia(horas, taxaFixa, taxaHoraMoto);
}
string Moto::getTipo() const{
    return "Moto";
}
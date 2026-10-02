#include "Caminhao.h"

using namespace std;

// taxa por hora específica de caminhao
namespace{
    const double taxaHoraCaminhao = 5.0;
    const double taxaFixa = 15.0;
}

Caminhao::Caminhao(const string& placa, const string& modelo, int clienteId, int id)
    : Veiculo(placa, modelo, clienteId, id){}

double Caminhao::calcularTarifa(double horas) const{
    return calcularComTolerancia(horas, taxaFixa, taxaHoraCaminhao);
}
string Caminhao::getTipo() const{
    return "Caminhao";
}
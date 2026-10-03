#include "Carro.h"

using namespace std;

// taxas do Carro; o namespace sem nome deixa elas visiveis so neste arquivo
namespace{
    const double taxaHoraCarro = 2.5;
    const double taxaFixa = 10.0;
}

// so repassa os dados pro construtor de Veiculo, que ja faz as validacoes
Carro::Carro(const string& placa, const string& modelo, int clienteId, int id)
    : Veiculo(placa, modelo, clienteId, id){}

double Carro::calcularTarifa(double horas) const{
    return calcularComTolerancia(horas, taxaFixa, taxaHoraCarro);
}
string Carro::getTipo() const{
    return "Carro";
}
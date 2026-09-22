#ifndef CARRO_H
#define CARRO_H

#include "Veiculo.h"

using namespace std;

class Carro : public Veiculo
{

public:
    Carro(const string& placa, const string& modelo, int clienteId, int id=0);

    double calcularTarifa(double horas) const override;
    string getTipo() const override;
};

#endif
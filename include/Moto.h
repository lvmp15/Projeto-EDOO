#ifndef MOTO_H
#define MOTO_H

#include "Veiculo.h"

using namespace std;

class Moto : public Veiculo
{

public:
    Moto(const string& placa, const string& modelo, int clienteId, int id=0);

    double calcularTarifa(double horas) const override;
    string getTipo() const override;
};

#endif
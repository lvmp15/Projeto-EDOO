#ifndef CAMINHAO_H
#define CAMINHAO_H

#include "Veiculo.h"

using namespace std;

class Caminhao : public Veiculo
{

public:
    Caminhao(const string& placa, const string& modelo, int clienteId, int id=0);

    double calcularTarifa(double horas) const override;
    string getTipo() const override;
};

#endif
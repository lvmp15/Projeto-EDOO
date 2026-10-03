#ifndef CARRO_H
#define CARRO_H

#include "Veiculo.h"

using namespace std;

// herda tudo de Veiculo e so define tarifa e tipo; nao precisa de destrutor proprio
class Carro : public Veiculo
{

public:
    Carro(const string& placa, const string& modelo, int clienteId, int id=0);

    // o override faz o compilador avisar se a assinatura nao bater com a do Veiculo
    double calcularTarifa(double horas) const override;
    string getTipo() const override;
};

#endif
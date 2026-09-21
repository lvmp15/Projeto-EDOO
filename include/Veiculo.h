#ifndef VEICULO_H
#define VEICULO_H

#include <string>
#include <iostream>

using namespace std;

class Veiculo
{
private:
    int id;
    string placa;
    string modelo;
    int clienteId;

    bool formatoValido(const string& placa) const;

public:
    Veiculo(const string& placa, const string& modelo,
            int clienteId, int id = 0);

    // virtual pq os veiculos sao apagados por um ponteiro Veiculo*
    virtual ~Veiculo();

    int getId() const;
    void setId(int id);

    string getPlaca() const;
    void setPlaca(const string& placa);

    string getModelo() const;
    void setModelo(const string& modelo);

    int getClienteId() const;
    void setClienteId(int clienteId);

    virtual double calcularTarifa(double horas) const = 0;
    virtual string getTipo() const = 0;

    virtual void exibir(ostream& saida) const;
};

ostream& operator<<(ostream& saida, const Veiculo& veiculo);

#endif

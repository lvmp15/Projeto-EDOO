#ifndef VEICULO_H
#define VEICULO_H

#include <string>
#include <iostream>

class Veiculo
{
private:
    int id;
    std::string placa;
    std::string modelo;
    int clienteId;

    bool formatoValido(const std::string& placa) const;

public:
    Veiculo(const std::string& placa, const std::string& modelo,
            int clienteId, int id = 0);

    // virtual porque os veiculos sao apagados por um ponteiro Veiculo*
    virtual ~Veiculo();

    int getId() const;
    void setId(int id);

    std::string getPlaca() const;
    void setPlaca(const std::string& placa);

    std::string getModelo() const;
    void setModelo(const std::string& modelo);

    int getClienteId() const;
    void setClienteId(int clienteId);

    virtual double calcularTarifa(double horas) const = 0;
    virtual std::string getTipo() const = 0;

    virtual void exibir(std::ostream& saida) const;
};

std::ostream& operator<<(std::ostream& saida, const Veiculo& veiculo);

#endif

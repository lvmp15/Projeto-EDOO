#ifndef VEICULO_H
#define VEICULO_H

#include <string>
#include <iostream>

using namespace std;

// classe abstrata: nao da pra criar um Veiculo direto, so Carro, Moto ou Caminhao
class Veiculo{

protected:
    // protected pq so as classes filhas usam, cada uma passando as suas taxas
    double calcularComTolerancia(double horas, double taxaFixa, double taxaHora, double tolerancia = 1.0) const;

private:
    // id 0 quer dizer que o veiculo ainda nao foi gravado no banco
    int id;
    string placa;
    string modelo;
    int clienteId;

    bool formatoValido(const string& placa) const;

public:
    Veiculo(const string& placa, const string& modelo,
            int clienteId, int id = 0);

    // virtual pq o unique_ptr<Veiculo> do DAO apaga Carro, Moto e Caminhao pelo ponteiro da base
    virtual ~Veiculo();

    int getId() const;
    // publico pq o DAO grava aqui o id que o banco gerou no insert
    void setId(int id);

    string getPlaca() const;
    void setPlaca(const string& placa);

    string getModelo() const;
    void setModelo(const string& modelo);

    int getClienteId() const;
    void setClienteId(int clienteId);

    // o "= 0" obriga cada filha a ter a sua versao, e e isso que deixa Veiculo abstrata
    virtual double calcularTarifa(double horas) const = 0;
    virtual string getTipo() const = 0;

    virtual void exibir(ostream& saida) const;
};

ostream& operator<<(ostream& saida, const Veiculo& veiculo);

#endif
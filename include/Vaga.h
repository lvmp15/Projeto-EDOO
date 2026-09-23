#ifndef VAGA_H
#define VAGA_H

#include <string>
#include <iostream>

#include "Veiculo.h"

using namespace std;

class Vaga{

private:
    int id;
    int numero;
    string tipo;
    bool ocupada;

    string normalizarTipo(const string& tipo) const;
    bool tipoValido(const string& tipo) const;

public:
    Vaga(int numero, const string& tipo, int id = 0);

    int getId() const;
    void setId(int id);

    int getNumero() const;
    void setNumero(int numero);

    string getTipo() const;
    void setTipo(const string& tipo);

    bool estaOcupada() const;

    // compara o tipo da vaga com o tipo do veiculo
    bool aceita(const Veiculo& veiculo) const;

    void ocupar();
    void liberar();

    void exibir(ostream& saida) const;
};

ostream& operator<<(ostream& saida, const Vaga& vaga);

#endif

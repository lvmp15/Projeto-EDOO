#ifndef VAGA_H
#define VAGA_H

#include <string>
#include <iostream>

#include "Veiculo.h"

using namespace std;

class Vaga{

private:
    // id 0 quer dizer que a vaga ainda nao foi gravada no banco
    int id;
    int numero;
    string tipo;
    // sem setter de proposito: so muda pelo ocupar() e liberar(), que conferem o estado antes
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

    // compara o tipo da vaga com o do veiculo; por ser referencia, chama o getTipo() da classe filha
    bool aceita(const Veiculo& veiculo) const;

    void ocupar();
    void liberar();

    void exibir(ostream& saida) const;
};

ostream& operator<<(ostream& saida, const Vaga& vaga);

#endif

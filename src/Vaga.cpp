#include "Vaga.h"

#include <stdexcept>
#include <cctype>

using namespace std;

Vaga::Vaga(int numero, const string& tipo, int id)
{
    // usa os setters para nao repetir a validacao
    setNumero(numero);
    setTipo(tipo);
    setId(id);

    ocupada = false;
}

int Vaga::getId() const
{
    return id;
}

void Vaga::setId(int id)
{
    this->id = id;
}

int Vaga::getNumero() const
{
    return numero;
}

void Vaga::setNumero(int numero)
{
    if (numero <= 0)
    {
        throw invalid_argument("Numero de vaga invalido");
    }

    this->numero = numero;
}

string Vaga::getTipo() const
{
    return tipo;
}

void Vaga::setTipo(const string& tipo)
{
    string normalizado = normalizarTipo(tipo);

    if (!tipoValido(normalizado))
    {
        throw invalid_argument("Tipo de vaga invalido: " + tipo);
    }

    this->tipo = normalizado;
}

// primeira letra maiuscula e o resto minusculo, entao "CARRO" e "carro" viram "Carro"
string Vaga::normalizarTipo(const string& tipo) const
{
    string normalizado;

    for (unsigned int i = 0; i < tipo.length(); i++)
    {
        if (i == 0)
        {
            normalizado += (char) toupper(tipo[i]);
        }
        else
        {
            normalizado += (char) tolower(tipo[i]);
        }
    }

    return normalizado;
}

bool Vaga::tipoValido(const string& tipo) const
{
    // os mesmos tipos devolvidos pelo getTipo() dos veiculos
    return tipo == "Carro" || tipo == "Moto" || tipo == "Caminhao";
}

bool Vaga::estaOcupada() const
{
    return ocupada;
}

bool Vaga::aceita(const Veiculo& veiculo) const
{
    return tipo == veiculo.getTipo();
}

void Vaga::ocupar()
{
    if (ocupada)
    {
        throw logic_error("A vaga ja esta ocupada");
    }

    ocupada = true;
}

void Vaga::liberar()
{
    if (!ocupada)
    {
        throw logic_error("A vaga ja esta livre");
    }

    ocupada = false;
}

void Vaga::exibir(ostream& saida) const
{
    saida << "Id: " << id
          << " | Numero: " << numero
          << " | Tipo: " << tipo
          << " | Situacao: ";

    if (ocupada)
    {
        saida << "Ocupada";
    }
    else
    {
        saida << "Livre";
    }
}

ostream& operator<<(ostream& saida, const Vaga& vaga)
{
    vaga.exibir(saida);
    return saida;
}

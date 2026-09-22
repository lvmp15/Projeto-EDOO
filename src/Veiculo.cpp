#include "Veiculo.h"

#include <stdexcept>
#include <cctype>

using namespace std;

Veiculo::Veiculo(const string& placa, const string& modelo,
                 int clienteId, int id)
{
    // passa pelos setters para nao repetir a validacao aq
    setPlaca(placa);
    setModelo(modelo);
    setClienteId(clienteId);
    setId(id);
}

Veiculo::~Veiculo()
{}

double Veiculo::calcularComTolerancia(double horas, double taxaFixa, double taxaHora, double tolerancia) const{
    if (horas <= tolerancia){
        return taxaFixa;
    }

    return taxaFixa + (horas - tolerancia) * taxaHora;
}

int Veiculo::getId() const
{
    return id;
}

void Veiculo::setId(int id)
{
    this->id = id;
}

string Veiculo::getPlaca() const
{
    return placa;
}

void Veiculo::setPlaca(const string& placa)
{
    string limpa;

    for (unsigned int i = 0; i < placa.length(); i++)
    {
        char c = placa[i];
        if (c != '-' && c != ' ')
        {
            limpa += (char) toupper(c);
        }
    }

    if (!formatoValido(limpa))
    {
        throw invalid_argument("Placa invalida: " + placa);
    }

    this->placa = limpa;
}

string Veiculo::getModelo() const
{
    return modelo;
}

void Veiculo::setModelo(const string& modelo)
{
    this->modelo = modelo;
}

int Veiculo::getClienteId() const
{
    return clienteId;
}

void Veiculo::setClienteId(int clienteId)
{
    if (clienteId <= 0)
    {
        throw invalid_argument("Id de cliente invalido");
    }

    this->clienteId = clienteId;
}

bool Veiculo::formatoValido(const string& placa) const
{
    if (placa.length() != 7)
    {
        return false;
    }

    // nos dois formatos as tres primeiras posicoes sao letras e a quarta e digito
    for (int i = 0; i < 3; i++)
    {
        if (!isalpha(placa[i]))
        {
            return false;
        }
    }

    if (!isdigit(placa[3]))
    {
        return false;
    }

    // formato antigo ABC1234
    if (isdigit(placa[4]) && isdigit(placa[5]) && isdigit(placa[6]))
    {
        return true;
    }

    // formato Mercosul ABC1D23
    if (isalpha(placa[4]) && isdigit(placa[5]) && isdigit(placa[6]))
    {
        return true;
    }

    return false;
}

void Veiculo::exibir(ostream& saida) const
{
    saida << "Id: " << id
          << " | Tipo: " << getTipo()
          << " | Placa: " << placa
          << " | Modelo: " << modelo
          << " | Cliente: " << clienteId;
}

ostream& operator<<(ostream& saida, const Veiculo& veiculo)
{
    veiculo.exibir(saida);
    return saida;
}

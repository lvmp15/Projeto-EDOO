#include "Veiculo.h"

#include <stdexcept>
#include <cctype>

Veiculo::Veiculo(const std::string& placa, const std::string& modelo,
                 int clienteId, int id)
{
    // passa pelos setters para nao repetir a validacao aqui
    setPlaca(placa);
    setModelo(modelo);
    setClienteId(clienteId);
    setId(id);
}

Veiculo::~Veiculo()
{
}

int Veiculo::getId() const
{
    return id;
}

void Veiculo::setId(int id)
{
    this->id = id;
}

std::string Veiculo::getPlaca() const
{
    return placa;
}

void Veiculo::setPlaca(const std::string& placa)
{
    std::string limpa;

    for (unsigned int i = 0; i < placa.length(); i++)
    {
        char c = placa[i];
        if (c != '-' && c != ' ')
        {
            limpa += (char) std::toupper(c);
        }
    }

    if (!formatoValido(limpa))
    {
        throw std::invalid_argument("Placa invalida: " + placa);
    }

    this->placa = limpa;
}

std::string Veiculo::getModelo() const
{
    return modelo;
}

void Veiculo::setModelo(const std::string& modelo)
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
        throw std::invalid_argument("Id de cliente invalido");
    }

    this->clienteId = clienteId;
}

bool Veiculo::formatoValido(const std::string& placa) const
{
    if (placa.length() != 7)
    {
        return false;
    }

    // nos dois formatos as tres primeiras posicoes sao letras e a quarta e digito
    for (int i = 0; i < 3; i++)
    {
        if (!std::isalpha(placa[i]))
        {
            return false;
        }
    }

    if (!std::isdigit(placa[3]))
    {
        return false;
    }

    // formato antigo ABC1234
    if (std::isdigit(placa[4]) && std::isdigit(placa[5]) && std::isdigit(placa[6]))
    {
        return true;
    }

    // formato Mercosul ABC1D23
    if (std::isalpha(placa[4]) && std::isdigit(placa[5]) && std::isdigit(placa[6]))
    {
        return true;
    }

    return false;
}

void Veiculo::exibir(std::ostream& saida) const
{
    saida << "Id: " << id
          << " | Tipo: " << getTipo()
          << " | Placa: " << placa
          << " | Modelo: " << modelo
          << " | Cliente: " << clienteId;
}

std::ostream& operator<<(std::ostream& saida, const Veiculo& veiculo)
{
    veiculo.exibir(saida);
    return saida;
}

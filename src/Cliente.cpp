#include "Cliente.h"

#include <stdexcept>
#include <cctype>

using namespace std;

Cliente::Cliente(const string& nome, const string& cpf, const string& telefone, int id)
{
    Cliente::setId(id);
    Cliente::setNome(nome);
    Cliente::setCpf(cpf);
    Cliente::setTelefone(telefone);
}

int Cliente::getId() const
{
    return id;
}

void Cliente::setId(int id)
{
    this->id = id;
}

string Cliente::getNome() const
{
    return nome;
}

void Cliente::setNome(const string& nome)
{
    if (nome.empty()) {
        throw invalid_argument("Nome nao pode ser vazio");
    }
    this->nome = nome;
}

string Cliente::getCpf() const
{
    return cpf;
}

void Cliente::setCpf(const string& cpf)
{
    string cpfLimpo = somenteDigitos(cpf);
    if (!cpfValido(cpfLimpo)) {
        throw invalid_argument("CPF invalido");
    }
    this->cpf = cpfLimpo;
}

string Cliente::getTelefone() const
{

    return telefone;
}

void Cliente::setTelefone(const string& telefone)
{
    if (telefone.empty()) {
        this->telefone = "";
        return;
    }
    else if(!telefone.empty()) {
        string telefoneLimpo = somenteDigitos(telefone);
        if (!telefoneValido(telefoneLimpo)) {
          throw invalid_argument("Telefone invalido");
     }
     this->telefone = telefoneLimpo;
}
}

bool Cliente::temTelefone() const
{
    bool semTelefone;
    semTelefone = !telefone.empty();
    return semTelefone;
}


string Cliente::somenteDigitos(const string& texto) const
{
    string resultado;
    for (char c : texto) {
        if (isdigit(c)) {
            resultado += c;
        }
    }
    return resultado;
}

bool Cliente::cpfValido(const string& cpf) const
{
    bool tamanhoCerto = cpf.length() == 11;
    return tamanhoCerto;
}

bool Cliente::telefoneValido(const string& telefone) const
{
    bool tamanhoCerto = telefone.length() == 10 || telefone.length() == 11;
    return tamanhoCerto;
}

void Cliente::exibir(ostream& saida) const
{
    saida << "ID: " << id << endl;
    saida << "Nome: " << nome << endl;
    saida << "CPF: " << cpf << endl;
    saida << "Telefone: " << telefone << endl;
}

ostream& operator<<(ostream& saida, const Cliente& cliente)
{
    cliente.exibir(saida);
    return saida;
}

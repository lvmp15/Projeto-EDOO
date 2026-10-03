#include "Cliente.h"

#include <stdexcept>
#include <cctype>

using namespace std;

Cliente::Cliente(const string& nome, const string& cpf, const string& telefone, int id)
{
    // usa os setters para aproveitar as validacoes, igual no Veiculo e na Vaga
    setId(id);
    setNome(nome);
    setCpf(cpf);
    setTelefone(telefone);
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
    // tira so a pontuacao, entao uma letra no meio continua la e o CPF e recusado
    string cpfLimpo = removerCaracteres(cpf, ".- ");
    if (!cpfValido(cpfLimpo)) {
        throw invalid_argument("CPF invalido: " + cpf + " (use 11 digitos, com ou sem ponto e hifen)");
    }
    this->cpf = cpfLimpo;
}

string Cliente::getTelefone() const
{
    return telefone;
}

void Cliente::setTelefone(const string& telefone)
{
    // telefone e opcional, entao vazio e aceito sem passar pela validacao
    if (telefone.empty()) {
        this->telefone = "";
        return;
    }

    string telefoneLimpo = removerCaracteres(telefone, "()- ");
    if (!telefoneValido(telefoneLimpo)) {
        throw invalid_argument("Telefone invalido: " + telefone + " (use DDD e numero, 10 ou 11 digitos)");
    }
    this->telefone = telefoneLimpo;
}

bool Cliente::temTelefone() const
{
    return !telefone.empty();
}


// tira de texto todos os caracteres que aparecem em remover
string Cliente::removerCaracteres(const string& texto, const string& remover) const
{
    string resultado;
    for (unsigned int i = 0; i < texto.length(); i++) {
        if (remover.find(texto[i]) == string::npos) {
            resultado += texto[i];
        }
    }
    return resultado;
}

bool Cliente::todosDigitos(const string& texto) const
{
    for (unsigned int i = 0; i < texto.length(); i++) {
        if (!isdigit((unsigned char) texto[i])) {
            return false;
        }
    }
    return true;
}

// so quantidade e formato, o digito verificador nao e conferido
bool Cliente::cpfValido(const string& cpf) const
{
    return cpf.length() == 11 && todosDigitos(cpf);
}

bool Cliente::telefoneValido(const string& telefone) const
{
    return (telefone.length() == 10 || telefone.length() == 11) && todosDigitos(telefone);
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

#ifndef CLIENTE_H
#define CLIENTE_H

#include <string>
#include <iostream>

using namespace std;

class Cliente {
    private:
        int id;
        string nome;
        string cpf;
        // ponto de atenção: o telefone é opcional, se tiver string vazia quer dizer que o cliente nn tem
        string telefone;

        // funcoes de apoio da validacao, ficam privadas pq so a propria classe usa
        string removerCaracteres(const string& texto, const string& remover) const;
        bool todosDigitos(const string& texto) const;
        bool cpfValido(const string& cpf) const;
        bool telefoneValido(const string& telefone) const;

    public:
        Cliente (const string& nome, const string& cpf, const string& telefone = "", int id = 0);

        int getId() const;
        void setId(int id);


        string getNome() const;
        void setNome(const string& nome);


        string getCpf() const;
        void setCpf(const string& cpf);


        string getTelefone() const;
        void setTelefone(const string& telefone);

        bool temTelefone() const;

        void exibir(ostream& saida) const;

};

ostream& operator<<(ostream& saida, const Cliente& cliente);

#endif

#include "Menu.h"

#include <iostream>
#include <cstdlib>
#include <cerrno>
#include <cctype>

using namespace std;

Menu::Menu()
{
}

void Menu::executar()
{
    int opcao = -1;

    while (opcao != 0)
    {
        exibir();

        // false so acontece no fim da entrada (Ctrl+D ou Ctrl+Z), entao encerra em vez de ficar em loop
        if (!lerInteiro("Opcao: ", 0, 11, opcao))
        {
            cout << "\nEntrada encerrada.\n";
            return;
        }

        executarOpcao(opcao);
    }

    cout << "Ate logo!\n";
}

void Menu::exibir() const
{
    cout << "\n===== ESTACIONAMENTO =====\n";
    cout << " 1. Cadastrar cliente\n";
    cout << " 2. Cadastrar veiculo\n";
    cout << " 3. Cadastrar vaga\n";
    cout << " 4. Registrar entrada\n";
    cout << " 5. Registrar saida\n";
    cout << " 6. Consultar vagas\n";
    cout << " 7. Consultar veiculos\n";
    cout << " 8. Consultar tickets\n";
    cout << " 9. Alterar cadastro\n";
    cout << "10. Excluir cadastro\n";
    cout << "11. Consultar pagamentos\n";
    cout << " 0. Sair\n";
}

void Menu::executarOpcao(int opcao)
{
    switch (opcao)
    {
        case 1:  cadastrarCliente();    break;
        case 2:  cadastrarVeiculo();    break;
        case 3:  cadastrarVaga();       break;
        case 4:  registrarEntrada();    break;
        case 5:  registrarSaida();      break;
        case 6:  consultarVagas();      break;
        case 7:  consultarVeiculos();   break;
        case 8:  consultarTickets();    break;
        case 9:  alterarCadastro();     break;
        case 10: excluirCadastro();     break;
        case 11: consultarPagamentos(); break;
    }
}

void Menu::cadastrarCliente() const    { emConstrucao("Cadastrar cliente"); }
void Menu::cadastrarVeiculo() const    { emConstrucao("Cadastrar veiculo"); }
void Menu::cadastrarVaga() const       { emConstrucao("Cadastrar vaga"); }
void Menu::registrarEntrada() const    { emConstrucao("Registrar entrada"); }
void Menu::registrarSaida() const      { emConstrucao("Registrar saida"); }
void Menu::consultarVagas() const      { emConstrucao("Consultar vagas"); }
void Menu::consultarVeiculos() const   { emConstrucao("Consultar veiculos"); }
void Menu::consultarTickets() const    { emConstrucao("Consultar tickets"); }
void Menu::consultarPagamentos() const { emConstrucao("Consultar pagamentos"); }

void Menu::alterarCadastro() const
{
    const string nomes[] = {"", "cliente", "veiculo", "vaga"};
    int tipo = escolherCadastro("Alterar");

    if (tipo != 0)
    {
        emConstrucao("Alterar " + nomes[tipo]);
    }
}

void Menu::excluirCadastro() const
{
    const string nomes[] = {"", "cliente", "veiculo", "vaga"};
    int tipo = escolherCadastro("Excluir");

    if (tipo != 0)
    {
        emConstrucao("Excluir " + nomes[tipo]);
    }
}

int Menu::escolherCadastro(const string& acao) const
{
    cout << "\n" << acao << " cadastro de:\n";
    cout << "1. Cliente\n";
    cout << "2. Veiculo\n";
    cout << "3. Vaga\n";
    cout << "0. Voltar\n";

    int tipo = 0;

    // no fim da entrada devolve 0 (voltar), e o menu principal encerra na proxima leitura
    if (!lerInteiro("Opcao: ", 0, 3, tipo))
    {
        return 0;
    }

    return tipo;
}

void Menu::emConstrucao(const string& nome) const
{
    cout << "\n[" << nome << "] ainda nao implementado.\n";
}

// repete a pergunta ate vir um inteiro valido dentro de minimo e maximo
bool Menu::lerInteiro(const string& pergunta, int minimo, int maximo, int& valor) const
{
    string linha;

    while (true)
    {
        cout << pergunta;

        // getline falha no fim da entrada, e sem esse teste o loop seria infinito
        if (!getline(cin, linha))
        {
            return false;
        }

        const char* inicio = linha.c_str();
        char* fim = NULL;

        errno = 0;
        long numero = strtol(inicio, &fim, 10);

        // fim == inicio quer dizer que nao leu nenhum digito (vazio ou letras)
        bool leuDigitos = (fim != inicio);

        // espacos depois do numero podem, qualquer outra coisa ("3x", "2 5") nao
        while (leuDigitos && isspace((unsigned char) *fim))
        {
            fim++;
        }

        // ERANGE: numero grande demais ate pro long
        if (leuDigitos && *fim == '\0' && errno != ERANGE && numero >= minimo && numero <= maximo)
        {
            valor = (int) numero;
            return true;
        }

        cout << "Entrada invalida, digite um numero de " << minimo << " a " << maximo << ".\n";
    }
}
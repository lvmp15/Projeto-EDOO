#include "Menu.h"
#include "Carro.h"
#include "Moto.h"
#include "Caminhao.h"

#include <iostream>
#include <cstdlib>
#include <cerrno>
#include <cctype>
#include <climits>

using namespace std;

Menu::Menu(BancoDados& banco)
    : banco(banco), clienteDAO(banco), veiculoDAO(banco), vagaDAO(banco), ticketDAO(banco)
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
    // qualquer erro cancela so a opcao atual e volta pro menu
    try
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
    catch (invalid_argument& e)
    {
        // as classes ja mandam mensagens legiveis, tipo "CPF invalido"
        cout << "\nErro: " << e.what() << "\n";
    }
    catch (runtime_error& e)
    {
        cout << "\nErro: " << traduzirErro(e.what()) << "\n";
    }
}


void Menu::registrarPagamento(Ticket& ticket)
{
    int opcao = 0;

    cout << "\n--- Pagamento ---\n";
    cout << "1. Dinheiro\n2. Cartao\n3. Pix\n";

    if (!lerInteiro("Metodo: ", 1, 3, opcao))
    {
        return;
    }

    string metodo;
    if (opcao == 1) {
        metodo = "Dinheiro";
    }
    else if (opcao == 2) {
        metodo = "Cartao";
    }
    else {
        metodo = "Pix";
    }

    Pagamento pagamento(&ticket, metodo);
    pagamento.confirmar();

    cout << "\nPagamento confirmado: R$ " << pagamento.getValor() << "\n";

    // salvar no banco quando existir o PagamentoDAO (sprint 2 05)
}

void Menu::cadastrarCliente()
{
    string nome, cpf, telefone;

    cout << "\n--- Cadastrar cliente ---\n";

    if (!lerTexto("Nome: ", nome) || !lerTexto("CPF: ", cpf) ||
        !lerTexto("Telefone (enter para deixar sem): ", telefone, true))
    {
        return;
    }

    Cliente cliente(nome, cpf, telefone);
    clienteDAO.inserir(cliente);

    cout << "\nCliente cadastrado:\n" << cliente;
}

void Menu::cadastrarVeiculo()
{
    string placa, modelo, tipo;
    int clienteId = 0;

    cout << "\n--- Cadastrar veiculo ---\n";

    if (!lerTexto("Placa: ", placa) || !lerTexto("Modelo: ", modelo) ||
        !lerTexto("Tipo (Carro, Moto ou Caminhao): ", tipo))
    {
        return;
    }

    // aceita "carro", "CARRO", "Carro"...
    for (unsigned int i = 0; i < tipo.length(); i++)
    {
        tipo[i] = (char) tolower(tipo[i]);
    }

    
    if (tipo != "carro" && tipo != "moto" && tipo != "caminhao")
    {
        throw invalid_argument("Tipo de veiculo invalido, use Carro, Moto ou Caminhao");
    }

    if (!lerInteiro("Id do cliente dono: ", 1, INT_MAX, clienteId))
    {
        return;
    }

    // busca antes para dar "cliente nao encontrado" em vez do erro de chave estrangeira
    Cliente dono = clienteDAO.buscarPorId(clienteId);

    if (tipo == "carro")
    {
        Carro carro(placa, modelo, clienteId);
        veiculoDAO.inserir(carro);
        cout << "\nVeiculo cadastrado para " << dono.getNome() << ":\n" << carro << "\n";
    }
    else if (tipo == "moto")
    {
        Moto moto(placa, modelo, clienteId);
        veiculoDAO.inserir(moto);
        cout << "\nVeiculo cadastrado para " << dono.getNome() << ":\n" << moto << "\n";
    }
    else 
    {
        Caminhao caminhao(placa, modelo, clienteId);
        veiculoDAO.inserir(caminhao);
        cout << "\nVeiculo cadastrado para " << dono.getNome() << ":\n" << caminhao << "\n";
    }
}

void Menu::cadastrarVaga()
{
    int numero = 0;
    string tipo;

    cout << "\n--- Cadastrar vaga ---\n";

    if (!lerInteiro("Numero: ", 1, INT_MAX, numero) || !lerTexto("Tipo (Carro, Moto ou Caminhao): ", tipo))
    {
        return;
    }

    Vaga vaga(numero, tipo);
    vagaDAO.inserir(vaga);

    cout << "\nVaga cadastrada:\n" << vaga << "\n";
}

void Menu::registrarEntrada()
{
    string placa;

    cout << "\n--- Registrar entrada ---\n";

    if (!lerTexto("Placa: ", placa))
    {
        return;
    }

    // no banco a placa fica sem traco e maiuscula, igual o setPlaca do Veiculo deixa
    string placaBusca;
    for (unsigned int i = 0; i < placa.length(); i++)
    {
        if (placa[i] != '-' && placa[i] != ' ')
        {
            placaBusca += (char) toupper(placa[i]);
        }
    }

    // se a placa nao existir o DAO lanca "Veiculo nao encontrado" e volta pro menu
    unique_ptr<Veiculo> veiculo = veiculoDAO.buscarPorPlaca(placaBusca);

    if (ticketDAO.temTicketAberto(veiculo->getId()))
    {
        Ticket aberto = ticketDAO.buscarAbertoPorVeiculo(veiculo->getId());
        Vaga vagaAtual = vagaDAO.buscarPorId(aberto.getVagaId());

        cout << "\nO veiculo " << veiculo->getPlaca() << " ja esta no estacionamento, na vaga "
             << vagaAtual.getNumero() << " (ticket " << aberto.getId() << ").\n";
        return;
    }

    // o laco nao sabe se e Carro, Moto ou Caminhao, so pergunta se a vaga aceita
    vector<Vaga> livres = vagaDAO.listarLivres();
    int posicao = -1;

    for (unsigned int i = 0; i < livres.size(); i++)
    {
        if (livres[i].aceita(*veiculo))
        {
            posicao = i;
            break;
        }
    }

    if (posicao == -1)
    {
        cout << "\nNao ha vaga livre para " << veiculo->getTipo() << " no momento.\n";
        return;
    }

    Vaga& vaga = livres[posicao];
    Ticket ticket(veiculo->getId(), vaga.getId(), time(NULL));

    // so muda o objeto, por isso fica fora da transacao
    vaga.ocupar();

    // ticket e vaga gravam juntos ou nenhum dos dois
    banco.executar("BEGIN;");

    try
    {
        ticketDAO.inserir(ticket);

        if (!vagaDAO.atualizar(vaga))
        {
            throw runtime_error("Vaga nao encontrada, entrada cancelada.");
        }

        banco.executar("COMMIT;");
    }
    catch (...)
    {
        // se o sqlite ja desfez sozinho o ROLLBACK falha, e o erro que importa e o de cima
        try
        {
            banco.executar("ROLLBACK;");
        }
        catch (runtime_error&)
        {
        }
        throw;
    }

    cout << "\nEntrada registrada:\n";
    cout << "Ticket: " << ticket.getId()
         << " | Placa: " << veiculo->getPlaca()
         << " | Vaga: " << vaga.getNumero()
         << " | Entrada: " << formatarData(ticket.getEntrada()) << "\n";
}

void Menu::registrarSaida() const     { emConstrucao("Registrar saida"); }
void Menu::consultarVagas() const      { emConstrucao("Consultar vagas"); }
void Menu::consultarVeiculos() const   { emConstrucao("Consultar veiculos"); }
void Menu::consultarTickets() const    { emConstrucao("Consultar tickets"); }
void Menu::consultarPagamentos() const { emConstrucao("Consultar pagamentos"); }

void Menu::alterarCadastro()
{
    switch (escolherCadastro("Alterar"))
    {
        case 1: alterarCliente(); break;
        case 2: alterarVeiculo(); break;
        case 3: alterarVaga();    break;
    }
}

void Menu::excluirCadastro()
{
    switch (escolherCadastro("Excluir"))
    {
        case 1: excluirCliente(); break;
        case 2: excluirVeiculo(); break;
        case 3: excluirVaga();    break;
    }
}

void Menu::alterarCliente()
{
    int id = 0;
    int campo = 0;
    string valor;

    if (!lerInteiro("Id do cliente: ", 1, INT_MAX, id))
    {
        return;
    }

    Cliente cliente = clienteDAO.buscarPorId(id);
    cout << "\nCliente encontrado:\n" << cliente;

    cout << "\nAlterar:\n";
    cout << "1. Nome\n";
    cout << "2. CPF\n";
    cout << "3. Telefone\n";
    cout << "0. Voltar\n";

    if (!lerInteiro("Opcao: ", 0, 3, campo) || campo == 0)
    {
        return;
    }

    if (campo == 1)
    {
        if (!lerTexto("Novo nome: ", valor))
        {
            return;
        }
        cliente.setNome(valor);
    }
    else if (campo == 2)
    {
        if (!lerTexto("Novo CPF: ", valor))
        {
            return;
        }
        cliente.setCpf(valor);
    }
    else
    {
        if (!lerTexto("Novo telefone (enter para deixar sem): ", valor, true))
        {
            return;
        }
        cliente.setTelefone(valor);
    }

    if (clienteDAO.atualizar(cliente))
    {
        cout << "\nCliente alterado:\n" << cliente;
    }
    else
    {
        cout << "\nCliente nao encontrado, nada foi alterado.\n";
    }
}

void Menu::alterarVeiculo()
{
    int id = 0;
    int campo = 0;
    string valor;

    if (!lerInteiro("Id do veiculo: ", 1, INT_MAX, id))
    {
        return;
    }

    // o DAO devolve unique_ptr pq Veiculo e abstrato
    unique_ptr<Veiculo> veiculo = veiculoDAO.buscarPorId(id);
    cout << "\nVeiculo encontrado:\n" << *veiculo << "\n";

    // o tipo nao muda aqui, pq trocaria a classe do objeto (Carro para Moto)
    cout << "\nAlterar:\n";
    cout << "1. Placa\n";
    cout << "2. Modelo\n";
    cout << "3. Cliente dono\n";
    cout << "0. Voltar\n";

    if (!lerInteiro("Opcao: ", 0, 3, campo) || campo == 0)
    {
        return;
    }

    if (campo == 1)
    {
        if (!lerTexto("Nova placa: ", valor))
        {
            return;
        }
        veiculo->setPlaca(valor);
    }
    else if (campo == 2)
    {
        if (!lerTexto("Novo modelo: ", valor))
        {
            return;
        }
        veiculo->setModelo(valor);
    }
    else
    {
        int clienteId = 0;
        if (!lerInteiro("Id do novo dono: ", 1, INT_MAX, clienteId))
        {
            return;
        }

        // confere se o cliente existe antes de trocar
        clienteDAO.buscarPorId(clienteId);
        veiculo->setClienteId(clienteId);
    }

    if (veiculoDAO.atualizar(*veiculo))
    {
        cout << "\nVeiculo alterado:\n" << *veiculo << "\n";
    }
    else
    {
        cout << "\nVeiculo nao encontrado, nada foi alterado.\n";
    }
}

void Menu::alterarVaga()
{
    int id = 0;
    int campo = 0;

    if (!lerInteiro("Id da vaga: ", 1, INT_MAX, id))
    {
        return;
    }

    Vaga vaga = vagaDAO.buscarPorId(id);
    cout << "\nVaga encontrada:\n" << vaga << "\n";

    cout << "\nAlterar:\n";
    cout << "1. Numero\n";
    cout << "2. Tipo\n";
    cout << "0. Voltar\n";

    if (!lerInteiro("Opcao: ", 0, 2, campo) || campo == 0)
    {
        return;
    }

    if (campo == 1)
    {
        int numero = 0;
        if (!lerInteiro("Novo numero: ", 1, INT_MAX, numero))
        {
            return;
        }
        vaga.setNumero(numero);
    }
    else
    {
        string tipo;
        if (!lerTexto("Novo tipo (Carro, Moto ou Caminhao): ", tipo))
        {
            return;
        }
        vaga.setTipo(tipo);
    }

    if (vagaDAO.atualizar(vaga))
    {
        cout << "\nVaga alterada:\n" << vaga << "\n";
    }
    else
    {
        cout << "\nVaga nao encontrada, nada foi alterado.\n";
    }
}

void Menu::excluirCliente()
{
    int id = 0;

    if (!lerInteiro("Id do cliente: ", 1, INT_MAX, id))
    {
        return;
    }

    Cliente cliente = clienteDAO.buscarPorId(id);
    cout << "\nCliente encontrado:\n" << cliente;

    if (!confirmar("Confirma a exclusao?"))
    {
        cout << "Exclusao cancelada.\n";
        return;
    }

    bool removido = false;

    try
    {
        removido = clienteDAO.remover(id);
    }
    catch (runtime_error& e)
    {
        if (ehErroDeChaveEstrangeira(e))
        {
            cout << "\nErro: esse cliente tem veiculos cadastrados, exclua os veiculos dele antes.\n";
            return;
        }
        throw;
    }

    if (removido)
    {
        cout << "Cliente excluido.\n";
    }
    else
    {
        cout << "Cliente nao encontrado, nada foi excluido.\n";
    }
}

void Menu::excluirVeiculo()
{
    int id = 0;

    if (!lerInteiro("Id do veiculo: ", 1, INT_MAX, id))
    {
        return;
    }

    unique_ptr<Veiculo> veiculo = veiculoDAO.buscarPorId(id);
    cout << "\nVeiculo encontrado:\n" << *veiculo << "\n";

    if (!confirmar("Confirma a exclusao?"))
    {
        cout << "Exclusao cancelada.\n";
        return;
    }

    bool removido = false;

    try
    {
        removido = veiculoDAO.remover(id);
    }
    catch (runtime_error& e)
    {
        if (ehErroDeChaveEstrangeira(e))
        {
            cout << "\nErro: esse veiculo tem tickets registrados e nao pode ser excluido.\n";
            return;
        }
        throw;
    }

    if (removido)
    {
        cout << "Veiculo excluido.\n";
    }
    else
    {
        cout << "Veiculo nao encontrado, nada foi excluido.\n";
    }
}

void Menu::excluirVaga()
{
    int id = 0;

    if (!lerInteiro("Id da vaga: ", 1, INT_MAX, id))
    {
        return;
    }

    Vaga vaga = vagaDAO.buscarPorId(id);
    cout << "\nVaga encontrada:\n" << vaga << "\n";

    if (!confirmar("Confirma a exclusao?"))
    {
        cout << "Exclusao cancelada.\n";
        return;
    }

    bool removido = false;

    try
    {
        removido = vagaDAO.remover(id);
    }
    catch (runtime_error& e)
    {
        if (ehErroDeChaveEstrangeira(e))
        {
            cout << "\nErro: essa vaga tem tickets registrados e nao pode ser excluida.\n";
            return;
        }
        throw;
    }

    if (removido)
    {
        cout << "Vaga excluida.\n";
    }
    else
    {
        cout << "Vaga nao encontrada, nada foi excluido.\n";
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

// troca a mensagem tecnica do sqlite por uma que o usuario entenda
string Menu::traduzirErro(const string& mensagem) const
{
    if (mensagem.find("UNIQUE constraint failed: cliente.cpf") != string::npos)
    {
        return "Ja existe um cliente com esse CPF.";
    }

    if (mensagem.find("UNIQUE constraint failed: veiculo.placa") != string::npos)
    {
        return "Ja existe um veiculo com essa placa.";
    }

    if (mensagem.find("UNIQUE constraint failed: vaga.numero") != string::npos)
    {
        return "Ja existe uma vaga com esse numero.";
    }

    if (mensagem.find("FOREIGN KEY constraint failed") != string::npos)
    {
        return "O registro esta ligado a outro cadastro, operacao cancelada.";
    }

    // erro do banco que nao foi previsto tambem nao vai cru para a tela
    if (mensagem.find("Erro no banco") == 0 || mensagem.find("Erro ao preparar SQL") == 0)
    {
        return "Nao foi possivel concluir a operacao no banco de dados.";
    }

    // o resto ja vem legivel dos DAOs, tipo "Cliente nao encontrado: id 7"
    return mensagem;
}

bool Menu::ehErroDeChaveEstrangeira(const runtime_error& erro) const
{
    return string(erro.what()).find("FOREIGN KEY constraint failed") != string::npos;
}

// mesmo formato e horario local que o TicketDAO grava no banco
string Menu::formatarData(time_t data) const
{
    char texto[20];

    strftime(texto, sizeof(texto), "%Y-%m-%d %H:%M:%S", localtime(&data));

    return texto;
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

// repete a pergunta ate vir um texto nao vazio, a nao ser que podeVazio seja true
bool Menu::lerTexto(const string& pergunta, string& valor, bool podeVazio) const
{
    string linha;

    while (true)
    {
        cout << pergunta;

        // mesmo cuidado do lerInteiro: no fim da entrada devolve false
        if (!getline(cin, linha))
        {
            return false;
        }

        // tira os espacos das pontas, entao "   " conta como vazio
        unsigned int inicio = 0;
        unsigned int fim = linha.length();

        while (inicio < fim && isspace((unsigned char) linha[inicio]))
        {
            inicio++;
        }

        while (fim > inicio && isspace((unsigned char) linha[fim - 1]))
        {
            fim--;
        }

        linha = linha.substr(inicio, fim - inicio);

        if (!linha.empty() || podeVazio)
        {
            valor = linha;
            return true;
        }

        cout << "Entrada invalida, o campo nao pode ficar vazio.\n";
    }
}

// so devolve true com s; no fim da entrada conta como nao
bool Menu::confirmar(const string& pergunta) const
{
    string resposta;

    while (lerTexto(pergunta + " (s/n): ", resposta))
    {
        if (resposta == "s" || resposta == "S")
        {
            return true;
        }

        if (resposta == "n" || resposta == "N")
        {
            return false;
        }

        cout << "Responda s ou n.\n";
    }

    return false;
}

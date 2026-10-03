#include "Menu.h"
#include "Carro.h"
#include "Moto.h"
#include "Caminhao.h"

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cerrno>
#include <cctype>
#include <climits>

using namespace std;

// a referencia e os DAOs, que nao tem construtor padrao, so podem ser iniciados nessa lista
Menu::Menu(BancoDados& banco)
    : banco(banco), clienteDAO(banco), veiculoDAO(banco), vagaDAO(banco), ticketDAO(banco),
      pagamentoDAO(banco)
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
    catch (exception&)
    {
        // rede de seguranca: o logic_error da Vaga e erros da biblioteca padrao nao fecham o programa,
        // e a mensagem tecnica deles nao vai pra tela
        cout << "\nErro interno, a operacao foi cancelada.\n";
    }
}


Pagamento Menu::registrarPagamento(Ticket& ticket)
{
    int opcao = 0;

    cout << "\n--- Pagamento ---\n";
    cout << "1. Dinheiro\n2. Cartao\n3. Pix\n";

    // sem metodo nao tem pagamento, e nada foi gravado ainda
    if (!lerInteiro("Metodo: ", 1, 3, opcao))
    {
        throw runtime_error("Pagamento cancelado, a saida nao foi registrada.");
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

    // passa o endereco do ticket, que e variavel do registrarSaida e continua vivo enquanto o pagamento existir
    Pagamento pagamento(&ticket, metodo);
    pagamento.confirmar();

    return pagamento;
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

    // sem cliente nao tem dono pra escolher, entao nem pede os dados do veiculo
    if (!mostrarClientes())
    {
        return;
    }

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

    // se a placa nao existir o DAO lanca "Veiculo nao encontrado" e volta pro menu
    unique_ptr<Veiculo> veiculo = veiculoDAO.buscarPorPlaca(normalizarPlaca(placa));

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

    // referencia: o ocupar() muda a propria vaga do vetor, sem fazer copia
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

void Menu::registrarSaida()
{
    string placa;

    cout << "\n--- Registrar saida ---\n";

    if (!lerTexto("Placa: ", placa))
    {
        return;
    }

    unique_ptr<Veiculo> veiculo = veiculoDAO.buscarPorPlaca(normalizarPlaca(placa));

    if (!ticketDAO.temTicketAberto(veiculo->getId()))
    {
        cout << "\nO veiculo " << veiculo->getPlaca() << " nao esta no estacionamento.\n";
        return;
    }

    Ticket ticket = ticketDAO.buscarAbertoPorVeiculo(veiculo->getId());
    Vaga vaga = vagaDAO.buscarPorId(ticket.getVagaId());

    // a tarifa sai do calcularTarifa do veiculo, que e virtual, o menu nao faz conta
    ticket.registrarSaida(time(NULL), *veiculo);

    cout << "\nPlaca: " << veiculo->getPlaca() << " | Tipo: " << veiculo->getTipo()
         << " | Vaga: " << vaga.getNumero() << "\n";
    cout << "Entrada: " << formatarData(ticket.getEntrada()) << "\n";
    cout << "Saida: " << formatarData(ticket.getSaida()) << "\n";
    cout << "Permanencia: " << formatarPermanencia(ticket.getEntrada(), ticket.getSaida()) << "\n";
    cout << fixed << setprecision(2);
    cout << "Valor a pagar: R$ " << ticket.getValor() << "\n";

    Pagamento pagamento = registrarPagamento(ticket);

    // vaga livre com ticket aberto so acontece com o banco inconsistente,
    // ai o veiculo sai mesmo assim, sem o liberar() lancar logic_error
    if (vaga.estaOcupada())
    {
        vaga.liberar();
    }

    // ticket, vaga e pagamento gravam juntos ou nenhum dos tres
    banco.executar("BEGIN;");

    try
    {
        if (!ticketDAO.atualizar(ticket))
        {
            throw runtime_error("Ticket nao encontrado, saida cancelada.");
        }

        if (!vagaDAO.atualizar(vaga))
        {
            throw runtime_error("Vaga nao encontrada, saida cancelada.");
        }

        pagamentoDAO.inserir(pagamento);

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

    cout << "\nPagamento confirmado: R$ " << pagamento.getValor() << " (" << pagamento.getMetodo() << ")\n";
    cout << "Saida registrada, vaga " << vaga.getNumero() << " liberada.\n";
}

void Menu::consultarVagas() const
{
    int filtro = 0;

    cout << "\n--- Consultar vagas ---\n";
    cout << "1. Todas\n";
    cout << "2. So as livres\n";
    cout << "3. So as ocupadas\n";
    cout << "0. Voltar\n";

    if (!lerInteiro("Opcao: ", 0, 3, filtro) || filtro == 0)
    {
        return;
    }

    vector<Vaga> vagas = vagaDAO.listarTodas();

    if (vagas.empty())
    {
        cout << "\nNao ha vagas cadastradas.\n";
        return;
    }

    // o resumo conta todas as vagas, o filtro so decide quais linhas aparecem
    int livres = 0;
    int ocupadas = 0;

    for (unsigned int i = 0; i < vagas.size(); i++)
    {
        if (vagas[i].estaOcupada())
        {
            ocupadas++;
        }
        else
        {
            livres++;
        }
    }

    if (filtro == 2 && livres == 0)
    {
        cout << "\nNao ha vagas livres no momento.\n";
    }
    else if (filtro == 3 && ocupadas == 0)
    {
        cout << "\nNao ha vagas ocupadas no momento.\n";
    }
    else
    {
        cout << "\n" << left << setw(6) << "Id" << setw(8) << "Numero" << setw(10) << "Tipo" << "Situacao\n";
        cout << string(32, '-') << "\n";

        for (unsigned int i = 0; i < vagas.size(); i++)
        {
            bool mostrar = filtro == 1 || (filtro == 2 && !vagas[i].estaOcupada()) ||
                           (filtro == 3 && vagas[i].estaOcupada());

            if (mostrar)
            {
                cout << setw(6) << vagas[i].getId() << setw(8) << vagas[i].getNumero()
                     << setw(10) << vagas[i].getTipo() << (vagas[i].estaOcupada() ? "Ocupada" : "Livre") << "\n";
            }
        }
    }

    cout << "\nLivres: " << livres << " | Ocupadas: " << ocupadas << " | Total: " << vagas.size() << "\n";
}

void Menu::consultarVeiculos() const
{
    cout << "\n--- Consultar veiculos ---\n";

    vector<unique_ptr<Veiculo>> veiculos = veiculoDAO.listarTodos();

    if (veiculos.empty())
    {
        cout << "\nNao ha veiculos cadastrados.\n";
        return;
    }

    cout << "\n" << left << setw(5) << "Id" << setw(10) << "Placa" << setw(16) << "Modelo"
         << setw(10) << "Tipo" << "Dono\n";
    cout << string(60, '-') << "\n";

    for (unsigned int i = 0; i < veiculos.size(); i++)
    {
        // o veiculo so guarda o id do dono, o nome vem do ClienteDAO
        Cliente dono = clienteDAO.buscarPorId(veiculos[i]->getClienteId());

        cout << setw(5) << veiculos[i]->getId() << setw(10) << veiculos[i]->getPlaca()
             << setw(16) << veiculos[i]->getModelo() << setw(10) << veiculos[i]->getTipo()
             << dono.getNome() << "\n";
    }

    cout << "\nTotal: " << veiculos.size() << " veiculo(s)\n";
}

void Menu::consultarTickets() const
{
    int filtro = 0;

    cout << "\n--- Consultar tickets ---\n";
    cout << "1. Todos\n";
    cout << "2. So os abertos\n";
    cout << "0. Voltar\n";

    if (!lerInteiro("Opcao: ", 0, 2, filtro) || filtro == 0)
    {
        return;
    }

    vector<Ticket> tickets;

    if (filtro == 1)
    {
        tickets = ticketDAO.listarTodos();
    }
    else
    {
        tickets = ticketDAO.listarAbertos();
    }

    if (tickets.empty())
    {
        if (filtro == 1)
        {
            cout << "\nNao ha tickets registrados.\n";
        }
        else
        {
            cout << "\nNao ha tickets abertos, nenhum veiculo esta no estacionamento.\n";
        }
        return;
    }

    cout << "\n" << left << setw(5) << "Id" << setw(10) << "Placa" << setw(6) << "Vaga"
         << setw(21) << "Entrada" << setw(21) << "Saida" << "Valor (R$)\n";
    cout << string(73, '-') << "\n";

    for (unsigned int i = 0; i < tickets.size(); i++)
    {
        // o ticket so guarda os ids, placa e numero da vaga vem dos outros DAOs
        unique_ptr<Veiculo> veiculo = veiculoDAO.buscarPorId(tickets[i].getVeiculoId());
        Vaga vaga = vagaDAO.buscarPorId(tickets[i].getVagaId());

        cout << setw(5) << tickets[i].getId() << setw(10) << veiculo->getPlaca()
             << setw(6) << vaga.getNumero() << setw(21) << formatarData(tickets[i].getEntrada());

        if (tickets[i].estaAberto())
        {
            cout << setw(21) << "em aberto" << "-\n";
        }
        else
        {
            cout << setw(21) << formatarData(tickets[i].getSaida())
                 << fixed << setprecision(2) << tickets[i].getValor() << "\n";
        }
    }

    cout << "\nTotal: " << tickets.size() << " ticket(s)\n";
}

void Menu::consultarPagamentos() const
{
    cout << "\n--- Consultar pagamentos ---\n";

    // cada pagamento aponta para um ticket desse vetor, que nao pode mudar enquanto eles forem usados
    vector<Ticket> tickets = ticketDAO.listarTodos();
    vector<Pagamento> pagamentos = pagamentoDAO.listarTodos(tickets);

    if (pagamentos.empty())
    {
        cout << "\nNao ha pagamentos registrados.\n";
        return;
    }

    cout << "\n" << left << setw(5) << "Id" << setw(8) << "Ticket" << setw(10) << "Placa"
         << setw(12) << "Valor (R$)" << setw(21) << "Data" << setw(10) << "Metodo" << "Status\n";
    cout << string(76, '-') << "\n";

    double arrecadado = 0.0;

    for (unsigned int i = 0; i < pagamentos.size(); i++)
    {
        Ticket* ticket = pagamentos[i].getTicket();
        unique_ptr<Veiculo> veiculo = veiculoDAO.buscarPorId(ticket->getVeiculoId());

        cout << setw(5) << pagamentos[i].getId() << setw(8) << ticket->getId()
             << setw(10) << veiculo->getPlaca()
             << setw(12) << fixed << setprecision(2) << pagamentos[i].getValor()
             << setw(21) << formatarData(pagamentos[i].getData())
             << setw(10) << pagamentos[i].getMetodo() << pagamentos[i].getStatus() << "\n";

        arrecadado += pagamentos[i].getValor();
    }

    cout << "\nTotal: " << pagamentos.size() << " pagamento(s) | Arrecadado: R$ " << arrecadado << "\n";
}

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

    if (!mostrarClientes() || !lerInteiro("Id do cliente: ", 1, INT_MAX, id))
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

    // o DAO devolve unique_ptr pq Veiculo e abstrato, e ele da o delete sozinho no fim da funcao
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
        if (!mostrarClientes() || !lerInteiro("Id do novo dono: ", 1, INT_MAX, clienteId))
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
        // com veiculo dentro, trocar o tipo deixaria por exemplo um caminhao numa vaga de moto
        if (vaga.estaOcupada())
        {
            cout << "\nA vaga esta ocupada, o tipo so pode mudar depois que o veiculo sair.\n";
            return;
        }

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

    if (!mostrarClientes() || !lerInteiro("Id do cliente: ", 1, INT_MAX, id))
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

bool Menu::mostrarClientes() const
{
    vector<Cliente> clientes = clienteDAO.listarTodos();

    if (clientes.empty())
    {
        cout << "\nNao ha clientes cadastrados, cadastre um cliente antes (opcao 1).\n";
        return false;
    }

    cout << "\nClientes cadastrados:\n";
    cout << left << setw(5) << "Id" << "Nome\n";
    cout << string(30, '-') << "\n";

    for (unsigned int i = 0; i < clientes.size(); i++)
    {
        cout << setw(5) << clientes[i].getId() << clientes[i].getNome() << "\n";
    }

    cout << "\n";
    return true;
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

    // acontece se outro programa (DB Browser, outro terminal) estiver usando o arquivo
    if (mensagem.find("database is locked") != string::npos)
    {
        return "O banco esta em uso por outro programa, feche-o e tente de novo.";
    }

    // erro do banco que nao foi previsto tambem nao vai cru para a tela.
    // "Erro ao executar SQL" vem do BEGIN, COMMIT e ROLLBACK
    if (mensagem.find("Erro no banco") == 0 || mensagem.find("Erro ao preparar SQL") == 0 ||
        mensagem.find("Erro ao executar SQL") == 0)
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

// so pra mostrar, tipo "2h 15min"; a tarifa usa as horas exatas
string Menu::formatarPermanencia(time_t entrada, time_t saida) const
{
    long minutos = (long) (difftime(saida, entrada) / 60);

    return to_string(minutos / 60) + "h " + to_string(minutos % 60) + "min";
}

// no banco a placa fica sem traco e maiuscula, igual o setPlaca do Veiculo deixa
string Menu::normalizarPlaca(const string& placa) const
{
    string normalizada;

    for (unsigned int i = 0; i < placa.length(); i++)
    {
        if (placa[i] != '-' && placa[i] != ' ')
        {
            normalizada += (char) toupper(placa[i]);
        }
    }

    return normalizada;
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

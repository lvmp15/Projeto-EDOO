// o httplib tem que vir primeiro: no Windows ele puxa o windows.h, que da conflito
// de "byte" com o std::byte se algum header nosso ja tiver feito using namespace std
#include "httplib.h"

#include "ServidorWeb.h"
#include "Carro.h"
#include "Moto.h"
#include "Caminhao.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <memory>
#include <vector>
#include <cstdlib>
#include <cerrno>
#include <cctype>
#include <climits>

using namespace std;

namespace
{
    // escapa aspas e barras pra um nome tipo Joao "Z" nao quebrar o JSON
    string textoJson(const string& texto)
    {
        string resultado = "\"";

        for (unsigned int i = 0; i < texto.length(); i++)
        {
            char c = texto[i];

            if (c == '"' || c == '\\')
            {
                resultado += '\\';
                resultado += c;
            }
            else if ((unsigned char) c < 32)
            {
                resultado += ' ';
            }
            else
            {
                resultado += c;
            }
        }

        return resultado + "\"";
    }

    string mensagemJson(const string& mensagem)
    {
        return "{\"mensagem\": " + textoJson(mensagem) + "}";
    }

    string erroJson(const string& erro)
    {
        return "{\"erro\": " + textoJson(erro) + "}";
    }
}

ServidorWeb::ServidorWeb(BancoDados& banco)
    : banco(banco), clienteDAO(banco), veiculoDAO(banco), vagaDAO(banco), ticketDAO(banco),
      pagamentoDAO(banco)
{
}

void ServidorWeb::iniciar(int porta)
{
    httplib::Server servidor;

    // o "npm run build" gera o React pronto em frontend/dist, e o servidor entrega esses arquivos
    bool temFront = servidor.set_mount_point("/", "./frontend/dist");

    servidor.Get("/api/estado", [this](const httplib::Request&, httplib::Response& res) {
        responder(res, [this]() { return estado(); });
    });

    servidor.Post("/api/clientes", [this](const httplib::Request& req, httplib::Response& res) {
        responder(res, [&]() {
            return cadastrarCliente(lerTexto(req, "nome"), lerTexto(req, "cpf"), lerTexto(req, "telefone", true));
        });
    });

    servidor.Post("/api/veiculos", [this](const httplib::Request& req, httplib::Response& res) {
        responder(res, [&]() {
            return cadastrarVeiculo(lerTexto(req, "placa"), lerTexto(req, "modelo"), lerTexto(req, "tipo"),
                                    lerInteiro(req, "clienteId"));
        });
    });

    servidor.Post("/api/vagas", [this](const httplib::Request& req, httplib::Response& res) {
        responder(res, [&]() { return cadastrarVaga(lerInteiro(req, "numero"), lerTexto(req, "tipo")); });
    });

    servidor.Post("/api/vagas/remover", [this](const httplib::Request& req, httplib::Response& res) {
        responder(res, [&]() { return removerVaga(lerInteiro(req, "vagaId")); });
    });

    servidor.Post("/api/entrada", [this](const httplib::Request& req, httplib::Response& res) {
        responder(res, [&]() { return alocarVeiculo(lerInteiro(req, "vagaId"), lerInteiro(req, "veiculoId")); });
    });

    servidor.Post("/api/saida", [this](const httplib::Request& req, httplib::Response& res) {
        responder(res, [&]() { return registrarSaida(lerInteiro(req, "vagaId"), lerTexto(req, "metodo")); });
    });

    cout << "Interface grafica em http://localhost:" << porta << "\n";

    if (!temFront)
    {
        cout << "Aviso: a pasta frontend/dist nao existe, rode \"npm run build\" dentro de frontend antes.\n";
    }

    // endl pra mensagem aparecer antes do listen travar o programa
    cout << "Aperte Ctrl+C para encerrar." << endl;

    // 127.0.0.1 deixa so este computador acessar, ninguem da rede entra
    if (!servidor.listen("127.0.0.1", porta))
    {
        throw runtime_error("Nao foi possivel usar a porta " + to_string(porta) +
                            ", veja se o programa ja nao esta aberto em outro terminal.");
    }
}

string ServidorWeb::estado() const
{
    vector<Cliente> clientes = clienteDAO.listarTodos();
    vector<unique_ptr<Veiculo>> veiculos = veiculoDAO.listarTodos();
    vector<Vaga> vagas = vagaDAO.listarTodas();
    // ticket aberto e veiculo que ainda esta dentro, e ele diz em qual vaga
    vector<Ticket> abertos = ticketDAO.listarAbertos();

    time_t agora = time(NULL);

    ostringstream json;
    json << fixed << setprecision(2);

    json << "{\"clientes\": [";

    for (unsigned int i = 0; i < clientes.size(); i++)
    {
        if (i > 0)
        {
            json << ", ";
        }

        json << "{\"id\": " << clientes[i].getId()
             << ", \"nome\": " << textoJson(clientes[i].getNome())
             << ", \"cpf\": " << textoJson(clientes[i].getCpf()) << "}";
    }

    json << "], \"veiculos\": [";

    for (unsigned int i = 0; i < veiculos.size(); i++)
    {
        string dono;

        for (unsigned int j = 0; j < clientes.size(); j++)
        {
            if (clientes[j].getId() == veiculos[i]->getClienteId())
            {
                dono = clientes[j].getNome();
            }
        }

        // vagaId 0 quer dizer que o veiculo esta fora do estacionamento
        int vagaId = 0;

        for (unsigned int j = 0; j < abertos.size(); j++)
        {
            if (abertos[j].getVeiculoId() == veiculos[i]->getId())
            {
                vagaId = abertos[j].getVagaId();
            }
        }

        if (i > 0)
        {
            json << ", ";
        }

        json << "{\"id\": " << veiculos[i]->getId()
             << ", \"placa\": " << textoJson(veiculos[i]->getPlaca())
             << ", \"modelo\": " << textoJson(veiculos[i]->getModelo())
             << ", \"tipo\": " << textoJson(veiculos[i]->getTipo())
             << ", \"dono\": " << textoJson(dono)
             << ", \"vagaId\": " << vagaId << "}";
    }

    json << "], \"vagas\": [";

    for (unsigned int i = 0; i < vagas.size(); i++)
    {
        if (i > 0)
        {
            json << ", ";
        }

        json << "{\"id\": " << vagas[i].getId()
             << ", \"numero\": " << vagas[i].getNumero()
             << ", \"tipo\": " << textoJson(vagas[i].getTipo())
             << ", \"ocupada\": " << (vagas[i].estaOcupada() ? "true" : "false");

        for (unsigned int j = 0; j < abertos.size(); j++)
        {
            if (abertos[j].getVagaId() != vagas[i].getId())
            {
                continue;
            }

            for (unsigned int k = 0; k < veiculos.size(); k++)
            {
                if (veiculos[k]->getId() == abertos[j].getVeiculoId())
                {
                    // calcularTarifa e virtual, cada tipo de veiculo usa a propria tabela de preco
                    double horas = difftime(agora, abertos[j].getEntrada()) / 3600.0;

                    json << ", \"veiculoId\": " << veiculos[k]->getId()
                         << ", \"entrada\": " << textoJson(formatarData(abertos[j].getEntrada()))
                         << ", \"permanencia\": " << textoJson(formatarPermanencia(abertos[j].getEntrada(), agora))
                         << ", \"valorAtual\": " << veiculos[k]->calcularTarifa(horas);
                }
            }
        }

        json << "}";
    }

    json << "]}";

    return json.str();
}

string ServidorWeb::cadastrarCliente(const string& nome, const string& cpf, const string& telefone)
{
    Cliente cliente(nome, cpf, telefone);
    clienteDAO.inserir(cliente);

    return "{\"mensagem\": " + textoJson("Cliente " + cliente.getNome() + " cadastrado.") +
           ", \"id\": " + to_string(cliente.getId()) + "}";
}

string ServidorWeb::cadastrarVeiculo(const string& placa, const string& modelo, const string& tipo, int clienteId)
{
    Cliente dono = clienteDAO.buscarPorId(clienteId);

    unique_ptr<Veiculo> veiculo;

    if (tipo == "Carro")
    {
        veiculo.reset(new Carro(placa, modelo, clienteId));
    }
    else if (tipo == "Moto")
    {
        veiculo.reset(new Moto(placa, modelo, clienteId));
    }
    else if (tipo == "Caminhao")
    {
        veiculo.reset(new Caminhao(placa, modelo, clienteId));
    }
    else
    {
        throw invalid_argument("Tipo de veiculo invalido, use Carro, Moto ou Caminhao");
    }

    veiculoDAO.inserir(*veiculo);

    return mensagemJson("Veiculo " + veiculo->getPlaca() + " cadastrado para " + dono.getNome() + ".");
}

string ServidorWeb::cadastrarVaga(int numero, const string& tipo)
{
    Vaga vaga(numero, tipo);
    vagaDAO.inserir(vaga);

    return mensagemJson("Vaga " + to_string(vaga.getNumero()) + " (" + vaga.getTipo() + ") adicionada.");
}

string ServidorWeb::removerVaga(int vagaId)
{
    Vaga vaga = vagaDAO.buscarPorId(vagaId);
    string numero = to_string(vaga.getNumero());

    if (vaga.estaOcupada())
    {
        throw runtime_error("A vaga " + numero + " esta ocupada, registre a saida antes de remover.");
    }

    try
    {
        vagaDAO.remover(vagaId);
    }
    catch (runtime_error& e)
    {
        // vaga que ja teve ticket fica presa pela chave estrangeira, pra nao perder o historico
        if (string(e.what()).find("FOREIGN KEY constraint failed") != string::npos)
        {
            throw runtime_error("A vaga " + numero + " ja foi usada e tem tickets, entao nao pode ser removida.");
        }
        throw;
    }

    return mensagemJson("Vaga " + numero + " removida.");
}

// igual o registrarEntrada do Menu, mas quem escolhe a vaga e o usuario no select
string ServidorWeb::alocarVeiculo(int vagaId, int veiculoId)
{
    unique_ptr<Veiculo> veiculo = veiculoDAO.buscarPorId(veiculoId);
    Vaga vaga = vagaDAO.buscarPorId(vagaId);
    string numero = to_string(vaga.getNumero());

    if (ticketDAO.temTicketAberto(veiculo->getId()))
    {
        Ticket aberto = ticketDAO.buscarAbertoPorVeiculo(veiculo->getId());
        Vaga vagaAtual = vagaDAO.buscarPorId(aberto.getVagaId());

        throw runtime_error("O veiculo " + veiculo->getPlaca() + " ja esta na vaga " +
                            to_string(vagaAtual.getNumero()) + ".");
    }

    if (vaga.estaOcupada())
    {
        throw runtime_error("A vaga " + numero + " ja esta ocupada.");
    }

    if (!vaga.aceita(*veiculo))
    {
        throw runtime_error("A vaga " + numero + " so aceita " + vaga.getTipo() + ".");
    }

    Ticket ticket(veiculo->getId(), vaga.getId(), time(NULL));
    vaga.ocupar();

    emTransacao([&]() {
        ticketDAO.inserir(ticket);

        if (!vagaDAO.atualizar(vaga))
        {
            throw runtime_error("Vaga nao encontrada, entrada cancelada.");
        }
    });

    return mensagemJson("Veiculo " + veiculo->getPlaca() + " alocado na vaga " + numero + ".");
}

string ServidorWeb::registrarSaida(int vagaId, const string& metodo)
{
    if (metodo != "Dinheiro" && metodo != "Cartao" && metodo != "Pix")
    {
        throw invalid_argument("Escolha o metodo de pagamento: Dinheiro, Cartao ou Pix");
    }

    Vaga vaga = vagaDAO.buscarPorId(vagaId);
    string numero = to_string(vaga.getNumero());

    vector<Ticket> abertos = ticketDAO.listarAbertos();
    int posicao = -1;

    for (unsigned int i = 0; i < abertos.size(); i++)
    {
        if (abertos[i].getVagaId() == vagaId)
        {
            posicao = i;
            break;
        }
    }

    if (posicao == -1)
    {
        throw runtime_error("Nao tem nenhum veiculo na vaga " + numero + ".");
    }

    // referencia pq o Pagamento guarda o endereco do ticket, e o vetor vive ate o fim da funcao
    Ticket& ticket = abertos[posicao];
    unique_ptr<Veiculo> veiculo = veiculoDAO.buscarPorId(ticket.getVeiculoId());

    ticket.registrarSaida(time(NULL), *veiculo);

    Pagamento pagamento(&ticket, metodo);
    pagamento.confirmar();

    if (vaga.estaOcupada())
    {
        vaga.liberar();
    }

    emTransacao([&]() {
        if (!ticketDAO.atualizar(ticket))
        {
            throw runtime_error("Ticket nao encontrado, saida cancelada.");
        }

        if (!vagaDAO.atualizar(vaga))
        {
            throw runtime_error("Vaga nao encontrada, saida cancelada.");
        }

        pagamentoDAO.inserir(pagamento);
    });

    ostringstream json;
    json << fixed << setprecision(2);

    json << "{\"mensagem\": " << textoJson("Saida registrada, vaga " + numero + " liberada.")
         << ", \"placa\": " << textoJson(veiculo->getPlaca())
         << ", \"permanencia\": " << textoJson(formatarPermanencia(ticket.getEntrada(), ticket.getSaida()))
         << ", \"valor\": " << pagamento.getValor()
         << ", \"metodo\": " << textoJson(pagamento.getMetodo()) << "}";

    return json.str();
}

void ServidorWeb::responder(httplib::Response& resposta, const function<string()>& acao)
{
    lock_guard<mutex> travado(trava);

    try
    {
        resposta.set_content(acao(), "application/json");
    }
    catch (invalid_argument& e)
    {
        resposta.status = 400;
        resposta.set_content(erroJson(e.what()), "application/json");
    }
    catch (runtime_error& e)
    {
        resposta.status = 400;
        resposta.set_content(erroJson(traduzirErro(e.what())), "application/json");
    }
    catch (exception&)
    {
        resposta.status = 500;
        resposta.set_content(erroJson("Erro interno, a operacao foi cancelada."), "application/json");
    }
}

void ServidorWeb::emTransacao(const function<void()>& gravar)
{
    banco.executar("BEGIN;");

    try
    {
        gravar();
        banco.executar("COMMIT;");
    }
    catch (...)
    {
        try
        {
            banco.executar("ROLLBACK;");
        }
        catch (runtime_error&)
        {
        }
        throw;
    }
}

int ServidorWeb::lerInteiro(const httplib::Request& requisicao, const string& campo) const
{
    string texto = requisicao.get_param_value(campo);

    const char* inicio = texto.c_str();
    char* fim = NULL;

    errno = 0;
    long numero = strtol(inicio, &fim, 10);

    if (fim == inicio || *fim != '\0' || errno == ERANGE || numero < 1 || numero > INT_MAX)
    {
        throw invalid_argument("Valor invalido no campo " + campo);
    }

    return (int) numero;
}

string ServidorWeb::lerTexto(const httplib::Request& requisicao, const string& campo, bool podeVazio) const
{
    string texto = requisicao.get_param_value(campo);

    unsigned int inicio = 0;
    unsigned int fim = texto.length();

    while (inicio < fim && isspace((unsigned char) texto[inicio]))
    {
        inicio++;
    }

    while (fim > inicio && isspace((unsigned char) texto[fim - 1]))
    {
        fim--;
    }

    texto = texto.substr(inicio, fim - inicio);

    if (texto.empty() && !podeVazio)
    {
        throw invalid_argument("O campo " + campo + " nao pode ficar vazio");
    }

    return texto;
}

// mesma traducao do Menu
string ServidorWeb::traduzirErro(const string& mensagem) const
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

    if (mensagem.find("database is locked") != string::npos)
    {
        return "O banco esta em uso por outro programa, feche-o e tente de novo.";
    }

    if (mensagem.find("Erro no banco") == 0 || mensagem.find("Erro ao preparar SQL") == 0 ||
        mensagem.find("Erro ao executar SQL") == 0)
    {
        return "Nao foi possivel concluir a operacao no banco de dados.";
    }

    return mensagem;
}

string ServidorWeb::formatarData(time_t data) const
{
    char texto[20];

    strftime(texto, sizeof(texto), "%d/%m %H:%M", localtime(&data));

    return texto;
}

string ServidorWeb::formatarPermanencia(time_t entrada, time_t saida) const
{
    long minutos = (long) (difftime(saida, entrada) / 60);

    return to_string(minutos / 60) + "h " + to_string(minutos % 60) + "min";
}

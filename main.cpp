#include "BancoDados.h"
#include "Menu.h"
#include "ServidorWeb.h"

#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    try
    {
        // ao sair do try o destrutor fecha a conexao sozinho, ate quando da erro
        BancoDados banco("estacionamento.db");

        if (!banco.tabelasExistem())
        {
            std::cout << "Banco vazio ou nao criado.\n";
            std::cout << "Rode antes: sqlite3 estacionamento.db \".read sql/schema.sql\"\n";
            return 1;
        }

        // "estacionamento web" abre a interface grafica, sem nada continua o menu do terminal
        if (argc > 1 && std::string(argv[1]) == "web")
        {
            ServidorWeb servidor(banco);
            servidor.iniciar(8080);
        }
        else
        {
            // o menu so guarda uma referencia pro banco, por isso o banco e criado antes
            Menu menu(banco);
            menu.executar();
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "Erro: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
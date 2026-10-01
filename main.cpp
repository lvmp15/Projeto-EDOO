#include "BancoDados.h"
#include "Menu.h"

#include <iostream>

int main()
{
    try
    {
        BancoDados banco("estacionamento.db");

        if (!banco.tabelasExistem())
        {
            std::cout << "Banco vazio ou nao criado.\n";
            std::cout << "Rode antes: sqlite3 estacionamento.db \".read sql/schema.sql\"\n";
            return 1;
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "Erro ao abrir o banco: " << e.what() << "\n";
        return 1;
    }

    Menu menu;
    menu.executar();

    return 0;
}
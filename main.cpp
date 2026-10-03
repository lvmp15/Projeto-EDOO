#include "BancoDados.h"
#include "Menu.h"

#include <iostream>

int main()
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

        // o menu so guarda uma referencia pro banco, por isso o banco e criado antes
        Menu menu(banco);
        menu.executar();
    }
    catch (const std::exception& e)
    {
        std::cout << "Erro: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
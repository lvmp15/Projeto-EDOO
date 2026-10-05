# Sistema de Administração e Controle de Estacionamento

Projeto da disciplina de Estruturas de Dados Orientadas a Objetos (CIN0135), do Centro de Informática da UFPE.

Programa de terminal em C++ que controla um estacionamento. Ele cadastra clientes, veículos (carro, moto ou caminhão) e vagas, registra a entrada do veículo numa vaga livre do mesmo tipo e, na saída, calcula a tarifa e registra o pagamento. Os dados ficam num banco SQLite (`estacionamento.db`).

## Integrantes / Login

- Arthur Guerra Laranjeira Salazar / agls
- João Pedro Monteiro da Cunha Santos / jpmcs
- Lucas Veloso Moura Pereira / lvmp
- Matheus de Assis Lins / mal5

## Requisitos

- Compilador com suporte a C++17 (g++)
- make
- SQLite 3: a biblioteca de desenvolvimento (`sqlite3.h` e `-lsqlite3`) e o programa de linha de comando `sqlite3`, usado para criar o banco

## Instalando as dependências

### Windows (MSYS2 / UCRT64)

Com o [MSYS2](https://www.msys2.org/) instalado, abra o terminal **MSYS2 UCRT64** e rode:

```
pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make mingw-w64-ucrt-x86_64-sqlite3
```

O pacote do sqlite3 já traz a biblioteca e o programa `sqlite3`.

Depois coloque `C:\msys64\ucrt64\bin` no `Path` do Windows. Sem isso o PowerShell não acha `g++`, `mingw32-make` nem `sqlite3`, e o executável não acha as DLLs na hora de rodar. Para conferir, num PowerShell novo:

```
g++ --version
mingw32-make --version
sqlite3 --version
```

### Linux e Mac

Os pacotes equivalentes resolvem. No Ubuntu/Debian:

```
sudo apt install build-essential sqlite3 libsqlite3-dev
```

No Mac, as ferramentas de linha de comando do Xcode (`xcode-select --install`) já trazem compilador, make e SQLite.

## Compilando

Todos os comandos daqui em diante são rodados na raiz do projeto (a pasta que tem o `Makefile`).

**No Windows o comando é `mingw32-make`, não `make`.** O MSYS2 instala o make com esse nome, e no PowerShell `make` dá "não é reconhecido como nome de cmdlet".

Windows:

```
mingw32-make
```

Linux e Mac:

```
make
```

O resultado é o executável `estacionamento.exe` no Windows e `estacionamento` no Linux/Mac.

Para apagar os `.o`, `.d` e o executável:

```
mingw32-make clean
```

(`make clean` no Linux/Mac.)

## Criando o banco

Antes de rodar pela primeira vez é preciso criar as tabelas (`schema.sql`) e, se quiser dados de exemplo, preencher (`seed.sql`):

```
sqlite3 estacionamento.db ".read sql/schema.sql"
sqlite3 estacionamento.db ".read sql/seed.sql"
```

Os dois comandos não imprimem nada quando dão certo.

**No PowerShell não use `sqlite3 estacionamento.db < sql/schema.sql`.** O PowerShell não aceita o operador `<` e dá o erro "Operador '<' reservado para uso futuro". O `.read` entre aspas funciona no PowerShell, no cmd e no terminal do Linux/Mac.

Se o programa rodar sem o banco criado, ele avisa e fecha:

```
Banco vazio ou nao criado.
Rode antes: sqlite3 estacionamento.db ".read sql/schema.sql"
```

Os scripts só funcionam num banco novo. Rodar o `schema.sql` de novo dá "table cliente already exists", e o `seed.sql` de novo dá "UNIQUE constraint failed". Para começar do zero, apague o arquivo e rode os dois comandos outra vez:

```
Remove-Item estacionamento.db
```

(`rm estacionamento.db` no Linux/Mac.)

## Rodando

Windows (PowerShell):

```
.\estacionamento.exe
```

Linux e Mac:

```
./estacionamento
```

Rode sempre a partir da raiz do projeto, porque o programa abre o `estacionamento.db` da pasta atual.

## Dados do seed

O `seed.sql` cria:

- 3 clientes: Ana Lima, Bruno Souza e Carla Nunes
- 4 veículos: ABC1234 (Gol, carro) e DEF5678 (Civic, carro) da Ana, GHI1J23 (Biz 125, moto) do Bruno e KLM4N56 (Accelo, caminhão) da Carla
- 10 vagas, todas livres: 1 a 6 para carro, 7 a 9 para moto e 10 para caminhão

Nenhum ticket nem pagamento vem pronto. Para testar a saída, primeiro registre uma entrada (opção 4) com uma dessas placas.

## Menu

```
===== ESTACIONAMENTO =====
 1. Cadastrar cliente
 2. Cadastrar veiculo
 3. Cadastrar vaga
 4. Registrar entrada
 5. Registrar saida
 6. Consultar vagas
 7. Consultar veiculos
 8. Consultar tickets
 9. Alterar cadastro
10. Excluir cadastro
11. Consultar pagamentos
 0. Sair
```

1. Cadastrar cliente: pede nome, CPF (11 dígitos, com ou sem ponto e hífen) e telefone, que é opcional.
2. Cadastrar veículo: pede placa (formato antigo ABC1234 ou Mercosul ABC1D23), modelo, tipo e o id do cliente dono.
3. Cadastrar vaga: pede o número e o tipo (Carro, Moto ou Caminhao).
4. Registrar entrada: pela placa, coloca o veículo na primeira vaga livre do mesmo tipo e abre um ticket.
5. Registrar saída: pela placa, fecha o ticket, mostra a permanência e o valor, pede o método de pagamento (dinheiro, cartão ou Pix) e libera a vaga.
6. Consultar vagas: lista todas, só as livres ou só as ocupadas, com o total de cada.
7. Consultar veículos: lista os veículos com tipo e nome do dono.
8. Consultar tickets: lista todos os tickets ou só os abertos (veículos que estão no estacionamento).
9. Alterar cadastro: altera um campo de um cliente, veículo ou vaga.
10. Excluir cadastro: exclui um cliente, veículo ou vaga, pedindo confirmação. Não deixa excluir cliente que tem veículo, nem veículo ou vaga que já têm ticket.
11. Consultar pagamentos: lista os pagamentos e o total arrecadado.

A tarifa depende do tipo do veículo. Até 1 hora paga só a taxa fixa; depois disso soma a taxa por hora, proporcional ao tempo:

- Carro: R$ 10,00 + R$ 2,50 por hora a mais
- Moto: R$ 5,00 + R$ 1,50 por hora a mais
- Caminhão: R$ 15,00 + R$ 5,00 por hora a mais

## Interface web

Além do menu, o programa tem uma interface no navegador, feita em React (pasta `frontend/`). Ela mostra o pátio com as vagas e permite cadastrar cliente, veículo e vaga, remover vaga, colocar um veículo numa vaga e registrar a saída com pagamento. Usa o mesmo `estacionamento.db`.

Precisa do Node.js 20.19+ ou 22.12+ (exigência do Vite). Primeiro gere os arquivos da interface (só uma vez):

```
cd frontend
npm install
npm run build
cd ..
```

Depois, na raiz do projeto:

```
.\estacionamento.exe web
```

(`./estacionamento web` no Linux/Mac.) O programa mostra:

```
Interface grafica em http://localhost:8080
Aperte Ctrl+C para encerrar.
```

Abra esse endereço no navegador. O servidor só aceita conexões do próprio computador.

## Estrutura de pastas

- `include/`: os headers (`.h`) de todas as classes. O `httplib.h` é a biblioteca [cpp-httplib](https://github.com/yhirose/cpp-httplib), usada só pela interface web.
- `src/`: as implementações (`.cpp`) das classes.
- `sql/`: `schema.sql` (cria as tabelas) e `seed.sql` (dados de exemplo).
- `frontend/`: a interface web em React.
- `docs/`: documentação do projeto.
- Na raiz: `main.cpp`, o `Makefile` e este README. O executável e o `estacionamento.db` também são criados aqui, e ficam fora do git pelo `.gitignore`.

## Arquitetura

O código está dividido em três partes.

**Classes de domínio.** Representam o estacionamento e fazem as validações (CPF, placa, tipo de vaga), sem saber nada de banco.

- `Veiculo` é abstrata. `Carro`, `Moto` e `Caminhao` herdam dela e cada uma implementa `calcularTarifa()` e `getTipo()`. O resto do código trabalha com `Veiculo&` ou `unique_ptr<Veiculo>` e não precisa saber qual é o tipo.
- `Cliente` é o dono dos veículos.
- `Vaga` tem número, tipo e se está ocupada. O `aceita()` diz se um veículo pode usar a vaga.
- `Ticket` registra entrada e saída de um veículo numa vaga. Na saída, pega o valor do `calcularTarifa()` do veículo.
- `Pagamento` guarda o valor, a data e o método de pagamento de um ticket fechado.

**DAOs.** `ClienteDAO`, `VeiculoDAO`, `VagaDAO`, `TicketDAO` e `PagamentoDAO`. Todo SQL que lê ou grava nas tabelas fica aqui: inserir, buscar, listar, atualizar e remover. O `VeiculoDAO` lê a coluna `tipo` e cria o `Carro`, a `Moto` ou o `Caminhao` certo.

**BancoDados.** Abre a conexão com o SQLite no construtor e fecha no destrutor. Liga as chaves estrangeiras e confere se as tabelas existem. Os DAOs usam a conexão dele, mas não abrem nem fecham nada.

O `Menu` (terminal) e o `ServidorWeb` (navegador) só conversam com os DAOs. O único comando SQL que eles mandam direto é o de transação (`BEGIN`, `COMMIT`, `ROLLBACK`), pelo `BancoDados`, para que a entrada e a saída gravem ticket, vaga e pagamento juntos ou não gravem nada.

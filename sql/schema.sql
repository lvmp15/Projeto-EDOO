PRAGMA foreign_keys = ON;

CREATE TABLE cliente (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    nome TEXT NOT NULL,
    cpf TEXT NOT NULL UNIQUE,
    telefone TEXT
);

CREATE TABLE veiculo (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    placa TEXT NOT NULL UNIQUE,
    modelo TEXT NOT NULL,
    tipo TEXT NOT NULL CHECK (tipo IN ('Carro', 'Moto', 'Caminhao')),
    cliente_id INTEGER NOT NULL,
    FOREIGN KEY (cliente_id) REFERENCES cliente(id)
);

CREATE TABLE vaga (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    numero INTEGER NOT NULL UNIQUE,
    tipo TEXT NOT NULL CHECK (tipo IN ('Carro', 'Moto', 'Caminhao')),
    -- sqlite nao tem booleano, entao 0 = livre e 1 = ocupada
    ocupada INTEGER NOT NULL DEFAULT 0 CHECK (ocupada IN (0, 1))
);

CREATE TABLE ticket (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    veiculo_id INTEGER NOT NULL,
    vaga_id INTEGER NOT NULL,
    -- datas no formato 'YYYY-MM-DD HH:MM:SS' para poder comparar como texto
    entrada TEXT NOT NULL,
    -- saida NULL quer dizer ticket aberto, o veiculo ainda esta no estacionamento
    saida TEXT CHECK (saida IS NULL OR saida > entrada),
    valor REAL CHECK (valor IS NULL OR valor >= 0),
    FOREIGN KEY (veiculo_id) REFERENCES veiculo(id),
    FOREIGN KEY (vaga_id) REFERENCES vaga(id)
);

CREATE TABLE IF NOT EXISTS pagamento (
    id        INTEGER PRIMARY KEY AUTOINCREMENT,
    ticket_id INTEGER NOT NULL UNIQUE REFERENCES ticket(id),
    valor     REAL NOT NULL,
    data      TEXT NOT NULL,
    metodo    TEXT NOT NULL CHECK (metodo IN ('Dinheiro', 'Cartao', 'Pix')),
    status    TEXT NOT NULL DEFAULT 'Pendente' CHECK (status IN ('Pendente', 'Pago'))
);

-- procurar o ticket aberto de um veiculo e a consulta mais usada na saida
CREATE INDEX idx_ticket_veiculo ON ticket(veiculo_id);

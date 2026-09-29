INSERT INTO cliente (nome, cpf, telefone) VALUES
    ('Ana Lima', '11122233344', '81988887777'),
    ('Bruno Souza', '55566677788', '81977776666'),
    ('Carla Nunes', '99988877766', NULL);

INSERT INTO veiculo (placa, modelo, tipo, cliente_id) VALUES
    ('ABC1234', 'Gol', 'Carro', 1),
    ('DEF5678', 'Civic', 'Carro', 1),
    ('GHI1J23', 'Biz 125', 'Moto', 2),
    ('KLM4N56', 'Accelo', 'Caminhao', 3);

-- 6 vagas de carro, 3 de moto e 1 de caminhao
INSERT INTO vaga (numero, tipo) VALUES
    (1, 'Carro'),
    (2, 'Carro'),
    (3, 'Carro'),
    (4, 'Carro'),
    (5, 'Carro'),
    (6, 'Carro'),
    (7, 'Moto'),
    (8, 'Moto'),
    (9, 'Moto'),
    (10, 'Caminhao');

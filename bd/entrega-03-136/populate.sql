DROP TABLE IF EXISTS categoria cascade;
DROP TABLE IF EXISTS categoria_simples cascade;
DROP TABLE IF EXISTS super_categoria cascade;
DROP TABLE IF EXISTS tem_outra cascade;
DROP TABLE IF EXISTS produto cascade;
DROP TABLE IF EXISTS tem_categoria cascade;
DROP TABLE IF EXISTS IVM cascade;
DROP TABLE IF EXISTS ponto_de_retalho cascade;
DROP TABLE IF EXISTS instalada_em cascade;
DROP TABLE IF EXISTS prateleira cascade;
DROP TABLE IF EXISTS planograma cascade;
DROP TABLE IF EXISTS retalhista cascade;
DROP TABLE IF EXISTS responsavel_por cascade;
DROP TABLE IF EXISTS evento_reposicao cascade;

----------------------------------------
-- Table Creation
----------------------------------------
-- Named constraints are global to the database.
-- Therefore the following use the following naming rules:
--   1. pk_table for names of primary key constraints
--   2. fk_table_another for names of foreign key constraints

CREATE TABLE categoria (
    nome VARCHAR(255) NOT NULL UNIQUE,
    CONSTRAINT pk_categoria PRIMARY KEY(nome)
);

CREATE TABLE categoria_simples (
    nome VARCHAR(255) NOT NULL UNIQUE, 
    CONSTRAINT pk_categoria_simples PRIMARY KEY(nome),
    CONSTRAINT fk_categoria_simples_categoria FOREIGN KEY(nome) REFERENCES categoria(nome)
);

CREATE TABLE super_categoria (
    nome VARCHAR(255) NOT NULL UNIQUE, 
    CONSTRAINT pk_super_categoria PRIMARY KEY(nome),
    CONSTRAINT fk_super_categoria_categoria FOREIGN KEY(nome) REFERENCES categoria(nome)
);

CREATE TABLE tem_outra (
    super_categoria VARCHAR(255) NOT NULL,
    categoria VARCHAR(255) NOT NULL UNIQUE, 
    CONSTRAINT pk_tem_outra PRIMARY KEY(categoria),
    CONSTRAINT fk_tem_outra_super_categoria FOREIGN KEY(super_categoria) REFERENCES super_categoria(nome),
    CONSTRAINT fk_tem_outra_categoria FOREIGN KEY(categoria) REFERENCES categoria(nome)
);

CREATE TABLE produto (
    ean NUMERIC(13, 0) NOT NULL UNIQUE,
    descr VARCHAR(255) NOT NULL,
    cat VARCHAR(255) NOT NULL,
    CONSTRAINT pk_produto PRIMARY KEY(ean),
    CONSTRAINT fk_produto_categoria FOREIGN KEY(cat) REFERENCES categoria(nome)
);

CREATE TABLE tem_categoria (
    ean NUMERIC(13, 0) NOT NULL,
    nome VARCHAR(255) NOT NULL,
    CONSTRAINT pk_tem_categoria PRIMARY KEY(ean, nome),
    CONSTRAINT fk_tem_categoria_produto FOREIGN KEY(ean) REFERENCES produto(ean),
    CONSTRAINT fk_tem_categoria_categoria FOREIGN KEY(nome) REFERENCES categoria(nome)
);

CREATE TABLE IVM (
    num_serie VARCHAR(255) NOT NULL,
    fabricante VARCHAR(255) NOT NULL,
    CONSTRAINT pk_IVM PRIMARY KEY(num_serie, fabricante)
);

CREATE TABLE ponto_de_retalho (
    nome VARCHAR(255) NOT NULL UNIQUE,
    distrito VARCHAR(255) NOT NULL,
    concelho VARCHAR(255) NOT NULL,
    CONSTRAINT pk_ponto_de_retalho PRIMARY KEY(nome)
);

CREATE TABLE instalada_em (
    num_serie VARCHAR(255) NOT NULL,
    fabricante VARCHAR(255) NOT NULL,
    "local" VARCHAR(255) NOT NULL,
    CONSTRAINT pk_instalada_em PRIMARY KEY(num_serie, fabricante),
    CONSTRAINT fk_instalada_em_IVM FOREIGN KEY(num_serie, fabricante) REFERENCES IVM(num_serie, fabricante),
    CONSTRAINT fk_instalada_em_ponto_de_retalho FOREIGN KEY("local") REFERENCES ponto_de_retalho(nome)
);

CREATE TABLE prateleira (
    nro INTEGER NOT NULL,
    num_serie VARCHAR(255) NOT NULL,
    fabricante VARCHAR(255) NOT NULL,
    altura NUMERIC(4,2),
    nome VARCHAR(255) NOT NULL,
    CONSTRAINT pk_prateleira PRIMARY KEY(nro, num_serie, fabricante),
    CONSTRAINT fk_prateleira_IVM  FOREIGN KEY(num_serie, fabricante) REFERENCES IVM(num_serie, fabricante), 
    CONSTRAINT fk_prateleira_categoria FOREIGN KEY(nome) REFERENCES categoria(nome)
);

CREATE TABLE planograma (
    ean NUMERIC(13, 0) NOT NULL,
    nro INTEGER NOT NULL,
    num_serie VARCHAR(255) NOT NULL,
    fabricante VARCHAR(255) NOT NULL,
    faces INTEGER NOT NULL,
    unidades INTEGER NOT NULL,
    CONSTRAINT pk_planograma PRIMARY KEY(ean, nro, num_serie, fabricante),
    CONSTRAINT fk_planograma_produto FOREIGN KEY(ean) REFERENCES produto(ean),
    CONSTRAINT fk_planograma_prateleira FOREIGN KEY(nro, num_serie, fabricante) REFERENCES prateleira(nro, num_serie, fabricante) 
);

CREATE TABLE retalhista (
    tin NUMERIC(9,0) NOT NULL UNIQUE,
    "name" VARCHAR(255) NOT NULL UNIQUE,
    CONSTRAINT pk_retalhista PRIMARY KEY(tin)
);

CREATE TABLE responsavel_por (
    nome_cat VARCHAR(255) NOT NULL,
    tin NUMERIC(9,0) NOT NULL,
    num_serie VARCHAR(255) NOT NULL,
    fabricante VARCHAR(255) NOT NULL,
    CONSTRAINT pk_responsavel_por PRIMARY KEY(num_serie, fabricante), 
    CONSTRAINT fk_responsavel_por_IVM FOREIGN KEY(num_serie, fabricante) REFERENCES IVM(num_serie, fabricante), 
    CONSTRAINT fk_responsavel_por_retalhista FOREIGN KEY(tin) REFERENCES retalhista(tin),
    CONSTRAINT fk_responsavel_por_categoria FOREIGN KEY(nome_cat) REFERENCES categoria(nome)
);

CREATE TABLE evento_reposicao (
    ean NUMERIC(13,0) NOT NULL,
    nro INTEGER NOT NULL,
    num_serie VARCHAR(255) NOT NULL,
    fabricante VARCHAR(255) NOT NULL,
    instante TIMESTAMPTZ NOT NULL, 
    unidades INTEGER NOT NULL,
    tin INTEGER NOT NULL,
    CONSTRAINT pk_evento_reposicao PRIMARY KEY(ean, nro, num_serie, fabricante, instante),
    CONSTRAINT fk_evento_reposicao_planograma FOREIGN KEY(ean, nro, num_serie, fabricante) REFERENCES planograma(ean, nro, num_serie, fabricante), 
    CONSTRAINT fk_evento_reposicao_retalhista FOREIGN KEY(tin) REFERENCES retalhista(tin)
);

----------------------------------------
-- Populate Relations 
----------------------------------------

INSERT INTO categoria VALUES ('Cereais e derivados');
INSERT INTO categoria VALUES ('Padaria');
INSERT INTO categoria VALUES ('Paes');
INSERT INTO categoria VALUES ('Pastelaria');
INSERT INTO categoria VALUES ('Bolos');
INSERT INTO categoria VALUES ('Biscoitos');
INSERT INTO categoria VALUES ('Massas');
INSERT INTO categoria VALUES ('Massa Fresca');

INSERT INTO categoria VALUES ('Laticinios');
INSERT INTO categoria VALUES ('Iogurtes');
INSERT INTO categoria VALUES ('Leite e derivados');

INSERT INTO categoria VALUES ('Doces');
INSERT INTO categoria VALUES ('Chocolates');
INSERT INTO categoria VALUES ('Bolachas');

INSERT INTO categoria VALUES ('Salgados');
INSERT INTO categoria VALUES ('Batatas fritas');
INSERT INTO categoria VALUES ('Pipocas');

INSERT INTO categoria VALUES ('Bebidas');
INSERT INTO categoria VALUES ('Bebidas alcoolicas');
INSERT INTO categoria VALUES ('Bebidas gaseificadas');
INSERT INTO categoria VALUES ('Aguas');

INSERT INTO categoria VALUES ('Healthy');
INSERT INTO categoria VALUES ('Frutas');
INSERT INTO categoria VALUES ('Saladas');


INSERT INTO categoria_simples VALUES ('Paes');
INSERT INTO categoria_simples VALUES ('Bolos');
INSERT INTO categoria_simples VALUES ('Biscoitos');
INSERT INTO categoria_simples VALUES ('Massa Fresca');
INSERT INTO categoria_simples VALUES ('Iogurtes');
INSERT INTO categoria_simples VALUES ('Leite e derivados');
INSERT INTO categoria_simples VALUES ('Chocolates');
INSERT INTO categoria_simples VALUES ('Bolachas');
INSERT INTO categoria_simples VALUES ('Batatas fritas');
INSERT INTO categoria_simples VALUES ('Pipocas');
INSERT INTO categoria_simples VALUES ('Bebidas alcoolicas');
INSERT INTO categoria_simples VALUES ('Bebidas gaseificadas');
INSERT INTO categoria_simples VALUES ('Aguas');
INSERT INTO categoria_simples VALUES ('Frutas');
INSERT INTO categoria_simples VALUES ('Saladas');


INSERT INTO super_categoria VALUES ('Cereais e derivados');
INSERT INTO super_categoria VALUES ('Padaria');
INSERT INTO super_categoria VALUES ('Pastelaria');
INSERT INTO super_categoria VALUES ('Massas');
INSERT INTO super_categoria VALUES ('Laticinios');
INSERT INTO super_categoria VALUES ('Doces');
INSERT INTO super_categoria VALUES ('Salgados');
INSERT INTO super_categoria VALUES ('Bebidas');
INSERT INTO super_categoria VALUES ('Healthy');


INSERT INTO tem_outra VALUES ('Cereais e derivados', 'Padaria');
INSERT INTO tem_outra VALUES ('Padaria', 'Paes');
INSERT INTO tem_outra VALUES ('Cereais e derivados', 'Pastelaria');
INSERT INTO tem_outra VALUES ('Pastelaria', 'Bolos');
INSERT INTO tem_outra VALUES ('Pastelaria', 'Biscoitos');
INSERT INTO tem_outra VALUES ('Cereais e derivados', 'Massas');
INSERT INTO tem_outra VALUES ('Massas', 'Massa Fresca');
INSERT INTO tem_outra VALUES ('Laticinios', 'Iogurtes');
INSERT INTO tem_outra VALUES ('Laticinios', 'Leite e derivados');
INSERT INTO tem_outra VALUES ('Doces', 'Chocolates');
INSERT INTO tem_outra VALUES ('Doces', 'Bolachas');
INSERT INTO tem_outra VALUES ('Salgados', 'Batatas fritas');
INSERT INTO tem_outra VALUES ('Salgados', 'Pipocas');
INSERT INTO tem_outra VALUES ('Bebidas', 'Bebidas alcoolicas');
INSERT INTO tem_outra VALUES ('Bebidas', 'Bebidas gaseificadas');
INSERT INTO tem_outra VALUES ('Bebidas', 'Aguas');
INSERT INTO tem_outra VALUES ('Healthy', 'Frutas');
INSERT INTO tem_outra VALUES ('Healthy', 'Saladas');


INSERT INTO produto VALUES (3199200313267, 'Ruffles', 'Batatas fritas');
INSERT INTO produto VALUES (7670394888534, 'Lays', 'Batatas fritas');
INSERT INTO produto VALUES (6238855578694, 'Pringles', 'Batatas fritas');
INSERT INTO produto VALUES (6861509494523, 'Doritos', 'Batatas fritas');
INSERT INTO produto VALUES (7375198808954, 'Snickers', 'Chocolates');
INSERT INTO produto VALUES (2731104729457, 'Kit Kat', 'Chocolates');
INSERT INTO produto VALUES (9657926139492, 'Prozis Protein Bar', 'Chocolates');
INSERT INTO produto VALUES (2563144910287, 'Milka', 'Chocolates');
INSERT INTO produto VALUES (5876633112961, 'Twix', 'Chocolates');
INSERT INTO produto VALUES (4753397644505, 'Toblerone', 'Chocolates');
INSERT INTO produto VALUES (1052477753445, 'Bollycao', 'Chocolates');
INSERT INTO produto VALUES (6290972701607, 'Oreo', 'Bolachas');
INSERT INTO produto VALUES (0482053928010, 'Chips Ahoy', 'Bolachas');
INSERT INTO produto VALUES (1144945992903, 'Croissant de Chocolate', 'Paes');
INSERT INTO produto VALUES (0118698103313, 'Sandes de Fiambre', 'Paes');
INSERT INTO produto VALUES (8833736827663, 'Sandes de Queijo', 'Paes');
INSERT INTO produto VALUES (0011600618786, 'Noddles', 'Massas');
INSERT INTO produto VALUES (9087008726799, 'Ucal', 'Leite e derivados');
INSERT INTO produto VALUES (7991099017273, 'Agua Luso', 'Aguas');
INSERT INTO produto VALUES (5356415237667, 'Agua Evian', 'Aguas');
INSERT INTO produto VALUES (6006915712520, 'Agua Monchique', 'Aguas');
INSERT INTO produto VALUES (6069917736338, 'Agua das Pedras Limao', 'Bebidas gaseificadas');
INSERT INTO produto VALUES (0780056307291, 'Agua das Pedras', 'Bebidas gaseificadas');
INSERT INTO produto VALUES (8449804254851, 'Coca Cola', 'Bebidas gaseificadas');
INSERT INTO produto VALUES (5442276737313, 'Pepsi', 'Bebidas gaseificadas');
INSERT INTO produto VALUES (9029763073923, 'Fanta Laranja', 'Bebidas gaseificadas');
INSERT INTO produto VALUES (5676735856677, 'Fanta Maracuja', 'Bebidas gaseificadas');
INSERT INTO produto VALUES (0859559358984, 'Sagres', 'Bebidas alcoolicas');
INSERT INTO produto VALUES (3253675671496, 'Super Bock', 'Bebidas alcoolicas');
INSERT INTO produto VALUES (3036287662881, 'Laranja', 'Frutas');
INSERT INTO produto VALUES (3103177090083, 'Banana', 'Frutas');


INSERT INTO tem_categoria VALUES (3199200313267, 'Salgados');
INSERT INTO tem_categoria VALUES (3199200313267, 'Batatas fritas');
INSERT INTO tem_categoria VALUES (7670394888534, 'Salgados');
INSERT INTO tem_categoria VALUES (7670394888534, 'Batatas fritas');
INSERT INTO tem_categoria VALUES (6238855578694, 'Salgados');
INSERT INTO tem_categoria VALUES (6238855578694, 'Batatas fritas');
INSERT INTO tem_categoria VALUES (6861509494523, 'Salgados');
INSERT INTO tem_categoria VALUES (6861509494523, 'Batatas fritas');
INSERT INTO tem_categoria VALUES (7375198808954, 'Doces');
INSERT INTO tem_categoria VALUES (7375198808954, 'Chocolates');
INSERT INTO tem_categoria VALUES (2731104729457, 'Doces');
INSERT INTO tem_categoria VALUES (2731104729457, 'Chocolates');
INSERT INTO tem_categoria VALUES (9657926139492, 'Doces');
INSERT INTO tem_categoria VALUES (9657926139492, 'Chocolates');
INSERT INTO tem_categoria VALUES (2563144910287, 'Doces');
INSERT INTO tem_categoria VALUES (2563144910287, 'Chocolates');
INSERT INTO tem_categoria VALUES (5876633112961, 'Doces');
INSERT INTO tem_categoria VALUES (5876633112961, 'Chocolates');
INSERT INTO tem_categoria VALUES (4753397644505, 'Doces');
INSERT INTO tem_categoria VALUES (4753397644505, 'Chocolates');
INSERT INTO tem_categoria VALUES (1052477753445, 'Doces');
INSERT INTO tem_categoria VALUES (1052477753445, 'Chocolates');
INSERT INTO tem_categoria VALUES (6290972701607, 'Doces');
INSERT INTO tem_categoria VALUES (6290972701607, 'Bolachas');
INSERT INTO tem_categoria VALUES (0482053928010, 'Doces');
INSERT INTO tem_categoria VALUES (0482053928010, 'Bolachas');
INSERT INTO tem_categoria VALUES (1144945992903, 'Cereais e derivados');
INSERT INTO tem_categoria VALUES (1144945992903, 'Padaria');
INSERT INTO tem_categoria VALUES (1144945992903, 'Paes');
INSERT INTO tem_categoria VALUES (0118698103313, 'Cereais e derivados');
INSERT INTO tem_categoria VALUES (0118698103313, 'Padaria');
INSERT INTO tem_categoria VALUES (0118698103313, 'Paes');
INSERT INTO tem_categoria VALUES (8833736827663, 'Cereais e derivados');
INSERT INTO tem_categoria VALUES (8833736827663, 'Padaria');
INSERT INTO tem_categoria VALUES (8833736827663, 'Paes');
INSERT INTO tem_categoria VALUES (0011600618786, 'Cereais e derivados');
INSERT INTO tem_categoria VALUES (0011600618786, 'Massas');
INSERT INTO tem_categoria VALUES (9087008726799, 'Laticinios');
INSERT INTO tem_categoria VALUES (9087008726799, 'Leite e derivados');
INSERT INTO tem_categoria VALUES (9087008726799, 'Bebidas');
INSERT INTO tem_categoria VALUES (7991099017273, 'Bebidas');
INSERT INTO tem_categoria VALUES (7991099017273, 'Aguas');
INSERT INTO tem_categoria VALUES (7991099017273, 'Healthy');
INSERT INTO tem_categoria VALUES (5356415237667, 'Bebidas');
INSERT INTO tem_categoria VALUES (5356415237667, 'Aguas');
INSERT INTO tem_categoria VALUES (5356415237667, 'Healthy');
INSERT INTO tem_categoria VALUES (6006915712520, 'Bebidas');
INSERT INTO tem_categoria VALUES (6006915712520, 'Aguas');
INSERT INTO tem_categoria VALUES (6006915712520, 'Healthy');
INSERT INTO tem_categoria VALUES (6069917736338, 'Bebidas');
INSERT INTO tem_categoria VALUES (6069917736338, 'Bebidas gaseificadas');
INSERT INTO tem_categoria VALUES (0780056307291, 'Bebidas');
INSERT INTO tem_categoria VALUES (0780056307291, 'Bebidas gaseificadas');
INSERT INTO tem_categoria VALUES (8449804254851, 'Bebidas');
INSERT INTO tem_categoria VALUES (8449804254851, 'Bebidas gaseificadas');
INSERT INTO tem_categoria VALUES (5442276737313, 'Bebidas');
INSERT INTO tem_categoria VALUES (5442276737313, 'Bebidas gaseificadas');
INSERT INTO tem_categoria VALUES (9029763073923, 'Bebidas');
INSERT INTO tem_categoria VALUES (9029763073923, 'Bebidas gaseificadas');
INSERT INTO tem_categoria VALUES (5676735856677, 'Bebidas');
INSERT INTO tem_categoria VALUES (5676735856677, 'Bebidas gaseificadas');
INSERT INTO tem_categoria VALUES (0859559358984, 'Bebidas');
INSERT INTO tem_categoria VALUES (0859559358984, 'Bebidas alcoolicas');
INSERT INTO tem_categoria VALUES (3253675671496, 'Bebidas');
INSERT INTO tem_categoria VALUES (3253675671496, 'Bebidas alcoolicas');
INSERT INTO tem_categoria VALUES (3036287662881, 'Healthy');
INSERT INTO tem_categoria VALUES (3036287662881, 'Frutas');
INSERT INTO tem_categoria VALUES (3103177090083, 'Healthy');
INSERT INTO tem_categoria VALUES (3103177090083, 'Frutas');


INSERT INTO IVM VALUES ('JG8MF9JQEE', 'Tecnovending');
INSERT INTO IVM VALUES ('MEC8LJZEVZ', 'Tecnovending');
INSERT INTO IVM VALUES ('BNDGYGOKWU', 'Tecnovending');
INSERT INTO IVM VALUES ('5VEU2CZB24', 'Magex');
INSERT INTO IVM VALUES ('W46HVW3A6R', 'Benson');
INSERT INTO IVM VALUES ('1ES62UGNNM', 'VendTrade');
INSERT INTO IVM VALUES ('PFAF099KMO', 'VendTrade');
INSERT INTO IVM VALUES ('AP61Y6IS7Y', 'VendTrade');
INSERT INTO IVM VALUES ('I0A5UIJXHK', 'Westomatic');
INSERT INTO IVM VALUES ('U1GPG7P54O', 'Westomatic');


INSERT INTO ponto_de_retalho VALUES ('IST - Lisboa', 'Lisboa', 'Lisboa');
INSERT INTO ponto_de_retalho VALUES ('Galp - Oeiras', 'Lisboa', 'Oeiras');
INSERT INTO ponto_de_retalho VALUES ('Continente - Amadora', 'Lisboa', 'Amadora');
INSERT INTO ponto_de_retalho VALUES ('Continente - Lisboa', 'Lisboa', 'Lisboa');
INSERT INTO ponto_de_retalho VALUES ('Continente - Loures', 'Lisboa', 'Loures');
INSERT INTO ponto_de_retalho VALUES ('Continente - Torres Vedras', 'Lisboa', 'Torres Vedras');
INSERT INTO ponto_de_retalho VALUES ('Continente - Vila Nova de Gaia', 'Porto', 'Vila Nova de Gaia');
INSERT INTO ponto_de_retalho VALUES ('Repsol - Sintra', 'Lisboa', 'Sintra');
INSERT INTO ponto_de_retalho VALUES ('Repsol - Anadia', 'Aveiro', 'Anadia');
INSERT INTO ponto_de_retalho VALUES ('Galp - Tondela', 'Viseu', 'Tondela');
INSERT INTO ponto_de_retalho VALUES ('Galp - Maia', 'Porto', 'Maia');


INSERT INTO instalada_em VALUES ('JG8MF9JQEE', 'Tecnovending', 'IST - Lisboa');
INSERT INTO instalada_em VALUES ('MEC8LJZEVZ', 'Tecnovending', 'IST - Lisboa');
INSERT INTO instalada_em VALUES ('BNDGYGOKWU', 'Tecnovending', 'Continente - Amadora' );
INSERT INTO instalada_em VALUES ('5VEU2CZB24', 'Magex', 'Galp - Tondela');
INSERT INTO instalada_em VALUES ('W46HVW3A6R', 'Benson', 'Continente - Vila Nova de Gaia');
INSERT INTO instalada_em VALUES ('1ES62UGNNM', 'VendTrade', 'Repsol - Anadia');
INSERT INTO instalada_em VALUES ('PFAF099KMO', 'VendTrade', 'Repsol - Sintra');
INSERT INTO instalada_em VALUES ('AP61Y6IS7Y', 'VendTrade', 'Continente - Loures');
INSERT INTO instalada_em VALUES ('I0A5UIJXHK', 'Westomatic', 'Galp - Oeiras');
INSERT INTO instalada_em VALUES ('U1GPG7P54O', 'Westomatic', 'Galp - Oeiras');


INSERT INTO prateleira VALUES (1, 'JG8MF9JQEE', 'Tecnovending', 10.50, 'Batatas fritas');
INSERT INTO prateleira VALUES (2, 'JG8MF9JQEE', 'Tecnovending', 10.50, 'Chocolates');
INSERT INTO prateleira VALUES (3, 'JG8MF9JQEE', 'Tecnovending', 10.50, 'Bebidas');
INSERT INTO prateleira VALUES (4, 'JG8MF9JQEE', 'Tecnovending', 10.50, 'Bebidas');
INSERT INTO prateleira VALUES (5, 'JG8MF9JQEE', 'Tecnovending', 10.50, 'Chocolates');
INSERT INTO prateleira VALUES (6, 'JG8MF9JQEE', 'Tecnovending', 10.50, 'Frutas');
INSERT INTO prateleira VALUES (1, 'MEC8LJZEVZ', 'Tecnovending', 10.50, 'Batatas fritas');
INSERT INTO prateleira VALUES (2, 'MEC8LJZEVZ', 'Tecnovending', 10.50, 'Chocolates');
INSERT INTO prateleira VALUES (3, 'MEC8LJZEVZ', 'Tecnovending', 10.50, 'Bebidas');
INSERT INTO prateleira VALUES (4, 'MEC8LJZEVZ', 'Tecnovending', 10.50, 'Bebidas');
INSERT INTO prateleira VALUES (5, 'MEC8LJZEVZ', 'Tecnovending', 10.50, 'Chocolates');
INSERT INTO prateleira VALUES (6, 'MEC8LJZEVZ', 'Tecnovending', 10.50, 'Frutas');
INSERT INTO prateleira VALUES (1, '5VEU2CZB24', 'Magex', 10.50, 'Batatas fritas');
INSERT INTO prateleira VALUES (2, '5VEU2CZB24', 'Magex', 10.50, 'Chocolates');
INSERT INTO prateleira VALUES (3, '5VEU2CZB24', 'Magex', 10.50, 'Bebidas');
INSERT INTO prateleira VALUES (1, '1ES62UGNNM', 'VendTrade', 10.50, 'Batatas fritas');
INSERT INTO prateleira VALUES (2, '1ES62UGNNM', 'VendTrade', 10.50, 'Chocolates');
INSERT INTO prateleira VALUES (3, '1ES62UGNNM', 'VendTrade', 10.50, 'Bebidas');
INSERT INTO prateleira VALUES (1, 'I0A5UIJXHK', 'Westomatic', 10.50, 'Batatas fritas');
INSERT INTO prateleira VALUES (2, 'I0A5UIJXHK', 'Westomatic', 10.50, 'Chocolates');
INSERT INTO prateleira VALUES (3, 'I0A5UIJXHK', 'Westomatic', 10.50, 'Bebidas');


INSERT INTO planograma VALUES (3199200313267, 1, 'JG8MF9JQEE', 'Tecnovending', 2, 10);
INSERT INTO planograma VALUES (7670394888534, 1, 'JG8MF9JQEE', 'Tecnovending', 3, 20);
INSERT INTO planograma VALUES (6238855578694, 1, 'JG8MF9JQEE', 'Tecnovending', 2, 10);

INSERT INTO planograma VALUES (3199200313267, 1, 'MEC8LJZEVZ', 'Tecnovending', 2, 10);
INSERT INTO planograma VALUES (6238855578694, 2, 'MEC8LJZEVZ', 'Tecnovending', 3, 20);
INSERT INTO planograma VALUES (1052477753445, 2, 'MEC8LJZEVZ', 'Tecnovending', 2, 10);

INSERT INTO planograma VALUES (3199200313267, 1, '5VEU2CZB24', 'Magex', 2, 10);
INSERT INTO planograma VALUES (1052477753445, 2, '5VEU2CZB24', 'Magex', 3, 20);
INSERT INTO planograma VALUES (5442276737313, 3, '5VEU2CZB24', 'Magex', 2, 10);
INSERT INTO planograma VALUES (8449804254851, 3, '5VEU2CZB24', 'Magex', 2, 10);

INSERT INTO planograma VALUES (3199200313267, 1, 'I0A5UIJXHK', 'Westomatic', 2, 10);
INSERT INTO planograma VALUES (6238855578694, 2, 'I0A5UIJXHK', 'Westomatic', 3, 20);
INSERT INTO planograma VALUES (0859559358984, 3, 'I0A5UIJXHK', 'Westomatic', 2, 10);


INSERT INTO retalhista VALUES (281603324, 'Afonso Valadim Fonseca');
INSERT INTO retalhista VALUES (232843090, 'Josefina Soverosa Cavadas');
INSERT INTO retalhista VALUES (295581387, 'Salvador Xavier Velasques');
INSERT INTO retalhista VALUES (289439442, 'Ming Caeira Rocha');
INSERT INTO retalhista VALUES (260389170, 'Francesca Canadas Redondo');
INSERT INTO retalhista VALUES (124639429, 'Taynara Feira Negromonte');
INSERT INTO retalhista VALUES (240217268, 'Rania Boaventura Ramalho');
INSERT INTO retalhista VALUES (310961319, 'Ndeye Borges Vargas');
INSERT INTO retalhista VALUES (146551656, 'Diniz Mourinho Botica');
INSERT INTO retalhista VALUES (299705080, 'Jadir Nobre Afonso');
INSERT INTO retalhista VALUES (153036788, 'Liliane Granjeiro Marques');


INSERT INTO responsavel_por VALUES('Aguas', 281603324, 'JG8MF9JQEE', 'Tecnovending');
INSERT INTO responsavel_por VALUES('Chocolates', 281603324, 'MEC8LJZEVZ', 'Tecnovending');
INSERT INTO responsavel_por VALUES('Batatas fritas', 295581387, 'BNDGYGOKWU', 'Tecnovending');
INSERT INTO responsavel_por VALUES('Salgados', 289439442, '1ES62UGNNM', 'VendTrade');
INSERT INTO responsavel_por VALUES('Salgados', 289439442, 'PFAF099KMO', 'VendTrade');
INSERT INTO responsavel_por VALUES('Salgados', 232843090, 'I0A5UIJXHK', 'Westomatic');


INSERT INTO evento_reposicao VALUES (3199200313267, 1, 'JG8MF9JQEE', 'Tecnovending', '2019-6-19 10:23:54 Europe/Lisbon', 5, 281603324);
INSERT INTO evento_reposicao VALUES (3199200313267, 1, 'MEC8LJZEVZ', 'Tecnovending', '2021-4-20 10:43:54 Europe/Lisbon', 3, 232843090);
INSERT INTO evento_reposicao VALUES (5442276737313, 3, '5VEU2CZB24', 'Magex', '2021-10-29 11:20:54 Europe/Lisbon', 5, 295581387);
INSERT INTO evento_reposicao VALUES (3199200313267, 1, 'JG8MF9JQEE', 'Tecnovending', '2019-6-20 13:23:54 Europe/Lisbon', 2, 310961319);
INSERT INTO evento_reposicao VALUES (6238855578694, 2, 'I0A5UIJXHK', 'Westomatic', '2022-2-2 19:20:54 Europe/Lisbon', 5, 281603324);
INSERT INTO evento_reposicao VALUES (1052477753445, 2, 'MEC8LJZEVZ', 'Tecnovending', '2022-4-7 21:20:54 Europe/Lisbon', 4, 299705080);
INSERT INTO evento_reposicao VALUES (8449804254851, 3, '5VEU2CZB24', 'Magex', '2022-5-1 11:20:54 Europe/Lisbon', 2, 124639429);
INSERT INTO evento_reposicao VALUES (8449804254851, 3, '5VEU2CZB24', 'Magex', '2022-5-1 11:23:54 Europe/Lisbon', 2, 124639429);
INSERT INTO evento_reposicao VALUES (6238855578694, 2, 'MEC8LJZEVZ', 'Tecnovending', '2022-5-3 14:23:54 Europe/Lisbon', 2, 124639429);


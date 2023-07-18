DROP VIEW IF EXISTS Vendas;

CREATE VIEW Vendas(ean, cat, ano, trimestre, mes, dia_mes, dia_semana, distrito, concelho, unidades)
AS
SELECT evento_reposicao.ean,
    produto.cat,
    EXTRACT(YEAR FROM instante) AS ano,
    EXTRACT(QUARTER FROM instante) AS trimestre,
    EXTRACT(MONTH FROM instante) AS mes,
    EXTRACT(DAY FROM instante) AS dia_mes,
    EXTRACT(DOW FROM instante) AS dia_semana,
    ponto_de_retalho.distrito,
    ponto_de_retalho.concelho,
    evento_reposicao.unidades
FROM evento_reposicao
INNER JOIN produto ON evento_reposicao.ean = produto.ean
INNER JOIN instalada_em ON evento_reposicao.num_serie = instalada_em.num_serie AND evento_reposicao.fabricante = instalada_em.fabricante
INNER JOIN ponto_de_retalho ON instalada_em.local = ponto_de_retalho.nome;


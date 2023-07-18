-- 1. num dado periodo (i.e. entre duas datas), por dia da semana, por concelho e no total

SELECT dia_semana, concelho, SUM(unidades) AS total_artigos_vendidos
FROM Vendas
WHERE MAKE_DATE(ano::INTEGER, mes::INTEGER, dia_mes::INTEGER) BETWEEN '2015-01-01' AND '2022-01-01'
GROUP BY 
    GROUPING SETS ( (dia_semana), (concelho), () );


-- 2. num dado distrito (i.e. “Lisboa”), por concelho, categoria, dia da semana e no total

Vendas(ean, cat, ano, trimestre, mes, dia_mes, dia_semana, distrito, concelho, unidades)

SELECT concelho, cat, dia_semana, SUM(unidades) AS total_artigos_vendidos
FROM Vendas
WHERE distrito = 'Lisboa'
GROUP BY
    GROUPING SETS ( (concelho, cat, dia_semana), () );


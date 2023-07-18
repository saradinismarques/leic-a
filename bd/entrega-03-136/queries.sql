-- Qual o nome do retalhista (ou retalhistas) responsaveis pela reposicao do maior numero de categorias?

SELECT "name"
FROM retalhista
    NATURAL JOIN (
        SELECT tin, COUNT(*) 
        FROM responsavel_por
        GROUP BY tin
        HAVING COUNT(*) >= ALL (
            SELECT COUNT(*)
            FROM responsavel_por
            GROUP BY tin
        )
    ) AS retalhista_mais_responsavel;


-- Qual o nome do ou dos retalhistas que sao responsaveis por todas as categorias simples?

SELECT DISTINCT "name"
FROM retalhista
INNER JOIN responsavel_por ON retalhista.tin = responsavel_por.tin
INNER JOIN categoria_simples ON categoria_simples.nome = responsavel_por.nome_cat;


-- Quais os produtos (ean) que nunca foram repostos?

SELECT ean
FROM produto
WHERE ean NOT IN (
    SELECT ean 
    FROM evento_reposicao);


-- Quais os produtos (ean) que foram repostos sempre pelo mesmo retalhista?

SELECT ean
FROM evento_reposicao
GROUP BY ean
HAVING COUNT(DISTINCT tin) = 1;


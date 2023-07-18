DROP TRIGGER IF EXISTS chk_categoria_ciclo_trigger ON tem_outra;
DROP TRIGGER IF EXISTS chk_unidades_evento_reposicao_trigger ON evento_reposicao;
DROP TRIGGER IF EXISTS chk_produto_prateleira_categoria_trigger ON planograma;


--(RI-1) Uma Categoria nao pode estar contida em si propria
CREATE OR REPLACE FUNCTION chk_categoria_ciclo_proc() RETURNS TRIGGER AS
$$
BEGIN
    IF NEW.super_categoria = NEW.categoria THEN
        RAISE EXCEPTION 'Uma Categoria nao pode estar contida em si propria';
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER chk_categoria_ciclo_trigger
BEFORE UPDATE OR INSERT ON tem_outra
FOR EACH ROW EXECUTE PROCEDURE chk_categoria_ciclo_proc();


--(RI-4) O numero de unidades repostas num Evento de Reposicao não pode exceder o numero de 
--unidades especificado no Planograma
CREATE OR REPLACE FUNCTION chk_unidades_evento_reposicao_proc() RETURNS TRIGGER AS
$$
DECLARE unidades_planograma INTEGER;
BEGIN
    SELECT planograma.unidades INTO unidades_planograma
    FROM planograma
    WHERE planograma.ean = NEW.ean AND planograma.nro = NEW.nro AND planograma.num_serie = NEW.num_serie 
          AND planograma.fabricante = NEW.fabricante;

    IF NEW.unidades > unidades_planograma THEN
        RAISE EXCEPTION 'O numero de unidades repostas num Evento de Reposicao nao pode exceder o numero de unidades especificado no Planograma';
    END IF;
    
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER chk_unidades_evento_reposicao_trigger
BEFORE UPDATE OR INSERT ON evento_reposicao
FOR EACH ROW EXECUTE PROCEDURE chk_unidades_evento_reposicao_proc();


--(RI-5) Um Produto so pode ser reposto numa Prateleira que apresente (pelo menos) uma das 
--Categorias desse produto
CREATE OR REPLACE FUNCTION chk_produto_prateleira_categoria_proc() RETURNS TRIGGER AS
$$
DECLARE count_categorias INTEGER;
BEGIN
    SELECT COUNT(*) INTO count_categorias
    FROM prateleira P
    WHERE P.nro = NEW.nro AND P.num_serie = NEW.num_serie AND P.fabricante = NEW.fabricante AND P.nome IN (
        SELECT nome
        FROM tem_categoria T
        WHERE T.ean = NEW.ean
    );

    IF count_categorias = 0 THEN
        RAISE EXCEPTION 'Um Produto so pode ser reposto numa Prateleira que apresente (pelo menos) uma das Categorias desse produto';
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER chk_produto_prateleira_categoria_trigger
BEFORE UPDATE OR INSERT ON planograma
FOR EACH ROW EXECUTE PROCEDURE chk_produto_prateleira_categoria_proc();


#!/usr/bin/python3
from wsgiref.handlers import CGIHandler
from flask import Flask
from flask import render_template, request, redirect, url_for
import psycopg2
import psycopg2.extras

## SGBD configs
DB_HOST = "db.tecnico.ulisboa.pt"
DB_USER = "ist192424"
DB_DATABASE = DB_USER
DB_PASSWORD = "12345"
DB_CONNECTION_STRING = "host=%s dbname=%s user=%s password=%s" % (DB_HOST, DB_DATABASE, DB_USER, DB_PASSWORD)

app = Flask(__name__)

@app.route("/")
def display_menu():
    try:
        return render_template("index.html")
    except Exception as e:
        return str(e)  # Renders a page with the error.

@app.route("/categorias")
def listar_editar_categorias():
    dbConn = None
    cursor = None
    try:
        dbConn = psycopg2.connect(DB_CONNECTION_STRING)
        cursor = dbConn.cursor(cursor_factory=psycopg2.extras.DictCursor)
        query = "SELECT categoria.nome, tem_outra.super_categoria FROM categoria LEFT JOIN tem_outra ON categoria.nome = tem_outra.categoria"
        cursor.execute(query)
        return render_template("categorias.html", cursor=cursor)
    except Exception as e:
        return str(e)
    finally:
        cursor.close()
        dbConn.close()

@app.route("/categorias", methods=["POST"])
def remove_categoria():
    dbConn = None
    cursor = None
    try:
        dbConn = psycopg2.connect(DB_CONNECTION_STRING)
        cursor = dbConn.cursor(cursor_factory=psycopg2.extras.DictCursor)
        nome = request.form["nome"]
        query1 = "DELETE FROM responsavel_por WHERE nome_cat = %s"
        query2 = "DELETE FROM evento_reposicao WHERE EXISTS(SELECT * FROM planograma INNER JOIN prateleira ON planograma.nro = prateleira.nro AND planograma.num_serie = prateleira.num_serie AND planograma.fabricante = prateleira.fabricante WHERE evento_reposicao.ean = planograma.ean AND evento_reposicao.nro = planograma.nro AND evento_reposicao.num_serie = planograma.num_serie AND evento_reposicao.fabricante = planograma.fabricante AND prateleira.nome = %s)"
        query3 = "DELETE FROM planograma WHERE EXISTS(SELECT * FROM prateleira WHERE planograma.nro = prateleira.nro AND planograma.num_serie = prateleira.num_serie AND planograma.fabricante = prateleira.fabricante AND prateleira.nome = %s)"
        query4 = "DELETE FROM prateleira WHERE nome = %s"
        query5 = "DELETE FROM tem_categoria WHERE EXISTS(SELECT * FROM produto WHERE tem_categoria.ean = produto.ean AND produto.cat = %s)"
        query6 = "DELETE FROM tem_categoria WHERE nome = %s"
        query7 = "DELETE FROM evento_reposicao WHERE EXISTS(SELECT * FROM planograma INNER JOIN produto ON planograma.ean = produto.ean WHERE evento_reposicao.ean = planograma.ean AND evento_reposicao.nro = planograma.nro AND evento_reposicao.num_serie = planograma.num_serie AND evento_reposicao.fabricante = planograma.fabricante AND produto.cat = %s)"
        query8 = "DELETE FROM planograma WHERE EXISTS(SELECT * FROM produto WHERE planograma.ean = produto.ean AND produto.cat = %s)"
        query9 = "DELETE FROM produto WHERE cat = %s"
        query10 = "DELETE FROM tem_outra WHERE categoria = %s"
        query11 = "DELETE FROM tem_outra WHERE EXISTS(SELECT * FROM super_categoria WHERE tem_outra.super_categoria = super_categoria.nome AND super_categoria.nome = %s);"
        query12 = "DELETE FROM super_categoria WHERE nome = %s"
        query13 = "DELETE FROM categoria_simples WHERE nome = %s"
        query14 = "DELETE FROM categoria WHERE nome = %s"
        data = (nome,)
        cursor.execute(query1, data)
        cursor.execute(query2, data)
        cursor.execute(query3, data)
        cursor.execute(query4, data)
        cursor.execute(query5, data)
        cursor.execute(query6, data)
        cursor.execute(query7, data)
        cursor.execute(query8, data)
        cursor.execute(query9, data)
        cursor.execute(query10, data)
        cursor.execute(query11, data)
        cursor.execute(query12, data)
        cursor.execute(query13, data)
        cursor.execute(query14, data)
        return redirect(url_for("listar_editar_categorias"))
    except Exception as e:  
        return str(e)
    finally:
        dbConn.commit() 
        cursor.close()
        dbConn.close()

@app.route("/categorias/inserir")
def inserir_categoria():
    dbConn = None
    cursor = None
    try:
        dbConn = psycopg2.connect(DB_CONNECTION_STRING)
        cursor = dbConn.cursor(cursor_factory=psycopg2.extras.DictCursor)
        query = "SELECT * FROM super_categoria"
        cursor.execute(query)
        return render_template("inserir_categorias.html", cursor=cursor)
    except Exception as e:
        return str(e)
    finally:
        cursor.close()
        dbConn.close()

@app.route("/categorias/inserir", methods=["POST"])
def insere_categoria():
    dbConn = None
    cursor = None
    try:
        dbConn = psycopg2.connect(DB_CONNECTION_STRING)
        cursor = dbConn.cursor(cursor_factory=psycopg2.extras.DictCursor)
        nome = request.form["nome"]
        super_categoria = request.form["super_categoria"]
        query1 = "INSERT INTO categoria VALUES (%s)"
        query2 = "INSERT INTO categoria_simples VALUES (%s)"
        data = (nome,)
        cursor.execute(query1, data)
        cursor.execute(query2, data)
        if(super_categoria != ""):
            query3 = "INSERT INTO tem_outra VALUES (%s, %s)"
            data = (super_categoria, nome)
            cursor.execute(query3, data)
        return redirect(url_for("listar_editar_categorias"))
    except Exception as e:
        return str(e)
    finally:
        dbConn.commit()
        cursor.close()
        dbConn.close()
        
@app.route("/retalhistas")
def listar_editar_retalhistas():
    dbConn = None
    cursor = None
    try:
        dbConn = psycopg2.connect(DB_CONNECTION_STRING)
        cursor = dbConn.cursor(cursor_factory=psycopg2.extras.DictCursor)
        query = "SELECT * FROM retalhista"
        cursor.execute(query)
        return render_template("retalhistas.html", cursor=cursor)
    except Exception as e:
        return str(e)
    finally:
        cursor.close()
        dbConn.close()

@app.route("/retalhistas", methods=["POST"])
def remove_retalhista():
    dbConn = None
    cursor = None
    try:
        dbConn = psycopg2.connect(DB_CONNECTION_STRING)
        cursor = dbConn.cursor(cursor_factory=psycopg2.extras.DictCursor)
        tin = request.form["tin"]
        query1 = "DELETE FROM evento_reposicao WHERE tin = %s"
        query2 = "DELETE FROM responsavel_por WHERE tin = %s"
        query3 = "DELETE FROM retalhista WHERE tin = %s"
        data = (tin,)
        cursor.execute(query1, data)
        cursor.execute(query2, data)
        cursor.execute(query3, data)
        return redirect(url_for("listar_editar_retalhistas"))
    except Exception as e:
        return str(e)
    finally:
        dbConn.commit()
        cursor.close()
        dbConn.close()

@app.route("/retalhistas/inserir")
def inserir_retalhista():
    try:
        return render_template("inserir_retalhistas.html")
    except Exception as e:
        return str(e)

@app.route("/retalhistas/inserir", methods=["POST"])
def insere_retalhista():
    dbConn = None
    cursor = None
    try:
        dbConn = psycopg2.connect(DB_CONNECTION_STRING)
        cursor = dbConn.cursor(cursor_factory=psycopg2.extras.DictCursor)
        tin = request.form["tin"]
        name = request.form["name"]
        query = "INSERT INTO retalhista VALUES (%s, %s)"
        data = (tin, name)
        cursor.execute(query, data)
        return redirect(url_for("listar_editar_retalhistas"))
    except Exception as e:
        return str(e)
    finally:
        dbConn.commit() 
        cursor.close()
        dbConn.close()

@app.route("/responsabilidades")
def listar_editar_responsabilidades():
    dbConn = None
    cursor = None
    try:
        dbConn = psycopg2.connect(DB_CONNECTION_STRING)
        cursor = dbConn.cursor(cursor_factory=psycopg2.extras.DictCursor)
        tin = request.args["tin"]
        query = "SELECT * FROM responsavel_por WHERE tin = %s"
        data = (tin,)
        cursor.execute(query, data)
        return render_template("responsabilidades.html", cursor=cursor, params=request.args)
    except Exception as e:
        return str(e)
    finally:
        cursor.close()
        dbConn.close()

@app.route("/responsabilidades", methods=["POST"])
def remove_responsabilidade():
    dbConn = None
    cursor = None
    try:
        dbConn = psycopg2.connect(DB_CONNECTION_STRING)
        cursor = dbConn.cursor(cursor_factory=psycopg2.extras.DictCursor)
        num_serie = request.form["num_serie"]
        fabricante = request.form["fabricante"]
        query = "DELETE FROM responsavel_por WHERE num_serie = %s AND fabricante = %s"
        data = (num_serie, fabricante)
        cursor.execute(query, data)
        return redirect(url_for("listar_editar_responsabilidades", tin=request.args.get("tin")))
    except Exception as e:
        return str(e)
    finally:
        dbConn.commit()
        cursor.close()
        dbConn.close()

@app.route("/responsabilidades/inserir")
def inserir_responsabilidade():
    dbConn = None
    cursor_IVM = None
    cursor_categoria = None
    try:
        dbConn = psycopg2.connect(DB_CONNECTION_STRING)
        cursor_IVM = dbConn.cursor(cursor_factory=psycopg2.extras.DictCursor)
        cursor_categoria = dbConn.cursor(cursor_factory=psycopg2.extras.DictCursor)
        query_IVM = "SELECT * FROM IVM WHERE (num_serie, fabricante) NOT IN (SELECT num_serie, fabricante FROM responsavel_por)"
        query_Categoria = "SELECT * FROM Categoria"
        cursor_IVM.execute(query_IVM)
        cursor_categoria.execute(query_Categoria)
        return render_template("inserir_responsabilidades.html", cursor_IVM=cursor_IVM, cursor_categoria=cursor_categoria, params=request.args)
    except Exception as e:
        return str(e)
    finally:
        cursor_IVM.close()
        cursor_categoria.close()
        dbConn.close()

@app.route("/responsabilidades/inserir", methods=["POST"])
def insere_responsabilidade():
    dbConn = None
    cursor = None
    try:
        dbConn = psycopg2.connect(DB_CONNECTION_STRING)
        cursor = dbConn.cursor(cursor_factory=psycopg2.extras.DictCursor)
        nome_cat = request.form["nome_cat"]
        tin = request.form["tin"]
        num_serie = request.form["num_serie"]
        fabricante = request.form["fabricante"]
        query = "INSERT INTO responsavel_por VALUES (%s, %s, %s, %s)"
        data = (nome_cat, tin, num_serie, fabricante)
        cursor.execute(query, data)
        return redirect(url_for("listar_editar_responsabilidades", tin=request.args.get("tin")))
    except Exception as e:
        return str(e)
    finally:
        dbConn.commit()
        cursor.close()
        dbConn.close()

@app.route("/ivms")
def listar_ivms():
    dbConn = None
    cursor = None
    try:
        dbConn = psycopg2.connect(DB_CONNECTION_STRING)
        cursor = dbConn.cursor(cursor_factory=psycopg2.extras.DictCursor)
        query = "SELECT * FROM IVM"
        cursor.execute(query)
        return render_template("ivms.html", cursor=cursor)
    except Exception as e:
        return str(e)
    finally:
        cursor.close()
        dbConn.close()

@app.route("/eventos_reposicao")
def listar_eventos_reposicao():
    dbConn = None
    cursor_evento_reposicao = None
    cursor_unidades_categoria = None
    try:
        dbConn = psycopg2.connect(DB_CONNECTION_STRING)
        cursor_evento_reposicao = dbConn.cursor(cursor_factory=psycopg2.extras.DictCursor)
        cursor_unidades_categoria = dbConn.cursor(cursor_factory=psycopg2.extras.DictCursor)
        num_serie = request.args["num_serie"]
        fabricante = request.args["fabricante"]
        query_evento_reposicao = "SELECT * FROM evento_reposicao WHERE num_serie=%s AND fabricante=%s"
        query_unidades_categoria = "SELECT SUM(unidades), cat FROM (SELECT * FROM evento_reposicao NATURAL JOIN produto WHERE num_serie=%s AND fabricante=%s) AS evento_reposicao_ivm GROUP BY cat"
        data = (num_serie, fabricante)
        cursor_evento_reposicao.execute(query_evento_reposicao, data)
        cursor_unidades_categoria.execute(query_unidades_categoria, data)
        return render_template("eventos_reposicao.html", cursor_evento_reposicao=cursor_evento_reposicao, cursor_unidades_categoria=cursor_unidades_categoria, params=request.args)
    except Exception as e:
        return str(e)
    finally:
        cursor_evento_reposicao.close()
        cursor_unidades_categoria.close()
        dbConn.close()

@app.route("/super_categorias")
def listar_super_categorias():
    dbConn = None
    cursor = None
    try:
        dbConn = psycopg2.connect(DB_CONNECTION_STRING)
        cursor = dbConn.cursor(cursor_factory=psycopg2.extras.DictCursor)
        query = "SELECT * FROM super_categoria"
        cursor.execute(query)
        return render_template("super_categorias.html", cursor=cursor)
    except Exception as e:
        return str(e)
    finally:
        cursor.close()
        dbConn.close()

@app.route("/sub_categorias")
def listar_sub_categorias():
    dbConn = None
    cursor = None
    try:
        dbConn = psycopg2.connect(DB_CONNECTION_STRING)
        cursor = dbConn.cursor(cursor_factory=psycopg2.extras.DictCursor)
        sub_categorias = []
        super_categoria = request.args["super_categoria"]
        categorias_qeue = [super_categoria]
        query = "SELECT * FROM tem_outra WHERE super_categoria=%s"
        while(categorias_qeue):
            super_categoria = categorias_qeue.pop(0)
            data = (super_categoria,)
            cursor.execute(query, data)
            sub_categorias_diretas = cursor.fetchall()
            sub_categorias += sub_categorias_diretas
            for tem_outra in sub_categorias_diretas:
                categorias_qeue += [tem_outra[1]]
        return render_template("sub_categorias.html", sub_categorias=sub_categorias, params=request.args)
    except Exception as e:
        return str(e)
    finally:
        cursor.close()
        dbConn.close()

CGIHandler().run(app)
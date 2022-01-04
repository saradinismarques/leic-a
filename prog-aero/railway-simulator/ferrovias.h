#ifndef FERROVIAS_H_
#define FERROVIAS_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DIM 100

typedef struct ponto{                                     /*criaçao de um tipo estruturado PONTO*/
	char nome[DIM];
	int x;
	int y;
	char cor[DIM];
	char tipo[DIM];
	int p_entrada, p_saida;
}PONTO;

typedef struct lista_pontos{  
	char nome[DIM];                        		  /*criaçao de um tipo estruturado LISTAPONTO para usar na lista*/
	PONTO p;
	struct lista_pontos *prox;
	struct lista_pontos *seg;
}LISTAPONTO;

typedef struct comboio{                                   /*criaçao de um tipo estruturado COMBOIO*/
	char nome[DIM];
	int raio;
	char cor[DIM];
	char caminho[DIM];
	char pinicial[DIM];
	int vidas;
}COMBOIO;

typedef struct lista_comboio{                             /*criaçao de um tipo estruturado LISTACOMBOIO para usar na lista*/
	COMBOIO c;
	struct lista_comboio *prox;
}LISTACOMBOIO;


LISTAPONTO *insere_na_lista_ponto(LISTAPONTO *inicio[DIM], PONTO p, int i, char nome_ferrovia[DIM]);

LISTACOMBOIO *insere_na_lista_comboio(LISTACOMBOIO *inicio2, COMBOIO c);

void le_dados_janela(char nome_ficheiro[DIM], int *xjanela, int *yjanela);

int mostra_pontos(LISTAPONTO *inicio[DIM], int nferrovia);

int mostra_comboio(LISTACOMBOIO *inicio2);

void inicia_lista(LISTAPONTO *inicio[DIM]);

void le_dados_linha(char nome_ficheiro[DIM], LISTAPONTO *inicio[DIM], int *nferrovias);

LISTACOMBOIO *le_dados_comboio(char nome_ficheiro[DIM], LISTACOMBOIO *inicio2);

int elimina_ferrovia (LISTAPONTO *inicio[DIM], int nferrovia);

LISTACOMBOIO *elimina_comboio(LISTACOMBOIO *inicio2);

LISTACOMBOIO* cria_comboio (LISTACOMBOIO *inicio2);

LISTAPONTO* procura_p_ligacao (LISTAPONTO *inicio [DIM], char ferrovia_p [DIM], char ponto [DIM], int nferrovia);

void le_dados_ligar_linhas(LISTAPONTO *inicio [DIM], char nome_ficheiro[DIM], int nferrovias);


#endif

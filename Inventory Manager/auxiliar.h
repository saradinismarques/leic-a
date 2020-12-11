/* ____________________________________________________________________
  | Ficheiro: auxiliar.h                                               |
  | Autor: Sara Marques (93342)                                        |
  | Descrição: Contem as bibliotecas, constantes, estruturas           |
  | do projeto e declaração das variáveis globais e funções auxiliares |
  |____________________________________________________________________|*/

/*bibliotecas*/
#include<stdio.h>
#include<string.h>

/*constantes*/
#define MAX_DESC 63
#define MAX_PROD 10000
#define MAX_ENC 500
#define MAX_PESO 200

/*estruturas*/
typedef struct{ /*tipo PROD com as características de um produto*/
    int idp; /*identificador*/
    char desc[MAX_DESC]; /*descrição*/
    int preco; /*preço*/
    int peso; /*peso*/
    int qtd; /*quantidade*/
}PROD;

typedef struct{ /*tipo ENC com as características de uma encomenda*/
    int ide; /*identificador*/
    PROD p[MAX_PROD]; /*vetor do tipo PROD (cada encomenda é um conjunto de produtos)*/
}ENC;

/*variáveis globais*/
PROD prod[MAX_PROD]; /*vetor do tipo PROD com todos os produtos criados*/
ENC enc[MAX_ENC]; /*vetor do tipo ENC com todas as encomendas criadas*/

/*funções auxiliares*/
void init_prod(PROD p[]);
void init_enc();
PROD p_cpy(PROD p);
void bubble_sort(PROD p[], int size, int n);
void quick_sort(PROD p[], int left, int right, int n);
int partition(PROD p[], int left, int right, int n);
void merge_sort(PROD p[], int left, int right, int n);
void merge(PROD p[], int left, int m, int right, int n);
int cmp_prc(PROD p1, PROD p2);
int cmp_desc(PROD p1, PROD p2);
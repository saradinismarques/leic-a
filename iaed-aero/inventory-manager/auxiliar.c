/* _________________________________________
  | Ficheiro: auxiliar.c                    |
  | Autor: Sara Marques (93342)             |
  | Descrição: Contem as funções auxiliares |
  |_________________________________________|*/

#include "auxiliar.h"
/*função para inicializar a variável global prod*/
void init_prod(PROD p[]){
    int i;

    for(i=0; i<MAX_PROD; i++){
        p[i].idp=-1;
        strcpy(p[i].desc, "");
        p[i].preco=0;
        p[i].peso=0;
        p[i].qtd=0;
    }
    return;
}
/*função para inicializar a variável global enc*/
void init_enc(ENC e[]){
    int i;

    for(i=0; i<MAX_ENC; i++){
        e[i].ide=-1;
        init_prod(e[i].p); /*inicializa o vetor de produtos de cada encomenda*/
    }
    return;
}
/*função que recebe um produto e copia os seu dados*/
PROD p_cpy(PROD p){
    PROD p_cpy;

    p_cpy.idp=p.idp;
    strcpy(p_cpy.desc, p.desc);
    p_cpy.preco=p.preco;
    p_cpy.peso=p.peso;
    p_cpy.qtd=p.qtd;
    return p_cpy; /*retorna a cópia*/
}
/*função para comparar dois preços, necessária na função l (cmp=0)*/
int cmp_prc(PROD p1, PROD p2){
    if(p1.preco>=p2.preco)
        return 1;

    return 0;    
}
/*função para comparar duas descriões por ordem alfabética, necessária na função L (cmp=1)*/
int cmp_desc(PROD p1, PROD p2){
    if(strcmp(p1.desc, p2.desc)>=0)
        return 1;

    return 0;
    
}
/*função para o algoritmo de ordenação (bubble sort) que recebe um vetor, o seu tamanho, e um número para a comparação que é suposto fazer*/
void bubble_sort(PROD p[], int size, int n){
    int i, j;
    PROD aux; /*variável auxiliar para realizar a troca*/

    for(i=0; i<size-1; i++){      
        for(j=0; j<size-i-1; j++){  
            if ((cmp_prc(p[j], p[j+1]) && !n) || (cmp_desc(p[j], p[j+1]) && n)){ /*caso a comparação retorne 1 troca os dois valores*/
                aux=p_cpy(p[j]);
                p[j]=p_cpy(p[j+1]);
                p[j+1]=p_cpy(aux);
            }
        }
    }  
    return;
}
/*função para o algoritmo de ordenação (quick sort) que recebe um vetor, o seu tamanho, e um número para a comparação que é suposto fazer*/
void quick_sort(PROD p[], int left, int right, int n){
    int i;
    if(right<=left)
        return;
    i=partition(p, left, right, n);
    quick_sort(p, left, i-1, n);
    quick_sort(p, i+1, right, n);
}
/*função partition*/
int partition(PROD p[], int left, int right, int n){
    int i=left-1, j=right;
    PROD pivot=p[right], aux;

    while(i<j){
        while((cmp_prc(p[i], pivot)==0 && !n) || (cmp_desc(p[i], pivot)==0 && n))
            ++i;
        while((cmp_prc(p[j], pivot)==1 && !n) || (cmp_desc(p[j], pivot)==1 && n))
            --j;
        if(i<j){
            aux=p_cpy(p[i]);
            p[i]=p_cpy(p[j]);
            p[j]=p_cpy(aux);
        }
    }
        aux=p_cpy(pivot);
        pivot=p_cpy(p[i]);
        p[i]=p_cpy(aux);

    return i;
}
/*função para o algoritmo de ordenação (merge sort) que recebe um vetor, o seu tamanho, e um número para a comparação que é suposto fazer*/
void merge_sort(PROD p[], int left, int right, int n){
    int m=(right+left)/2;
    if(right<=left)
        return;
    merge_sort(p, left, m, n);
    merge_sort(p, m+1, right, n);
    merge(p, left, m, right, n);
}
/*função merge*/
void merge(PROD p[], int left, int m, int right, int n){ 
      int i, j, k;
      PROD aux[MAX_PROD];
      for(i=m+1; i>left; i--) 
        aux[i-1]=p[i-1];
      for(j=m; j<right; j++)
        aux[right+m-j]=p[j+1];
      for(k=left; k<=right; k++)
        if((cmp_prc(aux[j], aux[i])==0 && !n) || (cmp_desc(aux[j], aux[i])==0 && n) || i>m)
          p[k]=aux[j--];
        else
          p[k]=aux[i++];
}
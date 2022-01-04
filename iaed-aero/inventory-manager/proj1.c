/* __________________________________________
  | Ficheiro: proj1.c                        |
  | Autor: Sara Marques (93342)              |
  | Descrição: Contem a função main e as     |
  | funções principais, uma por cada comando |
  |__________________________________________|*/

#include "auxiliar.h"
/*funções principais*/
void a(PROD p[]);
void q(PROD p[]);
void N(ENC e[]);
void A(PROD p[], ENC e[]);
void r(PROD p[]);
void R(PROD p[], ENC e[]);
void C(ENC e[]);
void p(PROD p[], ENC e[]);
void E(PROD p[], ENC e[]);
void m(PROD p[], ENC e[]);
void l(PROD p[]);
void L(ENC e[]);

/*função main*/
int main(){
    char cmd; /*variávell onde a instrução vai ser guardada*/

    init_prod(prod); /*inicialização das duas variáveis*/
    init_enc(enc);   /*globais usadas durante todo o programa*/

    do{ /*ciclo que lê o comando para depois realizar a respetiva função*/
        cmd=getchar();
        switch(cmd){
            case 'a':
                cmd=getchar();
                a(prod);
                break;
            case 'q':
                q(prod);
                break;
            case 'N':
                N(enc);
                break;
            case 'A':
                A(prod, enc);
                break;
            case 'r':
                r(prod);
                break;
            case 'R':
                R(prod, enc);
                break;
            case 'C':
                C(enc);
                break;
            case 'p':
                p(prod, enc);
                break;
            case 'E':
                E(prod, enc);
                break;
            case 'm':
                m(prod, enc);
                break;
            case 'l':
                l(prod);
                break;
            case 'L':
                L(enc);
                break;
        }
    }while(cmd!='x');
    return 0;
}
/*função para adicionar um novo produto ao sistema*/
void a(PROD p[]){
    int i;

    for(i=0; i<MAX_PROD; i++){ /*procura uma posição sem produto para adicionar o novo*/
        if(p[i].idp==-1){
            p[i].idp=i; /*o idp do novo produto é igual à sua posição*/
            scanf("%[^:]:%d:%d:%d", p[i].desc, &(p[i].preco), &(p[i].peso), &(p[i].qtd)); /*nessa posição põe os dados do novo produto introduzidos*/
            printf("Novo produto %d.\n", p[i].idp);
            return;
        }
    }
    return;
}
/*função para adicionar stock a um produto existente no sistema*/
void q(PROD p[]){
    int i, q;

    scanf("%d:%d", &i, &q); /*lê o produto e a quantidade a adicionar*/
    if(p[i].idp==-1){ /*erro caso não exista o produto no sistema*/
        printf("Impossivel adicionar produto %d ao stock. Produto inexistente.\n", i);
        return;
    }
    p[i].qtd+=q; /*adiciona a quantidade nova à que já lá está*/
    return;
}
/*função para criar uma nova encomenda*/
void N(ENC e[]){
    int i;

    for(i=0; i<MAX_ENC; i++){ /*procura uma posição sem encomenda para adicionar a nova*/
        if(e[i].ide==-1){
            e[i].ide=i; /*o ide da nova encomenda é igual à sua posição*/
            printf("Nova encomenda %d.\n", e[i].ide);
            return;
        }
    }
    return;
}
/*função para adicionar um produto a uma encomenda*/
void A(PROD p[], ENC e[]){
    int i, j, k, q, p_enc=0;

    scanf("%d:%d:%d", &j, &k, &q); /*lê ide, idp e qtd introduzidos*/
    if(e[j].ide==-1){ /*erro caso não exista essa encomenda*/
        printf("Impossivel adicionar produto %d a encomenda %d. Encomenda inexistente.\n", k, j);
        return;
    }
    if(p[k].idp==-1){ /*erro caso não exista esse produto*/
        printf("Impossivel adicionar produto %d a encomenda %d. Produto inexistente.\n", k, j);
        return;
    }
    if(p[k].qtd<q){ /*erro caso não exista stock suficiente para satisfazer a encomenda*/
        printf("Impossivel adicionar produto %d a encomenda %d. Quantidade em stock insuficiente.\n", k, j);
        return;
    }
    for(i=0; i<MAX_PROD; i++){ /*ciclo para calcular o peso que a encomenda já tem*/
        p_enc+=(e[j].p[i].peso)*(e[j].p[i].qtd); 
    }
    if((p_enc+p[k].peso*q)>MAX_PESO){ /*erro caso o peso total exceder o peso máximo permitido*/
        printf("Impossivel adicionar produto %d a encomenda %d. Peso da encomenda excede o maximo de 200.\n", k, j);
        return;
    }
    if(e[j].p[k].qtd!=0){ /*se o produto já existir na encomenda aumenta a quantidade*/
        e[j].p[k].qtd+=q;
    }
    else{ /*se não existir copia os dados desse produto (que está na posição igual ao seu idp) para a encomenda*/
        e[j].p[k]=p_cpy(p[k]); 
        e[j].p[k].qtd=q; /*coloca apenas quantidade introduzida*/
    }
    p[k].qtd-=q; /*retira stock do produto do sistema*/
    return;
}
/*função para remover stock a um produto existente*/
void r(PROD p[]){
    int i, q;

    scanf("%d:%d", &i, &q); /*lê idp e qtd introduzidos*/
    if(p[i].idp==-1){ /*erro caso não exista esse produto*/
        printf("Impossivel remover stock do produto %d. Produto inexistente.\n", i);
        return;
    }
    if(p[i].qtd<q){ /*erro caso a quantidade de stock restante após remoção ser negativa*/
        printf("Impossivel remover %d unidades do produto %d do stock. Quantidade insuficiente.\n", q, i);
        return;
    }
    p[i].qtd-=q; /*diminui a quantidade*/
    return;
}
/*função para remover um produto de uma encomenda*/
void R(PROD p[], ENC e[]){
    int i, j;

    scanf("%d:%d", &i, &j); /*lê ide e idp introduzidos*/
    if(e[i].ide==-1){ /*erro caso não exista essa encomenda*/
        printf("Impossivel remover produto %d a encomenda %d. Encomenda inexistente.\n", j, i);
        return;
    }
    if(p[j].idp==-1){ /*erro caso não exista esse produt*/
        printf("Impossivel remover produto %d a encomenda %d. Produto inexistente.\n", j, i);
        return;
    }
    p[j].qtd+=e[i].p[j].qtd; /*repõe stock ao produto no sistema*/
    e[i].p[j].qtd=0; /*atualiza a quantidade a zero na encomenda*/
    return;
}
/*função para calcular o custo de uma encomend*/
void C(ENC e[]){
    int i, j, prc_enc=0;

    scanf("%d", &j); /*lê ide introduzido*/
    if(e[j].ide==-1){ /*erro caso não exista essa encomenda*/
        printf("Impossivel calcular custo da encomenda %d. Encomenda inexistente.\n", j);
        return;
    }
    for(i=0; i<MAX_PROD; i++){ /*ciclo que vai somando o preço de cada produto por quantidade*/
            prc_enc+=(e[j].p[i].preco)*(e[j].p[i].qtd);
    }
    printf("Custo da encomenda %d %d.\n", j, prc_enc);
    return;
}
/*função para alterar o preço de um produto*/
void p(PROD p[], ENC e[]){
    int i, j, prc_novo;

    scanf("%d:%d", &i, &prc_novo); /*lê o idp e o novo preço introduzidos*/
    if(p[i].idp==-1){ /*erro caso não exista esse produto*/
        printf("Impossivel alterar preco do produto %d. Produto inexistente.\n", i);
        return;
    }
    p[i].preco=prc_novo; /*altera o preço no sistema*/
    for(j=0; j<MAX_ENC; j++){ /*ciclo para alterar em todas as encomendas que tenham esse produto*/
        if(e[j].ide!=-1 && e[j].p[i].idp!=-1)  
            e[j].p[i].preco=prc_novo;
    }
    return;
}
/*função para listar a descrição e quantidade de um produto numa encomenda*/
void E(PROD p[], ENC e[]){
    int i, j;

    scanf("%d:%d", &i, &j); /*lê o ide e idp introduzidos*/
    if(e[i].ide==-1){ /*erro caso não exista essa encomenda*/
        printf("Impossivel listar encomenda %d. Encomenda inexistente.\n", i);
        return;
    }
    if(p[j].idp==-1){ /*erro caso não exista esse produto*/
        printf("Impossivel listar produto %d. Produto inexistente.\n", j);
        return;
    }
    printf("%s %d.\n", p[j].desc, e[i].p[j].qtd); /*imprime descrição e quantidade desse produto*/
    return;
}     
/*função para listar o identificador da encomenda em que o produto dado ocorre mais vezes*/
void m(PROD p[], ENC e[]){
    int i, j, max=0, imax=-1;

    scanf("%d", &i); /*lê idp introduzido*/
    if(p[i].idp==-1){ /*erro caso não exista esse produto*/
        printf("Impossivel listar maximo do produto %d. Produto inexistente.\n", i);
        return;
    }
    for(j=0; j<MAX_ENC; j++){ /*ciclo para determinar quantidade máxima e índice correspondente*/
        if(e[j].p[i].qtd>max){
            max=e[j].p[i].qtd; /*atualiza quantidade máxima*/
            imax=j; /*atualiza o índice correspondente*/
        }
    }
    if(imax==-1) /*caso o produto não exista em nenhuma encomenda não faz nada*/
        return;
    
    printf("Maximo produto %d %d %d.\n", i, e[imax].ide, e[imax].p[i].qtd);
    return;
}
/*função para listar todos os produtos existentes no sistema por ordem crescente de preço*/
void l(PROD p[]){
    int i=0, size=0, cmp=0; /*cmp é a comparação que vai ser feita para ordenar*/
    PROD aux[MAX_PROD]; /*variável auxiliar (que vai ser uma cópia da variável global prod) e que vai ser ordenada*/

    while(p[i].idp!=-1){ /*copia todos os produtos do sistema para a variável auxiliar*/
        aux[i]=p_cpy(p[i]);
        i++;
    }
    size=i;
    /*bubble_sort(aux, size, cmp);*/
    /*quick_sort(aux, 0, size-1, cmp);*/
    merge_sort(aux, 0, size-1, cmp); /*algoritmo de ordenação*/
    
    printf("Produtos\n");
    for(i=0; i<size; i++){ /*imprime variavél auxiliar depois de ordenada*/
        printf("* %s %d %d\n", aux[i].desc, aux[i].preco, aux[i].qtd);
    }
    return;
}
/*função para listar todos os produtos de uma encomenda por ordem alfabética da descrição*/
void L(ENC e[]){
    int i, j, size=0, pos[MAX_PROD], cmp=1; /*cmp é a comparação que vai ser feita para ordenar*/
    PROD aux[MAX_PESO]; /*variável auxiliar (que vai ser uma cópia de um vetor de produtos da variável global enc) e que vai ser ordenada*/

    scanf("%d", &i); /*lê ide introduzido*/
    if(e[i].ide==-1){ /*erro caso não exista essa encomenda*/
        printf("Impossivel listar encomenda %d. Encomenda inexistente.\n", i);
        return;
    }
    for(j=0; j<MAX_PROD; j++){ /*ciclo que guarda as posições em que existam produtos nessa encomenda no vetor pos*/
        if(e[i].p[j].qtd!=0){
            pos[size]=j;
            size++;
        }
    }
    for(j=0; j<size; j++){ /*copia todos os produtos existentes na encomenda "saltando" as posições sem produto*/
        aux[j]=p_cpy(e[i].p[pos[j]]);
    }
    /*bubble_sort(aux, size, cmp);*/
    /*quick_sort(aux, 0, size-1, cmp);*/
    merge_sort(aux, 0, size-1, cmp); /*algoritmo de ordenação*/

    printf("Encomenda %d\n", e[i].ide);
    for(j=0; j<size; j++){ /*imprime variavél auxiliar depois de ordenada*/
        printf("* %s %d %d\n", aux[j].desc, aux[j].preco, aux[j].qtd);
    }
    return;
}

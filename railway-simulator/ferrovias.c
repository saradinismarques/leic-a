#include "ferrovias.h"

/*inicia a lista a NULL*/
void inicia_lista(LISTAPONTO *inicio[DIM]){
	int i;
	for(i=0;i<DIM;i=i+1){
		inicio[i]=NULL;
	}
}

/*cria uma lista para os pontos*/
LISTAPONTO* insere_na_lista_ponto(LISTAPONTO *inicio[DIM], PONTO p, int i, char nome_ferrovia[DIM]){ 
		LISTAPONTO *aux[DIM], *novo[DIM];
		novo[i] = (LISTAPONTO*) calloc (1, sizeof(LISTAPONTO));           /*criação de um bloco de memória para guardar todos os*/
		                                                                  /*pontos na lista*/
		if (novo[i] == NULL) {                                            /*caso o bloco seja igual a NULL ocorreu um erro na alocação de memória*/
			printf("Falta de memoria durante a insercao!\n");
			exit (0);
  		}
 		else {
			novo[i]->p=p;                                             /*inicializa o bloco de memória*/
			novo[i]->prox=NULL;
			strcpy(novo[i]->nome, nome_ferrovia);                     /*copia o nome da ferrovia para o campo nome*/
			novo[i]->p.p_entrada=1;                                   /*inicializa o campo ponto de entrada com 1*/
			novo[i]->p.p_saida=1;                                     /*inicializa o campo ponto de saída com 1*/ 
			if (inicio[i] == NULL) {
 				return novo[i];                                   /*1º elemento na lista*/                                 
			}
			aux[i] = inicio[i];
			while (aux[i]->prox != NULL){                             /*procura último elemento na lista*/
				aux[i] = aux[i]->prox;
			}
			aux[i]->prox = novo[i];                                   /*coloca o novo ponto nessa posição*/
			return inicio[i];                                         /*retorna a lista com os pontos existentes*/
		}
		
}		
 		
/*cria uma lista para os comboios*/
LISTACOMBOIO *insere_na_lista_comboio(LISTACOMBOIO *inicio2, COMBOIO c){         
		LISTACOMBOIO *aux, *novo;
		novo = (LISTACOMBOIO*) calloc (1, sizeof(LISTACOMBOIO));         /*criação de um bloco de memória para guardar todos os*/
                                                                                 /*comboios na lista*/
		if (novo == NULL) {                                              /*caso o bloco seja igual a NULL ocorreu um erro na alocação de memória*/
			printf("Falta de memoria durante a insercao!\n");
			exit (0);
		}
		novo->c=c;                                                       /*inicializa o bloco de memória*/                
		novo->prox=NULL;
		if (inicio2 == NULL) {
 			return novo;                                             /*1º elemento na lista*/                                                   
 		}
 		aux = inicio2;
		while (aux->prox != NULL){                                       /*procura último elemento na lista*/
			aux = aux->prox;
		}
		aux->prox = novo;                                                /*coloca o novo comboio nessa posição*/ 
		return inicio2;                                                  /*retorna a lista com os comboios existentes*/
}

/*le dados das dimensões da janela*/
void le_dados_janela(char nome_ficheiro[DIM], int *xjanela, int *yjanela){
	
	char linha[DIM];
	FILE* fp;
		
	fp=fopen(nome_ficheiro,"r");                                                      /*abre o ficheiro de configuração*/
	
		if(fp==NULL)
		{                                                                         /*testa a abertura do ficheiro*/
		  printf("Erro na abertura do fiheiro!\n");
		  exit (0);
		}
		while(fgets(linha, 100, fp)!=NULL){
	 	                                                                          /*lê cada linha do ficheiro*/ 
		  
		  if(strncmp(linha, "%", 1)!=0 && strncmp(linha, "\n", 1)!=0){
			
		  
		
		 	if(strncmp(linha, "JANELA:", 7)==0){
					sscanf(linha, "JANELA: %d %d", xjanela, yjanela); /*coloca os valores em duas variáveis*/
					                                                      
	  	  	}
		  }
		}
		fclose(fp);                                                               /*fecha o ficheiro*/ 
}

/*le dados da linha e guarda-os numa lista*/
void le_dados_linha(char nome_ficheiro[DIM], LISTAPONTO *inicio[DIM], int *nferrovia){   
		char nome_ferrovia[DIM]; 
		char fim_ferrovia[DIM];
		char linha[DIM];
		int ll=0;
	        int i=0;
		FILE* fp;
		PONTO p;
	
		fp=fopen(nome_ficheiro,"r");                                                      /*abre o ficheiro de configuração*/
	
		if(fp==NULL)
		{                                                                                 /*testa a abertura do ficheiro*/
		  	printf("Erro na abertura do fiheiro!\n");
		  	exit (0);
		}

		while(fgets(linha, 100, fp)!=NULL){                                               /*lê cada linha do ficheiro*/

		    if(strncmp(linha, "%", 1)!=0 && strncmp(linha, "\n", 1)!=0){                                           
		

	  	  	if(strncmp(linha, "LINHA:", 6)==0){                                       /*encontra a palavra LINHA:*/

				ll=1;                                                             
				sscanf(linha, "LINHA: %s", nome_ferrovia);                        /*faz o scan do nome da ferrovia*/
				
				
		  	}   

			else if(strncmp(linha, "FIM_DE_LINHA:", 13)==0){                          /*encontra a palavra FIM DE LINHA:*/

				ll=0;                                                   

				sscanf(linha, "FIM_DE_LINHA: %s", fim_ferrovia);                  /*faz o scan do nome da ferrovia*/
				
				i=i+1;
				*nferrovia=i;
				if(strcmp(nome_ferrovia, fim_ferrovia)!=0){                       /*compara se os nomes são iguais*/
					printf("Erro no processamento de linha.\n");              /*caso contrário dá erro*/
					exit(0);
	       	 		}                   
	 		}
		
			else if(ll==1){
				if(sscanf(linha,"%s %d %d %s %s", p.nome, &(p.x), &(p.y), p.cor, p.tipo)==5);    /*faz o scan das outras linhas*/ 
				
				inicio[i] = insere_na_lista_ponto(inicio, p, i, nome_ferrovia);           /*coloca-os na lista*/
	
			}   
		    }                                           					  /*até chegar ao fim de linha*/	
			
		  } 
					                                                                                                                                                                                                  
		fclose(fp);                                                                               /*fecha o ficheiro*/ 
}				                                                 

/*le dados dos comboios e guarda-os numa lista*/
LISTACOMBOIO *le_dados_comboio(char nome_ficheiro[DIM], LISTACOMBOIO *inicio2){  
                                                                 
		char linha[DIM];
		COMBOIO c;
		FILE* fp;

		fp=fopen(nome_ficheiro,"r");                                     /*abre o ficheiro de configuração*/

		if(fp==NULL)
		{                                                                /*testa a abertura do ficheiro*/
	 	 printf("Erro na abertura do fiheiro!\n");
	 	 exit (0);
		}
 	 	while(fgets(linha, 100, fp)!=NULL){                              /*lê cada linha do ficheiro*/
	 	                                                             
		  
		  if(strncmp(linha, "%", 1)!=0 && strncmp(linha, "\n", 1)!=0){   /*ignora as linhas que começam por % e espaço*/
		 	
                        /*encontra a palvra COMBOIO: e verifica se tem os dados suficientes para definir um comboio*/
   	  	  	if(sscanf(linha,"COMBOIO: %s %d %s %s %s %d", c.nome, &(c.raio), c.cor, c.caminho, c.pinicial, &(c.vidas))==6){
                                                
				inicio2 = insere_na_lista_comboio(inicio2, c);  /*insere cada comboio na lista*/ 
		  	}     

	       	  }                   
	 	}					                                             					
		fclose(fp);                                                     /*fecha o ficheiro*/
	return inicio2;                                                         /*retorna a lista com todos os comboios*/
}

/*mostra a informação de uma ferrovia escolhida pelo utilizador*/	
int mostra_pontos(LISTAPONTO *inicio[DIM], int nferrovia){                      
	int i;
	char ferrovia[DIM];
	LISTAPONTO *aux;

	printf("Qual é a ferrovia que deseja quer escolher?\n\n");
	for(i=0; i<nferrovia; i=i+1){                                           /*ciclo para mostrar todas as ferrovias*/
		printf("FERROVIA: %s\n", inicio[i]->nome);
	} 
        
	printf("\n");
	scanf("%s", ferrovia);                                                  /*guarda o nome da ferrovia escolhida na variavél ferrovia*/
	getchar();
	
	for(i=0; i<nferrovia; i=i+1){                                           /*percorre todas as ferrovias*/
		if(strcmp(inicio[i]->nome, ferrovia)==0){                       /*procura o nome da ferrovia pedida*/
			printf("FERROVIA: %s\n", ferrovia);
			aux=inicio[i];
			for (; aux != NULL; aux=aux->prox){                     /*ciclo parar mostra toda a informação sobre a ferrovia selecionada*/
				printf("Nome do ponto:%s\n", aux->p.nome);
				printf("Coordenada x:%d\n", aux->p.x);
				printf("Coordenada y:%d\n", aux->p.y);
				printf("Cor do ponto:%s\n", aux->p.cor);
				printf("Tipo do ponto:%s\n", aux->p.tipo);
				printf("\n");
			}
			return 0;
		}
	}
	printf("A ferrovia que selecionou não existe!\n");                      /*caso não encontre a ferrovia*/
	return 0;
}

/*mostra a informação de um comboio escolhido pelo utilizador*/
int mostra_comboio(LISTACOMBOIO *inicio2){                                      
	char comboio[DIM];
	LISTACOMBOIO *aux, *aux2;

	aux=inicio2;
	printf("Qual é o comboio que quer ver?\n");
	for(; aux!=NULL; aux=aux->prox){                                        /*ciclo para mostrar todos os elementos na lista*/
		printf("COMBOIO:%s\n", aux->c.nome);
	}
 	
	printf("\n");
        scanf("%s", comboio);                                                   /*guarda o nome do comboio escolhido na variavél comboio*/ 
	getchar();
	printf("COMBOIO:%s\n", comboio);
	
	aux2=inicio2;
	for(; aux2!=NULL; aux2=aux2->prox){                                     /*percorre os elementos da lista*/
		if(strcmp(aux2->c.nome, comboio)==0){                           /*procura o nome do comboio pedido*/
			
		        /*mostra a sua informação*/
			printf("Nome do comboio:%s\n", aux2->c.nome);
			printf("Raio dos círculos:%d\n", aux2->c.raio);
			printf("Cor do comboio:%s\n", aux2->c.cor);
			printf("Ferrovia em que viaja:%s\n", aux2->c.caminho);
			printf("Ponto de partida:%s\n", aux2->c.pinicial);
			printf("Número de viajens:%d\n", aux2->c.vidas);
		return 0;	
		}
	}
		
	printf("O comboio que selecionou não existe!\n");                       /*caso não encontre o comboio*/
	return 0;
}

/*elimina uma ferrovia escolhida pelo utilizador*/
int elimina_ferrovia (LISTAPONTO *inicio[DIM], int nferrovia) {                 
	int i;
	char ferrovia[DIM];
	LISTAPONTO *aux;

	printf("Qual é a ferrovia que deseja escolher?\n\n");
	for (i=0; i<nferrovia; i=i+1) {                                          /*ciclo para mostrar todas as ferrovias*/
		aux=inicio[i];
		printf ("FERROVIA: %s\n",aux->nome);
	}
	
	printf ("\n");
	scanf ("%s", ferrovia);                                                  /*guarda o nome da ferrovia escolhida na variavél ferrovia*/
	getchar();

	for (i=0; i<nferrovia; i++) {                                            /*percorre todas as ferrovias*/
		if (strcmp (inicio[i]->nome, ferrovia)==0) {                                              
			for (; inicio[i]!=NULL; inicio[i]=inicio[i]->prox) {     /*percorre os elementos da ferrovia selecionada*/
				free (inicio[i]);                                /*e vai eliminando*/
			}
			while (i<nferrovia) {
				inicio[i]=inicio[i+1];                           /*volta a colocar todos os números no vetor*/
				i=i+1;
			}
			printf ("A ferrovia que selecionou foi eliminada com sucesso!\n\n");
			return 1;
		}
	}
	printf("A ferrovia que selecionou não existe!\n\n");                    /*caso não encontre a ferrovia*/
	return 0;
}

/*elimina um comboio escolhido pelo utilizador*/
LISTACOMBOIO *elimina_comboio(LISTACOMBOIO *inicio2){                           
	LISTACOMBOIO *aux, *aux2, *atual;
	char comboio[DIM];

	aux=inicio2;
	printf("Qual é o comboio que pretende eliminar?\n");
	for(; aux!=NULL; aux=aux->prox){                                        /*ciclo para mostrar todos os elementos na lista*/
		printf("COMBOIO:%s\n", aux->c.nome);  
	}

	printf("\n");
	scanf("%s", comboio);                                                   /*guarda o nome do comboio escolhido na variavél comboio*/ 
	getchar();

	aux2=NULL;
  	atual=inicio2;
  	while (atual!=NULL) {                                                   /*até chegar ao fim da lista*/
    		if (strcmp(atual->c.nome, comboio)==0) {                        /*procura na lista o comboio com o nome que o utilizador escolheu*/
      			
			if (aux2==NULL) {                                       /*se o comboio escolhido for o primeiro elemento da lista*/

        			inicio2=inicio2->prox;                          /*a lista fica só com os elementos a partir do segundo*/
				printf ("O comboio que selecionou foi eliminada com sucesso!\n\n");

				return inicio2;                                 /*retorna a nova lista com os comboios*/
      			}                                                       /*se o comboio escolhido não for o primeiro elemento na lista*/
			aux2->prox=atual->prox;                                 /*o elemento anterior ao escolhido fica a apontar para o que está a seguir*/
			free (atual);                                           /*e liberta-se o bloco de memória que corresponde ao comboio escolhido*/

			printf ("O comboio que selecionou foi eliminada com sucesso!\n\n");
			return inicio2;                                        /*retorna a nova lista com os comboios*/
		}
        	
       aux2=atual;
       atual=atual->prox;
       }
       printf ("O comboio que selecionou não existe!\n");
       return inicio2;                                                         /*retorna a nova lista com os comboios*/
 
}

/*cria um comboio com os dados que o utilizador desejar*/
LISTACOMBOIO* cria_comboio (LISTACOMBOIO *inicio2) {                           
  LISTACOMBOIO *novo, *aux;

  novo = (LISTACOMBOIO*) calloc (1, sizeof(LISTACOMBOIO));                     /*criação de um novo bloco de memória para colocar o novo comboio*/
 	 if (novo==NULL) {
    		printf ("Falta de memoria durante a insercao!\n");
   	 exit (0);
  	 }
 	 novo->prox=NULL;                                                      /*inicia esse bloco com NULL pois já não vai ligar a mais nenhum comboio*/
         
	 /*pede informações ao utilizador sobre o comboio e guarda a informação no bloco de memória*/
  	 printf("Qual é o nome do novo comboio?\n");                
 	 scanf ("%s", novo->c.nome);                                                 
  	 getchar();

  	 printf ("Qual é o raio dos círculos do novo comboio?\n");
  	 scanf ("%d", &(novo->c.raio));
  	 getchar();

  	 printf ("Qual é a cor do novo comboio?\n");
  	 scanf ("%s", novo->c.cor);
  	 getchar();

 	 printf ("Em que ferrovia viaja o novo comboio?\n");
 	 scanf ("%s", novo->c.caminho);
         getchar();

  	 printf ("Qual é o ponto de partida do novo comboio?\n");
 	 scanf ("%s", novo->c.pinicial);
 	 getchar();

 	 printf ("Quantas viajem é que o novo comboio irá fazer?\n");
 	 scanf ("%d", &(novo->c.vidas));
 	 getchar();

 	 printf("\n");

  	 aux=inicio2;
  	 while (aux->prox!=NULL) {                                              /*procura o último elemento da lista*/
    		 aux=aux->prox;
  	 }
 	 aux->prox=novo;                                                        /*insere o novo comboio*/
	
	 printf ("O comboio foi criado com sucesso!\n\n");
	
  return inicio2;                                                               /*retorna a nova lista com todos os comboios*/
}

/*procura o ponto de ligação entre as ferrovias*/
LISTAPONTO* procura_p_ligacao (LISTAPONTO *inicio [DIM], char ferrovia_p [DIM], char ponto [DIM], int nferrovia) {     
	int i;
	LISTAPONTO *aux, *p_procurado;

	for (i=0; i<nferrovia; i=i+1) {                                         /*percorre todas as ferrovias*/
		if (strcmp (inicio[i]->nome, ferrovia_p)==0) {                  /*procura o nome da ferrovia pedida*/
			aux=inicio[i];
			for (; aux!=NULL; aux=aux->prox) {                      /*percorre os elementos da lista*/
				if (strcmp (aux->p.nome, ponto)==0) {           /*procura o nome do ponto de ligação*/
					p_procurado=aux;
					return p_procurado;                     /*retorna o ponto de ligação*/
				}
			}
		}
	}
	return NULL;                                                            /*se não encontrar retorna NULL*/
}

/*liga as ferrovias*/
void le_dados_ligar_linhas(LISTAPONTO *inicio [DIM], char nome_ficheiro[DIM], int nferrovias) {                        
	FILE *fp;
	LISTAPONTO *inicial, *final;
	char linha [DIM], linha_inicial [DIM], linha_final [DIM], ponto_inicial [DIM], ponto_final [DIM];

	fp = fopen (nome_ficheiro, "r");                                        /*abre o ficheiro de configuração*/

	if (fp==NULL) {
		printf("Erro na abertura do fiheiro!\n");                       /*testa a abertura do ficheiro*/
		exit (0);
	}

	while(fgets(linha, 100, fp)!=NULL){                                     /*lê cada linha do ficheiro*/
		
		/*ignora as linhas que começam por % e espaço*/
		if ((linha[0]!='%') && (linha[0]!='\n')) {                                              
			
			/*encontra a palvra LIGAR: e verifica se tem os dados suficientes para concretizar a ligação*/
			if (sscanf (linha, "LIGAR: %s %s %s %s", linha_final, ponto_final, linha_inicial, ponto_inicial)==4) {
				
				/*imprime toda a informação que designa a ligação*/
				printf ("LIGAR: %s %s %s %s \n", linha_final, ponto_final, linha_inicial, ponto_inicial);
				/*invoca a função procura ponto de ligação e coloca na variável final o ponto de saída*/
				final = procura_p_ligacao (inicio, linha_final, ponto_final, nferrovias);                
				                                                                                         
				/*se existir um ponto de saída invoca a função procura ponto de ligação*/
				/*e coloca na variável inicial o ponto de entrada*/
				if (final!=NULL) {                                                                       
					inicial = procura_p_ligacao (inicio, linha_inicial, ponto_inicial, nferrovias);  
					                                                                                 
					/*se existir um ponto de entrada*/
					if (inicial!=NULL) {  
                                             
                               			/*o ponto de entrada liga ao ponto de saída*/
						final->seg= inicial;                                                     
						final->p.p_saida=final->p.p_saida+1;                                         
						inicial->p.p_entrada=inicial->p.p_entrada+1;
					}
				}
			}
		}
	}
	printf ("\n");
	fclose(fp);                                                               /*fecha o ficheiro*/
}

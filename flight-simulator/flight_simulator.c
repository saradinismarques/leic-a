/*Trabalho intermédio de programação*/
/*Simulação de voo*/
/*Sara Marques, nº93342*/
/*Leonor Cordeiro, nº93288*/

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <SDL.h>
#define Pi 3.14159265359
#define DIM 1000
SDL_Window* g_pWindow = 0;
SDL_Renderer* g_pRenderer = 0;

  void abre_ficheiro_dados(double parametro []) {
	char linha[DIM];
	int i = 0;
	FILE *fp;
	
	fp=fopen("config_model.txt","r");                   /*abre o ficheiro de configuração*/
	
	if(fp==NULL){                                       /*testa a abertura do ficheiro*/
	  printf("Erro na abertura do fiheiro!\n");
	  exit (0);      
	}
	do{
	 fgets(linha, 100, fp);                             /*lê cada linha do ficheiro*/ 

	 if (linha[0]=='%'){                                /*se a linha começar por % não lê essa linha*/
	   continue;
	 }
	 else{
	   sscanf(linha,"%le", &parametro[i]);              /*faz o scan dos valores no ficheiro e*/ 
	   i=i+1;                                           /*atribui-os sucessivamentao ao vetor*/  
	 }                                                  /*parametro até chegar ao fim do*/
	}while(feof(fp)==0);                                /*ficheiro*/
	 
	 fclose(fp);                                        /*fecha o ficheiro*/                       	
}

  void altera_novo_nome (char str[], char ch) {             /*altera o nome do novo ficheiro*/
  	char *pch;
  	
	pch=strchr(str,ch);
 	if (pch!=NULL){                                    
     	   *pch='\0';                                       /*substitui o \n final por um \0*/
        }
 }

  void nome_novo_ficheiro(char nome[]){                     /*o utilizador escolhe o nome do novo*/
	                                                    /*ficheiro*/ 
	printf("Introduza um nome do ficheiro: ");       
        fgets(nome,DIM,stdin);                              /*lê o nome*/
	altera_novo_nome (nome,'\n');

	
 }
 
  /*calcula as variaveis auxiliares AR, Cl, E, e Cd*/ 
  void calcula_vaux(double parametro[], double vaux[]){   
                                                            
	vaux[4]=(parametro[3]*parametro[3])/parametro[2];
  	vaux[5]=(parametro[9]*Pi*vaux[4])/(1.0+sqrt(1.0+(vaux[4]*vaux[4])/4.0));
  	vaux[6]=1.0/(Pi*parametro[8]*vaux[4]);
  	vaux[7]=parametro[7]+vaux[6]*vaux[5]*vaux[5];
 }

  /*inicializa as variavéis velocidade, ângulo, posição e altura*/
  void inicializa_variaveis(double parametro[], double variavel[]){       
                                                                         
	variavel[1]=parametro[10];                                         
  	variavel[2]=parametro[11];
  	variavel[3]=parametro[12];
  	variavel[4]=parametro[13];
 }

  /*calcula a variação das variavéis com o tempo e atualiza-as*/
  void calcula_variaveis(double parametro[], double variavel[], double vaux[]){  

        vaux[0]=variavel[1]+(parametro[1]/parametro[4])*(-(vaux[7]*(1.0/2)*parametro[6]*variavel[1]*variavel[1]*parametro[2])-parametro[4]*parametro[5]*sin(variavel[2]));
	vaux[1]=variavel[2]+(parametro[1]/(parametro[4]*variavel[1]))*((vaux[5]*(1.0/2)*parametro[6]*variavel[1]*variavel[1]*parametro[2])-parametro[4]*parametro[5]*cos(variavel[2]));	
	vaux[2]=variavel[3]+variavel[1]*cos(variavel[2])*parametro[1];
   	vaux[3]=variavel[4]+variavel[1]*sin(variavel[2])*parametro[1];
    	    	
    	variavel[1]=vaux[0];
	variavel[2]=vaux[1];
	variavel[3]=vaux[2];
	variavel[4]=vaux[3];
 }
  
  
  void imprime_valores_ficheiro(double parametro[], double variavel[], double vaux[], char nome[]){
	FILE*fp1;
	
	fp1=fopen(nome,"w");                                       /*abre o novo ficheiro */
                                    
	if (fp1==NULL){                                            /*testa a abertura do ficheiro*/ 
	  printf("Erro na abertura do ficheiro!\n");            
	  exit(0);
        }

	/*imprime o valores lidos no ficheiro de configuração no novo ficheiro*/
        fprintf(fp1,"(%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le)\n", parametro[0], parametro[1], parametro[2], parametro[3], parametro[4], parametro[5], parametro[6], parametro[7], parametro[8], parametro[9]);
	
	/*ciclo para calcular as variavéis até um tempo final e enquanto a altura for maior que zero e imprimir esses valores no novo ficheiro*/
	for (variavel[0]=0.0; (variavel[0]<=parametro[0]) && (variavel[4]>0); variavel[0]=variavel[0]+parametro[1]){

	calcula_variaveis(parametro, variavel, vaux);

	fprintf(fp1,"%le\t%le\t%le\t%le\t%le\n", variavel[0], variavel[1], variavel[2], variavel[3], variavel[4]);
	}
	fclose(fp1);                                               /*fecha o ficheiro*/
 }
 
  /*calcula máximos e minimos e guarda os valores numa matriz*/
  void calcula_maximo_minimo(double variavel[], double mat[2][5]){
	int j;

	  for (j=0; j<5; j=j+1){
	    if(variavel[j]>mat[0][j]){                            /*calcula os máximos*/
	 	mat[0][j]=variavel[j];
	    }

	    if(variavel[j]<mat[1][j]){                            /*calcula os mínimos*/
	 	mat[1][j]=variavel[j];
	    }
	  }	
  }	     

  /*converte todos os valores para pixéis caso sejam utilizados no eixo dos xx*/
  void converte_pixeis_x (double mat[2][5], double variavel[5], int pixelx[5], int j){
    int pmax=620, pmin=300;

        pixelx[j]=pmin+((pmax-pmin)/(mat[0][j]-mat[1][j]))*(variavel[j]-mat[1][j]);
     
  }

  /*converte todos os valores para pixéis caso sejam utilizados no eixo dos yy*/
  void converte_pixeis_y (double mat[2][5], double variavel[5], int pixely[5], int k){
    int pmax=460, pmin=220;

        pixely[k]=pmin+((pmax-pmin)/(mat[0][k]-mat[1][k]))*(variavel[k]-mat[1][k]);
     
  }

  /*abre o ficheiro e calcula máximos e mínimos para as cinco variáveis*/
  void abre_ficheiro_dados_dois(double variavel[], char nome[], double mat[2][5]){
	FILE*fp2;
	char linha[DIM];
	fp2=fopen(nome,"r");                                  /*abre novamente o ficheiro criado*/

	if (fp2==NULL){                                       /*testa a abertura do ficheiro*/
	  printf("Erro na abertura do ficheiro!\n");
	  exit (0);
        }
	do{
	 fgets(linha, 1000, fp2);                             /*lê cada linha do ficheiro*/
	  
	 if (linha[0]=='('){                 		      /*se a linha não começar por ( não lê essa*/
	 continue;                                            /*linha*/
	 }

	 else{

	 /*faz o scan dos valores no ficheiro atribuindo o valor de cada coluna a cada variavel até chegar ao fim do ficheiro*/
	 sscanf(linha,"%le\t%le\t%le\t%le\t%le\n", &variavel[0], &variavel[1], &variavel[2], &variavel[3], &variavel[4]);

	 calcula_maximo_minimo(variavel, mat);

	 }
	}while (feof(fp2)==0);
	
	fclose(fp2);                                           /*fecha o ficheiro*/
}
  int desenha_grafico(double variavel[], double mat[2][5], char nome[]){
   int pixelx[5], pixely[5];
   int x, y, k, j;
   FILE*fp2;
   char linha[DIM];

   fp2=fopen(nome,"r");
   printf("Lista de variáveis:\n0-Tempo\n1-Velocidade\n2-Ângulo\n3-Posição\n4-Altura\nSelecione as variáveis x e y: ");	
   fgets(linha, DIM, stdin);                                  /*lê as opções escolhidas pelo utilizador*/
   sscanf(linha, "%d %d", &j, &k);                            /*atribui o valor às variavéis j e k*/
   if((j==k) || (j>4) || (k>4)){                              /*caso j e k sejam iguais ou sejam maiores*/ 
	       printf("Gráfico inválido!!!\n");               /*que quatro o gráfico é inválido*/
   }   

  if(SDL_Init(SDL_INIT_EVERYTHING) >= 0){                     /*inicializa a SDL*/                 
    /*cria a janela*/
    g_pWindow = SDL_CreateWindow("Análise de voo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 
SDL_WINDOW_SHOWN);
   if(g_pWindow != 0){                                        /*testa a criação da janela*/ 
      g_pRenderer = SDL_CreateRenderer(g_pWindow, -1, 0);     /*cria o renderer*/
   }
  }
  else{
   return 1;                                                  /*caso não consiga iniciar a SDL retorna 1*/
  }
   SDL_SetRenderDrawColor(g_pRenderer, 0, 0, 0, 255);         /*define a cor preto*/ 
   SDL_RenderClear(g_pRenderer);                              /*"pinta" o fundo de preto*/
   SDL_SetRenderDrawColor(g_pRenderer, 0, 0, 255, 255);       /*define a cor azul*/
   SDL_RenderDrawLine(g_pRenderer, 20, 220, 620, 220);        /*desenha o eixo dos xx a azul*/
   SDL_RenderDrawLine(g_pRenderer, 300, 20, 300, 460);        /*desenha o eixo dos yy a azul*/
   
   

   /*imprime menu para o utilizador escolher o que quer no eixo dos xx e no eixo dos yy*/
   
   do{
	 fgets(linha, 1000, fp2);                             /*lê cada linha do ficheiro*/
	 if (linha[0]=='('){                                  /*se a linha começar por ( não lê essa*/
	   continue;                                          /*linha*/
	 }
	   else{
	   /*faz o scan dos valores no ficheiro atribuindo o valor de cada coluna a cada variavel até chegar ao fim do ficheiro*/
	   sscanf(linha,"%le\t%le\t%le\t%le\t%le\n", &variavel[0], &variavel[1], &variavel[2], &variavel[3], &variavel[4]);                    
	   converte_pixeis_x(mat, variavel, pixelx, j);         
     	   converte_pixeis_y(mat, variavel, pixely, k);                                                       

     	   x=pixelx[j];                                             
     	   y=pixely[k];
     	   SDL_SetRenderDrawColor( g_pRenderer, 0, 0, 255, 255 );
     	   SDL_RenderDrawPoint(g_pRenderer, 300+x, 460-(y+220));     /*desenha os pontos com base nos valores*/
           }                                                         /*dos pixéis calculados*/
   }while (feof(fp2)==0);                                  
	
	SDL_RenderPresent(g_pRenderer);                              /*mostra a janela*/
  	fflush(stdout);
  	getchar();	
	fclose(fp2);                                                 /*fecha o ficheiro*/

  SDL_Quit();                                                        /*sai da SDL*/
return 0;
}

  /*volta a calcular as variavéis num ciclo retornando o valor da distância final*/  
  double retorna_distancia_percorrida(double parametro[], double variavel[], double vaux[]){

	for (variavel[0]=0.0; (variavel[0]<=parametro[0]) && (variavel[4]>0); variavel[0]=variavel[0]+parametro[1]){

	calcula_variaveis(parametro, variavel, vaux);

        }
	return variavel[3];
  }
 
  /*calcula ângulo de ataque que maximiza a distância percorrida*/ 
  void calcula_angulo_ideal(double parametro[], double variavel[], double vaux[]){
	double distancia_max=0;
	double angulo_ideal;
	
	abre_ficheiro_dados(parametro);            /*volta a abrir o ficheiro de configuração e a*/ 
                                                   /*atribuir os valores ao vetor parametro*/
	                                           /*os valores ao vetor parametro*/
	parametro[9]=(Pi/2.0);                     /*o valor do parametro[9] que corresponde ao ângulo*/
                                                   /*de ataque é substituido por Pi*/
	while (parametro[9]>(-Pi/2.0)){            /*para cada ângulo de ataque até -Pi calcula as*/ 
	                                           /*variavéis*/
	    
	  calcula_vaux(parametro, vaux);
	  inicializa_variaveis(parametro, variavel);
	  retorna_distancia_percorrida(parametro, variavel, vaux);
	 
	  if(variavel[3]>distancia_max){           /*compara sucessivamente cada distância calculada*/
	     distancia_max=variavel[3];            /*substituindo o valor da distância máxima quando*/
	     angulo_ideal=parametro[9];            /*encontra uma distância maior*/
	  }
	 parametro[9]=parametro[9]-0.001;
	}
	printf("Quando o ângulo de ataque é %le radianos atinge-se a distância horizontal máxima de %le metros\n", angulo_ideal, distancia_max);
  }

int main (void)
{
  char opcao;
  double parametro [14];
  double variavel [5];
  double vaux [8];
  char nome[DIM];
  char linha [DIM];
  double mat[2][5]={{0,0,0,0,0},{10,10,10,10,10}};

 do {
	/*apresenta menu das opções*/
	printf("Simulação de voo\n\nLista de opções:\n0 – Terminar o programa\n1 – Simular o movimento da aeronave\n2 – Visualizar resultados (gráficos)\n3 – Calcular ângulo de ataque ideal\nSelecione a opção: ");

	fgets(linha, DIM, stdin);
        sscanf(linha, "%c", &opcao);

	 switch(opcao){
	  case '0':
		printf("Escolheu encerrar o programa.\n");
		return 0;
          

	  case '1':
	
		abre_ficheiro_dados(parametro);
		nome_novo_ficheiro(nome);
		calcula_vaux(parametro, vaux);
		inicializa_variaveis(parametro, variavel);
		imprime_valores_ficheiro(parametro, variavel, vaux, nome);

		break;
          
	
	  case '2':

		abre_ficheiro_dados_dois(variavel, nome, mat);
	        desenha_grafico(variavel, mat, nome);

	        break;
  
 	  case '3':
		
		printf("Aguarde um momento...\n");
		calcula_angulo_ideal(parametro, variavel, vaux);
	        break;
	  
	  default: 

	 	printf("Opção inválida. Tente outra vez!\n");
	  	break;
	} 
 }while (opcao != '0');
  return 0;
}


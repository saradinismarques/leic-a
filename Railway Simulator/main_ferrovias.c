/*Leonor Cordeiro, nº93288*/
/*Sara Marques, nº93342*/

#include "ferrovias.h"			


int main(int argc, char *argv[])
{
  char opcao, linha[DIM];
  LISTAPONTO *inicio[DIM];
  LISTACOMBOIO *inicio2=NULL;
  char nome_ficheiro[DIM];
  int xjanela, yjanela;
  int nferrovia=0;
  int aux;
	if (argc!=2){
			return 0;
		}

	strcpy (nome_ficheiro, argv[1]);
	le_dados_janela(nome_ficheiro, &xjanela, &yjanela);
	inicia_lista(inicio);
	le_dados_linha(nome_ficheiro, inicio, &nferrovia);
	inicio2=le_dados_comboio(nome_ficheiro, inicio2);
	printf("\n");

	do {                                                             /*apresenta menu das opções*/
		printf("Lista de opções:\n");
		printf("0 – Terminar o programa\n");
		printf("1 – Mostrar a informação de uma ferrovia\n");
		printf("2 – Eliminar uma ferrovia\n");
		printf("3 – Mostrar a informação de um comboio\n");
		printf("4 – Eliminar um comboio\n");
		printf("5 – Criar um comboio\n");
		printf("6 – Simulação dos comboios\n");
		printf("Selecione a opção: ");

		fgets(linha, DIM, stdin);
        	sscanf(linha, "%c", &opcao);
	

	 	switch(opcao){
	  	case '0':
			printf("Escolheu encerrar o programa!\n");
			return 0;
          

	  	case '1':
			
			printf("\n");
			mostra_pontos(inicio, nferrovia); 
			printf("\n");
			
			break;
          
	
	  	case '2':
			
			printf("\n");
			aux=elimina_ferrovia (inicio, nferrovia);

      			if (aux==1) {
        			nferrovia=nferrovia-1;                                   /*Diminui o numero de ferrovias existentes*/
			}
			printf("\n");

	        	break;
  
 	  	case '3':

			printf("\n");
			mostra_comboio(inicio2); 
			printf("\n");    
       
			break;

	  	case '4':

			printf("\n");
			inicio2=elimina_comboio(inicio2);
			printf("\n");

			break;

	  	case '5':

                        printf("\n");
			inicio2=cria_comboio(inicio2);
			printf("\n");
	 
			break;

	  	case '6':
			printf("\n");
			printf("Função em desenvolvimento...\n");
			printf("\n");

			break;
	  	default: 

	 		printf("Opção inválida! Tente outra vez!!!\n");
	  		break;
		} 
 	}while (opcao != '0');
  	return 0;
}

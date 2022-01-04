;===============================================================================
; Projeto de Introdução à Arquitetura de Computadores 
; Programa: Dino.as
;
; Grupo: 20
; Clara Pais, nº92622
; Sara Marques, nº93342
; 
; Data: 16/11/2020 				
;===============================================================================
;=================================================================
; CONSTANTES
;-----------------------------------------------------------------
; janela de texto
TERM_READ       EQU     FFFFh
TERM_WRITE      EQU     FFFEh
TERM_STATUS     EQU     FFFDh
TERM_CURSOR     EQU     FFFCh
TERM_COLOR      EQU     FFFBh
; display de 7 segmentos
DISP7_D0        EQU     FFF0h
DISP7_D1        EQU     FFF1h
DISP7_D2        EQU     FFF2h
DISP7_D3        EQU     FFF3h
DISP7_D4        EQU     FFEEh
DISP7_D5        EQU     FFEFh
; timer
TIMER_CONTROL   EQU     FFF7h
TIMER_COUNTER   EQU     FFF6h
TIMER_SETSTART  EQU     1
TIMER_SETSTOP   EQU     0
TIMERCOUNT_INIT EQU     1
; interrupções
INT_MASK        EQU     FFFAh
INT_MASK_VAL    EQU     8081h 
; stack
SP_INIT         EQU     6000h
; posições no terminal
POS_TERRENO     EQU     2200h
POS_NUVEM       EQU     0500h
POS_DINO_INI    EQU     220Ch
POS_GAMEOVER    EQU     1400h
POS_START       EQU     1500h
SALTO_MAX       EQU     0E0Ch
NEXT_LINE       EQU     0100h
; cores
AMARELO         EQU     00D8h  
PRETO           EQU     0000h
VERDE           EQU     0010h  
AZUL            EQU     003Bh
ROXO            EQU     00A7h
VERMELHO        EQU     00E5h
BRANCO          EQU     00FFh
; outras
N               EQU     80              ;dimensão do terreno de jogo
altura          EQU     8               ;altura máxima dos cactos
;=================================================================
; VARIÁVEIS GLOBAIS
;-----------------------------------------------------------------
                ORIG    2000h
; timer
TIMER_COUNTVAL  WORD    TIMERCOUNT_INIT 
TIMER_TICK      WORD    0               
TIME            WORD    0               
; display de 7 segmentos
DISP7_NUMS      TAB     6
DISP7_ADDR      STR     DISP7_D0,DISP7_D1,DISP7_D2,DISP7_D3,DISP7_D4,DISP7_D5
DIGITO          WORD    0
; outras
PRIMEIRO_JOGO   WORD    0               ;0-primeiro jogo, 1-outro 
INICIA_JOGO     WORD    0               ;0-jogo começou, 1-ainda não começou
POS_DINO_ATUAL  WORD    POS_DINO_INI
EXTREMOS_DINO   STR     220Ch, 220Dh, 210Eh, 200Fh, 2010h
DIRECAO_SALTO   WORD    0               ;0-cima, 1-baixo
SALTO           WORD    0               ;0-não está a saltar, 1-está a saltar
OLHO_DINO       WORD    0               ;0-olho normal, 1-olho gameo over
x               WORD    5               ;semente
; desenho no terminal
terrenocima     STR     '_/\__________________/\___________________________/\__/\_________________/\_____', 0
terrenobaixo    STR     '-   . -- _- .   __ -.   .   --   . - _  . _  -  .   -.     _ --.  _ . --    _.  ', 0

vazia           STR     '                                                                                ', 0      

nuvem1          STR     '          __                                                                    ', 0
nuvem2          STR     '        _(  )__                                                      __         ', 0
nuvem3          STR     '       (_______)                                                   _(  )__      ', 0
nuvem4          STR     '                                                                  (_______)     ', 0
nuvem5          STR     '                                                  __                            ', 0
nuvem6          STR     '                                               __(  )_                          ', 0
nuvem7          STR     '                                              (_______)                         ', 0

start           STR     '                                PRESS 0 TO START                                ', 0

gameover1       STR     '                     __   __        __     __        __  __                     ', 0
gameover2       STR     '                    | _  |__| |\/| |_     |  | \  / |_  |__)                    ', 0
gameover3       STR     '                    |__| |  | |  | |__    |__|  \/  |__ |  \                    ', 0

dino1           STR     '  __', 0
dino2           STR     ' |^_|', 0
dino3           STR     ' /|- ', 0
dino4           STR     '/_|', 0
dino5           STR     '||', 0 

dino6           STR     ' |X_|', 0    

cacto           STR     '#'

                ORIG    0000h  

JOGO            TAB     N            ;tabela em memória que representa o terreno de jogo

;=================================================================
; principal: função principal do programa
;-----------------------------------------------------------------
principal:      MVI     R6, SP_INIT           ;inicialização da pilha
                
                ;imprime elementos no terminal
                JAL     imprimeterreno
                JAL     imprimenuvens
                MVI     R1, POS_DINO_INI
                JAL     imprimedino
                
                MVI     R1, TERM_COLOR
                MVI 	R2, BRANCO
                STOR 	M[R1], R2
                
                MVI     R1, POS_START
                MVI     R2, start
                JAL     imprimelinha
                
                MVI     R1, INT_MASK          ;configura máscara de interrupções
                MVI     R2, INT_MASK_VAL
                STOR    M[R1], R2
                ENI                           ;permite interrupções
                              
espera:         MVI     R4, TERM_STATUS       ;espera que a variavél COMECA_JOGO fique a 1
                LOAD    R1, M[R4]             ;para começar o jogo
                CMP     R1, R0
                JAL.NZ  PROCESS_CHAR
                MVI     R1, INICIA_JOGO
                LOAD    R1, M[R1]
                CMP     R1, R0
                BR.Z    espera
                
                ;começa o jogo
comecajogo:     MVI     R1, INT_MASK          
                MVI     R2, INT_MASK_VAL
                MVI     R4, 0008h                
                ADD     R4, R4, R2
                STOR    M[R1], R4             ;adiciona à máscara de interrupções os saltos

                MVI     R1, PRIMEIRO_JOGO     ;verifica se é o primeiro jogo    
                LOAD    R1, M[R1]
                CMP     R1, R0
                BR.NZ   limpa
                MVI     R2, start
                MVI     R1, POS_START
                JAL     apagalinha
                BR      else
                
limpa:          JAL     limpatudo             ;se não for apaga tudo do terminal
                JAL     imprimeterreno        ;e imprime novamente
                JAL     imprimenuvens 
                
                MVI     R1, POS_DINO_INI
                JAL     imprimedino

                ;o timer começa a contar
else:           MVI     R2, TIMERCOUNT_INIT
                MVI     R1, TIMER_COUNTER
                STOR    M[R1], R2          
                MVI     R1, TIMER_TICK
                STOR    M[R1], R0          
                MVI     R1, TIMER_CONTROL
                MVI     R2, TIMER_SETSTART
                STOR    M[R1], R2          

                ;loop que vai atualizando o timer e o jogo e processar o caracter 
                MVI     R4, TERM_STATUS
                MVI     R5, TIMER_TICK
loop:           LOAD    R1, M[R5]
                CMP     R1, R0
                JAL.NZ  PROCESS_TIMER_EVENT
                LOAD    R1, M[R4]
                CMP     R1, R0
                JAL.NZ  PROCESS_CHAR
                MVI     R1, JOGO      ;coloca em R1 o endereço de início da tabela JOGO
                MVI     R2, N         ;coloca em R2 a dimensão da tabela
                JAL     atualizajogo
                BR      loop

;=================================================================
; PROCESS_TIMER_EVENT: processa eventos do timer
;-----------------------------------------------------------------
PROCESS_TIMER_EVENT:
                DEC     R6
                STOR    M[R6], R7
                MVI     R2, TIMER_TICK
                DSI                     ;região crítica (para as interrupcoes)
                LOAD    R1, M[R2]
                DEC     R1              ;retira um a TIMER_TICK
                STOR    M[R2], R1
                ENI

                MVI     R1, TIME        ;atualiza tempo
                LOAD    R2, M[R1]
                INC     R2
                STOR    M[R1], R2
                
                MVI     R1, DIGITO
                LOAD    R1, M[R1]
                
                JAL     conta           ;atualiza pontuacao
                               
                MVI     R2, DIGITO
                STOR    M[R2], R1
                          
                LOAD    R7, M[R6]
                INC     R6
                
                JMP     R7

;=================================================================
; PROCESS_CHAR: processa caracteres na janela de texto
;-----------------------------------------------------------------
PROCESS_CHAR:   DEC     R6
                STOR    M[R6], R7
                
                MVI     R1, TERM_READ     ;lê o caracter do terminal
                LOAD    R2, M[R1]
                
                MVI     R1, ' '
                CMP     R2, R1
                BR.Z    salta
                MVI     R1, '0'
                CMP     R2, R1
                BR.Z    comeca
                BR      retorna
                
salta:          MVI     R2, SALTO         ;se for espaço atualiza a variável SALTO (inicia um salto)
                MVI     R1, 1
                STOR    M[R2], R1
                BR      retorna

comeca:         MVI     R2, INICIA_JOGO   ;se for '0' atualiza a variável COMECA_JOGO (começa o jogo)
                MVI     R1, 1
                STOR    M[R2], R1
                
retorna:        LOAD    R7, M[R6]
                INC     R6
                
                JMP     R7

;=================================================================
; conta: atualiza o valor da pontuação e escreve no display
;-----------------------------------------------------------------
conta:          DEC     R6 
                STOR    M[R6],R7            ;guarda R7 na pilha

                MVI     R1, DISP7_NUMS      ;prepara para atualizar o display e a pontuação
                LOAD    R1, M[R1]
                
                INC     R1

                JAL     WRITE_ON_DISP7
                
                LOAD    R7, M[R6]
                INC     R6
                
                JMP     R7

WRITE_ON_DISP7:  ;escreve a pontuacao atualizada no display
                DEC     R6                  
                STOR    M[R6],R7            ;guarda R7 na pilha
                DEC     R6
                STOR    M[R6],R4            ;guarda R4 na pilha
                DEC     R6
                STOR    M[R6],R5            ;guarda R5 na pilha 
                
                MVI     R3, DISP7_NUMS 
                MVI     R4, DISP7_ADDR
				
                MVI     R5,10
escrevetodos:   ;insere valor no dígito Dx
                CMP     R1, R5             ;verifica se um dos digitos é maior ou igual a 10
                BR.NN   proximodigito      ;se for chama proximodigito

                LOAD    R2, M[R4]
                STOR    M[R2],R1
                LOAD    R2, M[R3]
                STOR    M[R3],R1
                
                LOAD    R5, M[R6]           ;retira R5 da pilha                
                INC     R6
                LOAD    R4, M[R6]           ;retira R4 da pilha 
                INC     R6
                LOAD    R7, M[R6]           ;retira R7 da pilha
                INC     R6
                
                JMP     R7
                
                ;vai para o próximo dígito
proximodigito:  MVI     R1, 0              ;coloca esse display a 0 e acrescenta 1 ao próximo
                LOAD    R2, M[R4]
                STOR    M[R2],R1
                LOAD    R2, M[R3]
                STOR    M[R3],R1

                INC     R3
                INC     R4
                
                LOAD    R2, M[R3]
                
                MOV     R1, R2
                INC     R1 
                 
                BR      escrevetodos      ;repete para todos os elementos

;=================================================================
; atualizajogo: atualiza o terreno de jogo e o dinossauro. Recebe
; a tabela JOGO (R1) e a dimensão (R2). Não devolve nada
;-----------------------------------------------------------------
atualizajogo:   DEC     R6
                STOR    M[R6], R7      ;guarda R7 na pilha
                DEC     R6
                STOR    M[R6], R4      ;guarda R4 na pilha
                DEC     R6
                STOR    M[R6], R5      ;guarda R5 na pilha
                
                MOV     R4, R1
                
                MOV     R5, R2         ;inicializa R5 a 1 para ajudar a percorrer a tabela JOGO 
                DEC     R5
                                
                MVI     R1, POS_TERRENO

ciclo:          INC     R4            
                LOAD    R2, M[R4]      ;coloca em R4 o valor da coluna n + 1
                
                DEC     R6
                STOR    M[R6], R1
                DEC     R6
                STOR    M[R6], R2
                
                CMP     R2, R0
                JAL.NZ  atualizacacto  ;se existir cacto atualiza a sua posição no terminal

                LOAD    R2, M[R6]
                INC     R6
                LOAD    R1, M[R6]
                INC     R6
                
                DEC     R4            
                STOR    M[R4], R2     ;coloca na coluna n o valor da coluna n + 1 
                INC     R1
                INC     R4            
                DEC     R5          
                CMP     R5, R0        ;compara R5 com a dimensão da tabela
                BR.NZ   ciclo         ;enquanto não for igual volta a executar o ciclo
                                      ;quando chega ao fim prepara-se para chamar a função geracacto
                DEC     R6
                STOR    M[R6], R1     ;guarda na pilha o valor de R1 que vai ser usado como argumento para geracacto
                MVI     R1, altura    ;coloca em R1 a altura
                JAL     geracacto     ;salta para função geracacto
                LOAD    R1, M[R6]
                INC     R6            ;R1 volta a ficar com o endereço da ultima coluna da tabela
                STOR    M[R4], R3     ;nessa coluna coloca o valor de retorno de geracacto
                
                MOV     R2, R3
                CMP     R2, R0
                JAL.NZ  imprimecacto  ;se for diferente de 0 imprime o cacto na última posição
                
                MVI     R2, SALTO
                LOAD    R2, M[R2]
                CMP     R2, R0
                JAL.NZ  atualizasalto ;se houver um salto a decorrer atualiza o salto
                
                LOAD    R5, M[R6]     ;retira R5 da pilha
                INC     R6
                LOAD    R4, M[R6]     ;retira R4 da pilha
                INC     R6
                LOAD    R7, M[R6]     ;retira R7 da pilha
                INC     R6
                
                JMP     R7            ;volta à função principal

;=================================================================
; geracacto: gera um número aleatório entre 0 e altura. Recebe a 
; altura (R1). Devolve o número gerado
;-----------------------------------------------------------------
geracacto:      DEC     R6
                STOR    M[R6], R7     ;guarda R7 na pilha
                DEC     R6
                STOR    M[R6], R4     ;guarda R4 na pilha
                DEC     R6
                STOR    M[R6], R5     ;guarda R5 na pilha

                MVI     R5, x         
                LOAD    R5, M[R5]     ;coloca em R5 o valor de x
                MVI     R4, 1
                
                AND     R4, R5, R4    ;bit = x & 1
                SHR     R5            ;x = x >> 1
                CMP     R4, R0        ;if bit
                JAL.P   alterabit     ;salta para a função alterabit
                
                MVI     R4, x         ;atualiza o valor de x
                STOR    M[R4], R5     
                
                MVI     R4, 62258     
                CMP     R5, R4        ;if x < 62258
                BR.C    semcacto      ;salta para a função semcacto
                
                DEC     R1
                AND     R3, R1, R5    ;x & (altura - 1)
                INC     R3            ;devolve x & (altura - 1)) + 1
                
                LOAD    R5, M[R6]     ;retira R5 da pilha
                INC     R6
                LOAD    R4, M[R6]     ;retira R4 da pilha
                INC     R6
                LOAD    R7, M[R6]     ;retira R7 da pilha
                INC     R6
                
                JMP     R7            ;volta à função atualizajogo
 
                
alterabit:      MVI     R4, b400h
                XOR     R5, R5, R4    ;x = XOR(x, 0xb400)

                JMP     R7            ;volta à função geracacto


semcacto:       LOAD    R5, M[R6]     ;retira R5 da pilha
                INC     R6
                LOAD    R4, M[R6]     ;retira R4 da pilha
                INC     R6
                LOAD    R7, M[R6]     ;retira R7 da pilha
                INC     R6
                
                MVI     R3, 0         ;devolve 0 (não tem cacto)
                
                JMP     R7            ;volta a função atualizajogo

;=================================================================
; imprimeterreno: imprime o terreno de jogo no terminal. Não  
; recebe nem devolve nada.
;-----------------------------------------------------------------
imprimeterreno: DEC     R6
                STOR    M[R6], R7         ;guarda R7 na pilha

                MVI     R1, TERM_COLOR
                MVI 	R2, AMARELO       ;põe a cor a amarelo
                STOR 	M[R1], R2

                MVI 	R1, POS_TERRENO
                MVI     R2, terrenocima   ;parte de cima do terreno
                                
                DEC     R6
                STOR    M[R6], R1         ;guarda R1 na pilha
                JAL     imprimelinha
                LOAD    R1, M[R6]         ;retira R1 da pilha
                INC     R6
                
                MVI     R2, NEXT_LINE
                ADD     R1, R1, R2        ;linha abaixo
                MVI     R2, terrenobaixo  ;parte de baixo do terreno
                
                JAL     imprimelinha
                               
                LOAD    R7, M[R6]         ;retira R7 da pilha
                INC     R6
                
                JMP     R7

;=================================================================
; imprimenuvens: imprime as nuvens no terminal. Não recebe nem 
; devolve nada.
;-----------------------------------------------------------------
imprimenuvens:  DEC     R6
                STOR    M[R6], R7         ;guarda R7 na pilha

                MVI     R1, TERM_COLOR
                MVI 	R2, AZUL          ;põe a cor a azul
                STOR 	M[R1], R2

                MVI     R1, POS_NUVEM
                MVI     R2, nuvem1        ;1º parte das nuvens
                
                DEC     R6
                STOR    M[R6], R1         ;guarda R1 na pilha
                JAL     imprimelinha
                LOAD    R1, M[R6]         ;retira R1 da pilha
                INC     R6               
                
                MVI     R2, NEXT_LINE
                ADD     R1, R1, R2
                MVI     R2, nuvem2        ;2º parte das nuvens
                
                DEC     R6
                STOR    M[R6], R1         ;guarda R1 na pilha
                JAL     imprimelinha
                LOAD    R1, M[R6]         ;retira R1 da pilha
                INC     R6
                
                MVI     R2, NEXT_LINE
                ADD     R1, R1, R2
                MVI     R2, nuvem3        ;3º parte das nuvens

                DEC     R6
                STOR    M[R6], R1         ;guarda R1 na pilha
                JAL     imprimelinha
                LOAD    R1, M[R6]         ;retira R1 da pilha
                INC     R6
                
                MVI     R2, NEXT_LINE
                ADD     R1, R1, R2
                MVI     R2, nuvem4       ;4º parte das nuvens

                DEC     R6
                STOR    M[R6], R1        ;guarda R1 na pilha
                JAL     imprimelinha
                LOAD    R1, M[R6]        ;retira R1 da pilha
                INC     R6
                
                MVI     R2, NEXT_LINE
                ADD     R1, R1, R2
                MVI     R2, nuvem5       ;5º parte das nuvens

                DEC     R6
                STOR    M[R6], R1        ;guarda R1 na pilha
                JAL     imprimelinha
                LOAD    R1, M[R6]        ;retira R1 da pilha
                INC     R6
                
                MVI     R2, NEXT_LINE
                ADD     R1, R1, R2
                MVI     R2, nuvem6       ;6º parte das nuvens

                DEC     R6
                STOR    M[R6], R1        ;guarda R1 na pilha
                JAL     imprimelinha
                LOAD    R1, M[R6]        ;retira R1 da pilha
                INC     R6
                
                MVI     R2, NEXT_LINE
                ADD     R1, R1, R2
                MVI     R2, nuvem7       ;7º parte das nuvens
                
                JAL     imprimelinha
                
                LOAD    R7, M[R6]        ;retira R7 da pilha
                INC     R6
                
                JMP     R7

;=================================================================
; imprimedinossauro: imprime o dinossauro no terminal. Recebe a
; sua posição (R1). Não devolve nada. 
;----------------------------------------------------------------- 
imprimedino:    DEC     R6
                STOR    M[R6], R7         ;guarda R7 na pilha
                DEC     R6
                STOR    M[R6], R4         ;guarda R4 na pilha
                
                MVI     R4, TERM_COLOR
                MVI 	R2, ROXO          ;põe a cor a roxo
                STOR 	M[R4], R2
                                              
                MVI     R2, dino5         ;5º parte do dinossauro
                
                DEC     R6
                STOR    M[R6], R1         ;guarda R1 na pilha
                JAL     imprimelinha
                LOAD    R1, M[R6]         ;retira R1 da pilha
                INC     R6 
                              
                MVI     R2, NEXT_LINE
                SUB     R1, R1, R2
                MVI     R2, dino4         ;4º parte do dinossauro  
                
                DEC     R6
                STOR    M[R6], R1         ;guarda R1 na pilha
                JAL     imprimelinha
                LOAD    R1, M[R6]         ;retira R1 da pilha
                INC     R6
                
                MVI     R2, NEXT_LINE
                SUB     R1, R1, R2
                MVI     R2, dino3         ;3º parte do dinossauro  
                        
                DEC     R6
                STOR    M[R6], R1         ;guarda R1 na pilha
                JAL     imprimelinha
                LOAD    R1, M[R6]         ;retira R1 da pilha
                INC     R6
                
                MVI     R2, NEXT_LINE
                SUB     R1, R1, R2
                
                MVI     R4, OLHO_DINO
                LOAD    R4, M[R4]
                CMP     R4, R0
                BR.NZ   dinomorto         ;vê que olho do dinossauro vai imprimir
                
dinovivo:       MVI     R2, dino2         ;2º parte do dinossauro (vivo)
                BR      imprimeolho

dinomorto:      MVI     R2, dino6         ;2º parte do dinossauro (morto)
                
imprimeolho:    DEC     R6
                STOR    M[R6], R1         ;guarda R1 na pilha
                JAL     imprimelinha
                LOAD    R1, M[R6]         ;retira R1 da pilha
                INC     R6
                
                MVI     R2, NEXT_LINE
                SUB     R1, R1, R2
                MVI     R2, dino1         ;1º parte do dinossauro
                
                JAL     imprimelinha
                
                LOAD    R4, M[R6]         ;retira R7 da pilha
                INC     R6
                LOAD    R7, M[R6]         ;retira R7 da pilha
                INC     R6
                
                JMP     R7

;=================================================================
; apagadino: apaga o dinossauro do terminal. Recebe a posição (R1).
; Não devolve nada.  
;----------------------------------------------------------------- 
apagadino:      DEC     R6
                STOR    M[R6], R7     ;guarda R7 na pilha
            
                MVI     R2, dino5     ;5º parte do dinossauro
                
                DEC     R6
                STOR    M[R6], R1     ;guarda R1 na pilha
                JAL     apagalinha
                LOAD    R1, M[R6]     ;retira R1 da pilha
                INC     R6
                               
                MVI     R2, NEXT_LINE
                SUB     R1, R1, R2
                MVI     R2, dino4     ;4º parte do dinossauro
                
                DEC     R6
                STOR    M[R6], R1     ;guarda R1 na pilha
                JAL     apagalinha
                LOAD    R1, M[R6]     ;retira R1 da pilha
                INC     R6
                
                MVI     R2, NEXT_LINE
                SUB     R1, R1, R2
                MVI     R2, dino3     ;3º parte do dinossauro
                
                DEC     R6
                STOR    M[R6], R1     ;guarda R1 na pilha
                JAL     apagalinha
                LOAD    R1, M[R6]     ;retira R1 da pilha
                INC     R6
                
                MVI     R2, NEXT_LINE
                SUB     R1, R1, R2
                MVI     R2, dino2     ;2º parte do dinossauro
                
                DEC     R6
                STOR    M[R6], R1     ;guarda R1 na pilha
                JAL     apagalinha
                LOAD    R1, M[R6]     ;retira R1 da pilha
                INC     R6
                
                MVI     R2, NEXT_LINE
                SUB     R1, R1, R2
                MVI     R2, dino1     ;1º parte do dinossauro 
                
                JAL     apagalinha
                
                LOAD    R7, M[R6]     ;retira R7 da pilha
                INC     R6
                
                JMP     R7

;=================================================================
; atualizasalto: atualiza o salto do dinossauro. Não recebe nem
; devolve nada.
;----------------------------------------------------------------- 
atualizasalto:  DEC     R6
                STOR    M[R6], R7            ;guarda R7 na pilha
                DEC     R6
                STOR    M[R6], R4            ;guarda R4 na pilha
                DEC     R6
                STOR    M[R6], R5            ;guarda R5 na pilha
               
                MVI     R1, POS_DINO_ATUAL   ;coloca em R1 a posição atual do dinossauro
                LOAD    R1, M[R1]
                
                DEC     R6
                STOR    M[R6], R1            ;guarda R1 na pilha

                JAL     apagadino            ;apaga dinossauro da posição anterior
                
                LOAD    R1, M[R6]            ;retira R1 da pilha
                INC     R6

                MVI     R5, NEXT_LINE
                MVI     R2, DIRECAO_SALTO    ;coloca em R2 a direção do salto 
                LOAD    R2, M[R2]

                CMP     R2, R0
                BR.NZ   desce

sobe:           SUB     R1, R1, R5           ;se estiver a subir muda a posição 
                SUB     R1, R1, R5           ;4linhas para cima
                SUB     R1, R1, R5
                SUB     R1, R1, R5
                
                MVI     R4, EXTREMOS_DINO    ;põe em R4 a cadeia de caracteres com os extremos do dinossauro   
                MVI     R2, 5                ;número de extremos
atualizaextremo:
                LOAD    R3, M[R4]
                SUB     R3, R3, R5
                SUB     R3, R3, R5
                SUB     R3, R3, R5
                SUB     R3, R3, R5
                
                STOR    M[R4], R3            ;guarda o novo valor
                INC     R4
                DEC     R2
                CMP     R2, R0
                BR.NZ   atualizaextremo      ;atualiza cada extremo no caso da subida           
                BR      atualizadino

desce:          ADD     R1, R1, R5           ;se estiver a descer muda a posição 
                ADD     R1, R1, R5           ;4 linhas para baixo
                ADD     R1, R1, R5
                ADD     R1, R1, R5
                
                MVI     R4, EXTREMOS_DINO    ;põe em R4 a cadeia de caracteres com os extremos do dinossauro   
                MVI     R2, 5                ;número de extremos 
atualizaextremo2:
                LOAD    R3, M[R4]
                ADD     R3, R3, R5
                ADD     R3, R3, R5
                ADD     R3, R3, R5
                ADD     R3, R3, R5
                
                STOR    M[R4], R3            ;guarda o novo valor
                INC     R4
                DEC     R2
                CMP     R2, R0
                BR.NZ   atualizaextremo2     ;atualiza cada extremo no caso da descida

atualizadino:   DEC     R6
                STOR    M[R6], R1            ;guarda R1 na pilha

                JAL     imprimedino          ;imprime dinossauro na nova posição

                LOAD    R1, M[R6]            ;retira R1 da pilha
                INC     R6

                MVI     R2, POS_DINO_ATUAL
                STOR    M[R2], R1            ;atualiza a variável que guarda a sua posição atual
                          
                MVI     R2, SALTO_MAX
                CMP     R1, R2
                BR.Z    mudadirecao          ;compara a posição atual com a altura máxima do salto   
                BR      mantemdirecao
                
mudadirecao:    MVI     R2, DIRECAO_SALTO    ;se for igual muda a direção do salto
                MVI     R4, 1                
                STOR    M[R2], R4
                
mantemdirecao:  MVI     R2, POS_DINO_INI
                CMP     R1, R2
                BR.Z    terminasalto         ;compara a posição atual com a posição inicial   
                BR      naotermina
                
terminasalto:   MVI     R2, DIRECAO_SALTO    ;se for igual o salto terminou
                STOR    M[R2], R0
                MVI     R2, SALTO
                STOR    M[R2], R0

naotermina:     

                LOAD    R5, M[R6]            ;retira R5 da pilha
                INC     R6
                LOAD    R4, M[R6]            ;retira R4 da pilha
                INC     R6
                LOAD    R7, M[R6]            ;retira R7 da pilha
                INC     R6
                
                JMP     R7

;=================================================================
; imprimecacto: imprime um cacto no terminal. Recebe a sua posição
; (R1) e altura (R2). Não devolve nada. 
;----------------------------------------------------------------- 
imprimecacto:   DEC     R6
                STOR    M[R6], R7           ;guarda R7 na pilha
                DEC     R6
                STOR    M[R6], R4           ;guarda R4 na pilha
                DEC     R6
                STOR    M[R6], R5           ;guarda R5 na pilha

                MVI     R4, TERM_COLOR
                MVI 	R5, VERDE           ;põe a cor a verde
                STOR 	M[R4], R5
                MOV     R4, R2   
                
imprimealtura:  DEC     R6
                STOR    M[R6], R4

                MVI     R4, 5
                MVI     R5, EXTREMOS_DINO

verificacolisao:                            ;antes de imprimir o cacto na nova posição
                LOAD    R3, M[R5]           ;compara se colide com algum extremo do dinossauro
                CMP     R1, R3
                JAL.Z   gameover            ;se sim termina o jogo
                INC     R5
                DEC     R4
                CMP     R4, R0
                BR.NZ   verificacolisao
                
                LOAD    R4, M[R6]
                INC     R6
                          
                MVI     R2, cacto
                
                JAL     imprimecaracter     ;se não imprime o caracter
                
                MVI     R5, NEXT_LINE
                SUB     R1, R1, R5

                DEC     R4
                CMP     R4, R0
                BR.NZ   imprimealtura       ;repete o ciclo para todos os caracteres até chegar à altura
                               
                LOAD    R5, M[R6]           ;retira R5 da pilha
                INC     R6
                LOAD    R4, M[R6]           ;retira R4 da pilha
                INC     R6
                LOAD    R7, M[R6]           ;retira R7 da pilha
                INC     R6
                
                JMP     R7

;=================================================================
; apagacacto: apaga um cacto do terminal. Recebe a sua posição
; (R1) e altura (R2). Não devolve nada.  
;----------------------------------------------------------------- 
apagacacto:     DEC     R6
                STOR    M[R6], R7           ;guarda R7 na pilha
                DEC     R6
                STOR    M[R6], R4           ;guarda R4 na pilha
                DEC     R6
                STOR    M[R6], R5           ;guarda R5 na pilha
                
                MVI     R5, NEXT_LINE
                MOV     R4, R2
                
                DEC     R6
                STOR    M[R6], R1

apagaaltura:    JAL     apagacaracter      ;apaga o caracter
                
                SUB     R1, R1, R5
                DEC     R4
                CMP     R4, R0
                BR.NZ   apagaaltura        ;repete o ciclo para todos os caracteres até chegar à altura
                
                LOAD    R1, M[R6]
                INC     R6
                
                MVI     R4, TERM_COLOR
                MVI 	R5, AMARELO        ;põe a cor a amarelo      
                STOR 	M[R4], R5
                
                MOV     R5, R1
                MVI 	R1, POS_TERRENO
                MVI     R2, terrenocima                
                
procuraterreno: INC     R1                 ;procura a posição do terreno que foi apagada ao apagar o cacto
                INC     R2
                CMP     R1, R5
                BR.NZ   procuraterreno
                JAL     imprimecaracter    ;volta a imprimir esse caracter  
                            
                LOAD    R5, M[R6]          ;retira R5 da pilha
                INC     R6
                LOAD    R4, M[R6]          ;retira R4 da pilha
                INC     R6
                LOAD    R7, M[R6]          ;retira R7 da pilha
                INC     R6
                
                JMP     R7

;=================================================================
; atualizacacto: atualiza a posição de um cacto. Recebe a sua 
; posição (R1) e altura (R2). Não devolve nada.  
;----------------------------------------------------------------- 
atualizacacto:  DEC     R6
                STOR    M[R6], R7          ;guarda R7 na pilha
                DEC     R6
                STOR    M[R6], R4          ;guarda R4 na pilha
                DEC     R6
                STOR    M[R6], R5          ;guarda R5 na pilha
                
                DEC     R6
                STOR    M[R6], R1          ;guarda R1 na pilha
                DEC     R6
                STOR    M[R6], R2          ;guarda R2 na pilha
                
                MVI     R4, POS_TERRENO
                CMP     R1, R4             ;compara a nova posição do cacto com a posição mais à esquerda no terminal
                JAL.NZ  imprimecacto       ;se for igual não imprime o cacto

                LOAD    R2, M[R6]          ;retira R2 da pilha
                INC     R6
                LOAD    R1, M[R6]          ;retira R1 da pilha
                INC     R6
                
                INC     R1                 ;volta à posição anterior 
                
                JAL     apagacacto         ;apaga o cacto  
                
                LOAD    R5, M[R6]          ;retira R5 da pilha
                INC     R6
                LOAD    R4, M[R6]          ;retira R4 da pilha
                INC     R6
                LOAD    R7, M[R6]          ;retira R7 da pilha
                INC     R6
                
                JMP     R7

;=================================================================
; imprimegameover: imprime as palavras game over no terminal. Não
; recebe nem devolve nada.
;-----------------------------------------------------------------
imprimegameover:DEC     R6
                STOR    M[R6], R7          ;guarda R7 na pilha

                MVI     R1, TERM_COLOR
                MVI 	R2, VERMELHO       ;põe a cor a vermelho
                STOR 	M[R1], R2

                MVI 	R1, POS_GAMEOVER   
                MVI     R2, gameover1      ;1º parte de game over
                                
                DEC     R6
                STOR    M[R6], R1          ;guarda R1 na pilha
                JAL     imprimelinha
                LOAD    R1, M[R6]          ;retira R1 da pilha
                INC     R6
                
                MVI     R2, NEXT_LINE
                ADD     R1, R1, R2
                MVI     R2, gameover2      ;2º parte de game over
                
                DEC     R6
                STOR    M[R6], R1          ;guarda R1 na pilha
                JAL     imprimelinha
                LOAD    R1, M[R6]          ;retira R1 da pilha
                INC     R6
                                
                MVI     R2, NEXT_LINE
                ADD     R1, R1, R2
                MVI     R2, gameover3      ;3º parte de game over
                         
                JAL     imprimelinha
                               
                LOAD    R7, M[R6]          ;retira R7 da pilha
                INC     R6
                
                JMP     R7

;=================================================================
; gameover: termina o jogo. Não recebe nem devolve nada.
;----------------------------------------------------------------- 
gameover:       DSI
                MVI     R1,INT_MASK
                MVI     R2,INT_MASK_VAL
                STOR    M[R1],R2
                ENI
                
                MVI     R1, OLHO_DINO
                MVI     R2, 1
                STOR    M[R1], R2

                MVI     R1, POS_DINO_ATUAL
                LOAD    R1, M[R1]
                JAL     imprimedino

                JAL     imprimegameover

                MVI     R1, PRIMEIRO_JOGO
                MVI     R2, 1
                STOR    M[R1], R2
                
                MVI     R2,INICIA_JOGO
                STOR    M[R2], R0

                MVI     R1, OLHO_DINO
                MVI     R2, 0
                STOR    M[R1], R2
                
                JAL     espera
                
;=================================================================
; limpatudo: limpa tudo do jogo antigo. Não recebe nem
; devolve nada.
;----------------------------------------------------------------- 
limpatudo:      DEC     R6
                STOR    M[R6], R7       ;guarda R7 na pilha
                DEC     R6
                STOR    M[R6], R4       ;guarda R4 na pilha
                DEC     R6
                STOR    M[R6], R5       ;guarda R5 na pilha
                
                MVI     R1, 0000h
                MVI     R5, 2C00h       ;limpa todas as linhas no terminal
limpaterminal:  MVI     R2, vazia       ;cadeia de caracteres vazia auxiliar 
                MVI     R4, NEXT_LINE
                DEC     R6
                STOR    M[R6], R1
                JAL     imprimelinha
                LOAD    R1, M[R6]
                INC     R6
                ADD     R1, R1, R4
                CMP     R1, R5
                BR.NZ   limpaterminal
                
                MVI     R1, DISP7_NUMS   ;limpa o que está em memória do display de 7 segmentos
                MVI     R2, 6
                ADD     R2, R2, R1
limpamemoria:   DEC     R2
                STOR    M[R2], R0
                CMP     R2, R1
                BR.NZ   limpamemoria
                
                MVI     R1, DISP7_ADDR   ;põe a 0 todos os números no display de 7 segmentos 
                MVI     R2, 6
                ADD     R2, R2, R1
limpadisplay:   DEC     R2
                LOAD    R4, M[R2]
                STOR    M[R4], R0
                CMP     R2, R1
                BR.NZ   limpadisplay
                              
                MVI     R1, JOGO         ;põe a 0 todos os valores em memória da tab JOGO 
                MVI     R2, N
                ADD     R2, R1, R2
limpajogo:      DEC     R2
                STOR    M[R2], R0
                CMP     R2, R1
                BR.NZ   limpajogo
                
                LOAD    R5, M[R6]        ;retira R7 da pilha
                INC     R6
                LOAD    R4, M[R6]        ;retira R7 da pilha
                INC     R6
                LOAD    R7, M[R6]        ;retira R7 da pilha
                INC     R6
                
                JMP     R7
                
;=================================================================
; imprimelinha: imprime uma linha no terminal. Recebe a posição (R1)
; e a linha (R2). Não devolve nada.
;----------------------------------------------------------------- 
imprimelinha:   DEC     R6
                STOR    M[R6], R7     ;guarda R7 na pilha
                DEC     R6
                STOR    M[R6], R4     ;guarda R4 na pilha
                DEC     R6
                STOR    M[R6], R5     ;guarda R5 na pilha
               
                LOAD    R4, M[R2]                
                
imprime:        JAL     imprimecaracter
                INC     R1
                INC     R2
                LOAD    R4, M[R2]
                CMP     R4, R0
                BR.NZ   imprime       ;imprime cada caracter
                
                LOAD    R5, M[R6]     ;retira R5 da pilha
                INC     R6
                LOAD    R4, M[R6]     ;retira R4 da pilha
                INC     R6
                LOAD    R7, M[R6]     ;retira R7 da pilha
                INC     R6
                
                JMP     R7

;=================================================================
; apagalinha: apaga uma linha do terminal. Recebe a posição (R1)
; e a linha (R2). Não devolve nada.
;----------------------------------------------------------------- 
apagalinha:     DEC     R6
                STOR    M[R6], R7     ;guarda R7 na pilha
                DEC     R6
                STOR    M[R6], R4     ;guarda o valor original de R4 na pilha
                DEC     R6
                STOR    M[R6], R5     ;guarda o valor original de R5 na pilhA
                
                LOAD    R4, M[R2]
                               
apaga:          DEC     R6
                STOR    M[R6], R2     ;guarda R7 na pilha

                JAL     apagacaracter
                
                LOAD    R2, M[R6]     ;retira R5 da pilha
                INC     R6
                
                INC     R1
                INC     R2
                LOAD    R4, M[R2]
                CMP     R4, R0
                BR.NZ   apaga         ;apaga cada caracter
                
                LOAD    R5, M[R6]     ;retira R5 da pilha
                INC     R6
                LOAD    R4, M[R6]     ;retira R4 da pilha
                INC     R6
                LOAD    R7, M[R6]     ;retira R7 da pilha
                INC     R6
                
                JMP     R7

;=================================================================
; imprimecaracter: imprime um caracter no terminal. Recebe a posição 
; (R1) e o caracter (R2). Não devolve nada.  
;----------------------------------------------------------------- 
imprimecaracter:DEC     R6
                STOR    M[R6], R7     ;guarda R7 na pilha
                DEC     R6
                STOR    M[R6], R4     ;guarda o valor original de R4 na pilha
                DEC     R6
                STOR    M[R6], R5     ;guarda o valor original de R5 na pilha

                MVI     R4, TERM_CURSOR
                MVI     R5, TERM_WRITE
                STOR    M[R4], R1
                LOAD    R4, M[R2]
                STOR    M[R5], R4     ;escreve no terminal
                
                LOAD    R5, M[R6]     ;retira R5 da pilha
                INC     R6
                LOAD    R4, M[R6]     ;retira R4 da pilha
                INC     R6
                LOAD    R7, M[R6]     ;retira R7 da pilha
                INC     R6
                
                JMP     R7

;=================================================================
; apagacaracter: apaga um caracter do terminal. Recebe a posição 
; (R1). Não devolve nada.   
;----------------------------------------------------------------- 
apagacaracter:  DEC     R6
                STOR    M[R6], R7       ;guarda R7 na pilha
                DEC     R6
                STOR    M[R6], R4       ;guarda R4 na pilha
                DEC     R6
                STOR    M[R6], R5       ;guarda R5 na pilha

                MVI     R4, TERM_COLOR
                MVI 	R5, PRETO       ;põe a cor a preto
                STOR 	M[R4], R5

                MVI     R2, cacto       

                MVI     R4, TERM_CURSOR
                MVI     R5, TERM_WRITE
                STOR    M[R4], R1
                LOAD    R4, M[R2]
                STOR    M[R5], R4       ;escreve o caracter a preto no terminal 
                
                LOAD    R5, M[R6]       ;retira R5 da pilha
                INC     R6
                LOAD    R4, M[R6]       ;retira R4 da pilha
                INC     R6
                LOAD    R7, M[R6]       ;retira R7 da pilha
                INC     R6
                
                JMP     R7

;=================================================================
; rotina auxiliar para a interrupção
;----------------------------------------------------------------- 
AUX_TIMER_ISR:  DEC     R6
                STOR    M[R6],R1            ;guarda R1 na pilha
                DEC     R6
                STOR    M[R6],R2            ;guarda R2 na pilha
                ;restart do timer
                MVI     R1,TIMER_COUNTVAL
                LOAD    R2,M[R1]
                MVI     R1,TIMER_COUNTER
                STOR    M[R1],R2
                MVI     R1,TIMER_CONTROL
                MVI     R2,TIMER_SETSTART
                STOR    M[R1],R2            ;começa timer
                
                MVI     R2,TIMER_TICK       
                LOAD    R1,M[R2]
                INC     R1                  ;incrementa flag do timer
                STOR    M[R2],R1 
               
                LOAD    R2,M[R6]            ;retira R2 da pilha 
                INC     R6
                LOAD    R1,M[R6]            ;retira R1 da pilha
                INC     R6 

                JMP     R7
      
;=================================================================
; rotinas das interrupções
;----------------------------------------------------------------- 
                ORIG    7FF0h
TIMER_ISR:      
                DEC     R6
                STOR    M[R6],R7             ;guarda R7 na pilha
                
                JAL     AUX_TIMER_ISR        ;chama função auxiliar
                
                LOAD    R7,M[R6]             ;retira R7 da pilha 
                INC     R6 
                RTI
                

                ORIG    7F00h
KEYZERO:        
                DEC     R6
                STOR    M[R6],R7             ;guarda R7 na pilha
                                        
                MVI     R1, INICIA_JOGO
                MVI     R2, 1
                STOR    M[R1], R2            ;coloca a 1 a variável COMECA_JOGO (começa o jogo)  
                
                LOAD    R7,M[R6]             ;retira R7 da pilha 
                INC     R6                
                RTI


                ORIG    7F30h
KEYUP:         
                DEC     R6
                STOR    M[R6],R7             ;guarda R7 na pilha
                DEC     R6
                STOR    M[R6],R1             ;guarda R1 na pilha
                DEC     R6
                STOR    M[R6],R2             ;guarda R2 na pilha
                
                MVI     R1, SALTO
                MVI     R2, 1
                STOR    M[R1], R2            ;coloca a 1 a variável SALTO (inicia um salto)             
                
                LOAD    R2,M[R6]             ;retira R2 da pilha  
                INC     R6
                LOAD    R1,M[R6]             ;retira R1 da pilha 
                INC     R6
                LOAD    R7,M[R6]             ;retira R7 da pilha 
                INC     R6
                RTI


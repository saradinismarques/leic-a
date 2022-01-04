;===============================================================================
; Projeto de Introdução à Arquitetura de Computadores (Primeira Parte)
; Programa: Dino.as
;
; Grupo: 20
; Clara Pais, nº92622
; Sara Marques, nº93342
; 
; Data: 16/11/2020 				
;===============================================================================

                ORIG    2000h
N               EQU     80            ;dimensão do terreno de jogo
altura          EQU     4             ;altura máxima dos cactos
x               WORD    5             ;semente
                
                ORIG    0000h
JOGO            TAB     N             ;tabela em memória que representa o terreno de jogo


principal:      MVI     R1, JOGO      ;coloca em R1 o endereço de início da tabela JOGO
                MVI     R2, N         ;coloca em R2 a dimensão da tabela
                MVI     R6, 4000h     ;inicializa a pilha          
                        
                JAL     atualizajogo  ;salta para a função atualizajogo
                BR      principal     ;executa o programa em loop


atualizajogo:   DEC     R6
                STOR    M[R6], R7     ;guarda R7 na pilha
                DEC     R6
                STOR    M[R6], R4     ;guarda o valor original de R4 na pilha
                DEC     R6
                STOR    M[R6], R5     ;guarda o valor original de R5 na pilha
                
                MVI     R5, 1         ;inicializa R5 a 1 para ajudar a percorrer a tabela JOGO 
                
ciclo:          INC     R1            
                LOAD    R4, M[R1]     ;coloca em R4 o valor da coluna n + 1
                DEC     R1            
                STOR    M[R1], R4     ;coloca na coluna n o valor da coluna n + 1 
                INC     R1            
                INC     R5            
                CMP     R5, R2        ;compara R5 com a dimensão da tabela
                BR.NZ   ciclo         ;enquanto não for igual volta a executar o ciclo
                
                                      ;quando chega ao fim prepara-se para chamar a função geracacto
                MOV     R4, R1        ;coloca em R4 o valor de R1 que vai ser usado como argumento para geracacto
                MVI     R1, altura    ;coloca em R1 a altura
                JAL     geracacto     ;salta para função geracacto
                MOV     R1, R4        ;R1 volta a ficar com o endereço da ultima coluna da tabela
                STOR    M[R1], R3     ;nessa coluna coloca o valor de retorno de geracacto

                LOAD    R5, M[R6]     ;retira R5 da pilha
                INC     R6
                LOAD    R4, M[R6]     ;retira R4 da pilha
                INC     R6
                LOAD    R7, M[R6]     ;retira R7 da pilha
                INC     R6
                
                JMP     R7            ;volta à função principal


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
                
                

                
                
                
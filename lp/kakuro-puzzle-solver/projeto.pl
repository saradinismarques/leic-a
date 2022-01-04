% LP 20/21 - Projeto (Solucionador de puzzles Kakuro)
% Nome: Sara Marques
% Numero: 93342

:- [codigo_comum].



%================== Predicados para a inicializacao de puzzles ==================

%--------------------------------------------------------------------------------
% combinacoes_soma(N, Els, Soma, Combs), em que N e um inteiro, Els e uma lista 
% de inteiros, e Soma e um inteiro, significa que Combs e a lista ordenada cujos 
% elementos sao as combinacoes N a N, dos elementos de Els cuja soma e Soma.
%--------------------------------------------------------------------------------

combinacoes_soma(N, Els, Soma, Combs) :-
    bagof(Comb, combinacao(N, Els, Comb), Combs_Aux),
    include(soma_lista(Soma), Combs_Aux, Combs).


%--------------------------------------------------------------------------------
% Auxiliar: soma_lista(S, L), significa que Soma e o resultado de somar todos os
% elementos da lista Lista.
%--------------------------------------------------------------------------------

soma_lista(Soma, Lista) :-
    sum_list(Lista, Soma).


%--------------------------------------------------------------------------------
% permutacoes_soma(N, Els, Soma, Perms), em que N e um inteiro, Els e uma lista 
% de inteiros, e Soma e um inteiro, significa que Perms e a lista ordenada cujos 
% elementos sao as permutacoes das combinacoes N a N, dos elementos de Els cuja 
% soma e Soma.
%--------------------------------------------------------------------------------
 
permutacoes_soma(N, Els, Soma, Perms) :-
    combinacoes_soma(N, Els, Soma, Combs),
    maplist(encontra_permutacoes, Combs, Perms_Aux),
    append(Perms_Aux, Perms_Not_Sorted),
    sort(Perms_Not_Sorted, Perms).


%--------------------------------------------------------------------------------
% Auxiliar: encontra_permutacoes(Lista, El), significa que Lista_Perms e a lista
% de todas as permutacoes possiveis da lista Lista. 
%--------------------------------------------------------------------------------
    
encontra_permutacoes(Lista, Lista_Perms) :-
    setof(Perm, permutation(Lista, Perm), Lista_Perms).


%--------------------------------------------------------------------------------
% espaco_fila(Fila, Esp, H_V), em que Fila e uma fila (linha ou coluna) de um
% puzzle e H_V e um dos atomos h ou v, conforme se trate de uma fila horizontal 
% ou vertical, respectivamente, significa que Esp e um espaco de Fila. 
%--------------------------------------------------------------------------------

espaco_fila(Fila, Esp, H_V) :-
    espaco_fila_aux(Fila, [], Esp,  H_V, _).

espaco_fila_aux([], Aux, Esp, _, Soma) :- 
    length(Aux, Comp),
    Comp > 0,
    espaco(Esp, Soma, Aux).

espaco_fila_aux([P|R], Aux, Esp, H_V, Soma) :-
    not(is_list(P)), 
    append(Aux, [P], Aux_2), 
    espaco_fila_aux(R, Aux_2, Esp, H_V, Soma).
  
espaco_fila_aux([P|_], Aux, Esp, _, Soma) :-
    is_list(P),
    length(Aux, Comp),
    Comp > 0,
    espaco(Esp, Soma, Aux).

espaco_fila_aux([P|R], _, Esp, H_V, _) :-
    is_list(P),
    soma_h_v(H_V, P, Soma),
    espaco_fila_aux(R, [], Esp, H_V, Soma).


%--------------------------------------------------------------------------------
% Auxiliar: soma_h_v(H_V, Lista, Soma), significa que Soma e o primeiro ou segundo
% elemento da lista Lista dependendo de H_V.
%--------------------------------------------------------------------------------

soma_h_v(v, [Soma|_], Soma).
soma_h_v(h, [_, Soma|_], Soma).


%--------------------------------------------------------------------------------
% Auxiliar: espaco(Espaco, Soma, Lista), significa que Espaco e unificavel com
% espaco(Soma, Lista).
%--------------------------------------------------------------------------------

espaco(espaco(Soma, Lista), Soma, Lista).


%--------------------------------------------------------------------------------
% espacos_fila(H_V, Fila, Espacos), em que Fila e uma fila (linha ou coluna) de
% uma grelha e e H_V e um dos atomos h ou v, significa que Espacos e a lista de 
% todos os espacos de Fila, da esquerda para a direita.
%--------------------------------------------------------------------------------

espacos_fila(H_V, Fila, Espacos) :-
    bagof(Espaco, espaco_fila(Fila, Espaco, H_V), Espacos).
 
espacos_fila(_, _, []).


%--------------------------------------------------------------------------------
% espacos_puzzle(Puzzle, Espacos), em que Puzzle e um puzzle, significa que
% Espacos e a lista de espacos de Puzzle.
%--------------------------------------------------------------------------------

espacos_puzzle(Puzzle, Espacos) :-
    maplist(espacos_fila(h), Puzzle, Espacos_H),
    mat_transposta(Puzzle, Puzzle_Transposta),
    maplist(espacos_fila(v), Puzzle_Transposta, Espacos_V),
    append(Espacos_H, Espacos_V, Espacos_H_V),
    exclude(=([]), Espacos_H_V, Espacos_Not_Empty),
    flatten(Espacos_Not_Empty, Espacos).


%--------------------------------------------------------------------------------
% Auxiliar: membro_lista(Lista, El), significa que El e elemento da lista Lista.
% --------------------------------------------------------------------------------

membro_lista([P|_], El) :-
    El == P.

membro_lista([_|R], El) :-
    membro_lista(R, El).


%--------------------------------------------------------------------------------
% espacos_com_posicoes_comuns(Espacos, Esp, Esps_com), em que Espacos e uma lista 
% de espacos e Esp e um espaco, significa que Esps_com e a lista de espacos com 
% variaveis em comum com Esp, exceptuando Esp. Os espacos em Esps_com devem
% aparecer pela mesma ordem que aparecem em Espacos.
%--------------------------------------------------------------------------------

espacos_com_posicoes_comuns(Espacos, Esp, Esps_com) :-
    include(espaco_comum(Esp), Espacos, Esps_com).


%--------------------------------------------------------------------------------
% Auxiliar: espaco_comum(Esp, Espaco), significa que Esp e Espaco tem elementos
% comuns.
%--------------------------------------------------------------------------------

espaco_comum(Esp, Espaco) :-
    Esp \== Espaco,
    espaco(Esp, _, L_Esp),
    espaco(Espaco, _, L_Espaco),
    include(membro_lista(L_Esp), L_Espaco, Esp_com),
    length(Esp_com, Comp),
    Comp > 0.


%--------------------------------------------------------------------------------
% permutacoes_soma_espacos(Espacos, Perms_soma), em que Espacos e uma lista de 
% espacos, significa que Perms_soma e a lista de listas de 2 elementos, em que o 
% 1o elemento e um espaco de Espacos e o 2o e a lista ordenada de permutacoes 
% cuja soma e igual a soma do espaco.
%--------------------------------------------------------------------------------

permutacoes_soma_espacos(Espacos, Perms_soma) :-
    maplist(permutacoes_soma_espacos_aux, Espacos, Perms_soma).

permutacoes_soma_espacos_aux(Espaco, Perms_soma) :-
    espaco(Espaco, Soma, L_Espaco),
    length(L_Espaco, Comp),
    findall(N, between(1, 9, N), Numbers),
    permutacoes_soma(Comp, Numbers, Soma, Perms),
    append([Espaco], [Perms], Perms_soma).
    

%--------------------------------------------------------------------------------
% permutacao_possivel_espaco(Perm, Esp, Espacos, Perms_soma), em que Perm e uma 
% permutacao, Esp e um espaco, Espacos e uma lista de espacos, e Perms_soma e uma 
% lista de listas tal como obtida pelo predicado anterior, significa que Perm e 
% uma permutacao possivel para o espaco Esp.
%--------------------------------------------------------------------------------

permutacao_possivel_espaco(Perm, Esp, Espacos, Perms_soma) :-
    member([Esp_Aux|[L_Perms]], Perms_soma),
    Esp_Aux == Esp,
    espacos_com_posicoes_comuns(Espacos, Esp, Espacos_comuns),
    member(Perm, L_Perms),
    experimenta_perm_aux(Perm, Esp, Perm, Espacos_comuns, Perms_soma).


%--------------------------------------------------------------------------------
% Auxiliar: experimenta_perm_aux(Perm, Esp, Perm_Aux, Espacos_comuns, Perms_soma), 
% significa que Perm e uma permutacao do espaco Esp que e possivel para os restantes
% espacos que tenham posicoes em comum com Esp (Espacos_comuns).
%--------------------------------------------------------------------------------

experimenta_perm_aux(Perm, Esp, Perm_Aux, Espacos_comuns, Perms_soma) :-
    forall(member(Esp_sub, Espacos_comuns), not(nao_verifica_perm(Esp_sub, Perms_soma, Esp, Perm_Aux))), 
    Perm = Perm_Aux.


%--------------------------------------------------------------------------------
% Auxiliar: nao_verifica_perm(Espaco, Perms_soma, Esp, Perm_Aux), significa que 
% nenhuma permutacao do espaco Espaco e possivel ao unificar o espaco Esp com a 
% permutacao Perm_Aux.
%--------------------------------------------------------------------------------

nao_verifica_perm(Espaco, Perms_soma, Esp, Perm_Aux) :-
    member([Espaco_Aux|[L_Perms]], Perms_soma), 
    Espaco_Aux == Espaco,
    espaco(Espaco, _, L_Espaco),
    espaco(Esp, _, L_Esp),
    L_Esp = Perm_Aux,
    forall(member(Perm, L_Perms), not(subsumes_term(L_Espaco, Perm))).


%--------------------------------------------------------------------------------
% permutacoes_possiveis_espaco(Espacos, Perms_soma, Esp, Perms_poss), em que 
% Espacos e uma lista de espacos, Perms_soma e uma lista de listas tal como obtida 
% pelo predicado permutacoes_soma_espacos, e Esp e um espaco, significa que 
% Perms_poss e uma lista de 2 elementos em que o primeiro e a lista de variaveis 
% de Esp e o segundo e a lista ordenada de permutacoes possiveis para o espaco Esp.
%--------------------------------------------------------------------------------

permutacoes_possiveis_espaco(Espacos, Perms_soma, Esp, Perms_poss) :-
    bagof(Perm, permutacao_possivel_espaco(Perm, Esp, Espacos, Perms_soma), Perms),
    espaco(Esp, _, L_Esp),
    append([L_Esp], [Perms], Perms_poss).


%--------------------------------------------------------------------------------
% permutacoes_possiveis_espacos(Espacos, Perms_poss_esps), em que Espacos e uma 
% lista de espacos, significa que Perms_poss_esps e a lista de permutacoes
% possiveis.
%--------------------------------------------------------------------------------

permutacoes_possiveis_espacos(Espacos, Perms_poss_esps) :-
    permutacoes_soma_espacos(Espacos, Perms_soma),
    maplist(permutacoes_possiveis_espaco(Espacos, Perms_soma), Espacos, Perms_poss_esps).


%--------------------------------------------------------------------------------
% numeros_comuns(Lst_Perms, Numeros_comuns), em que Lst_Perms e uma lista de 
% permutacoes, significa que Numeros_comuns e uma lista de pares (pos, numero),
% significando que todas as listas de Lst_Perms contem o numero numero na posicao
% pos.
%--------------------------------------------------------------------------------

numeros_comuns(Lst_Perms, Numeros_comuns) :-
    numeros_comuns_aux(Lst_Perms, 1, Numeros_comuns, [], [], Lst_Perms), !.

numeros_comuns_aux([P|_], Pos, Numeros_comuns, _, Numeros_comuns, _) :-
    length(P, Comp),
    Pos > Comp, !.

numeros_comuns_aux([], Pos, Numeros_comuns, Numeros, Aux, Lst_Perms) :-
    sort(Numeros, [_]), !,
    nth1(1, Numeros, Numero), 
    par(Par, Pos, Numero),
    append(Aux, [Par], Aux_2),
    New_pos is Pos + 1,
    numeros_comuns_aux(Lst_Perms, New_pos, Numeros_comuns, [], Aux_2, Lst_Perms).

numeros_comuns_aux([], Pos, Numeros_comuns, _, Aux, Lst_Perms) :-
    New_pos is Pos + 1,
    numeros_comuns_aux(Lst_Perms, New_pos, Numeros_comuns, [], Aux, Lst_Perms).

numeros_comuns_aux([P|R], Pos, Numeros_comuns, Numeros, Aux, Lst_Perms) :-
    nth1(Pos, P, El),
    append(Numeros, [El], Numeros_2),
    numeros_comuns_aux(R, Pos, Numeros_comuns, Numeros_2, Aux, Lst_Perms).


%--------------------------------------------------------------------------------
% Auxiliar: par(Par, Pos, Numero), significa que Par e unificavel com 
% par(Pos, Numero).
%--------------------------------------------------------------------------------

par((Pos,Numero), Pos, Numero).
    

%--------------------------------------------------------------------------------
% atribui_comuns(Perms_Possiveis), em que Perms_Possiveis e uma lista de
% permutacoes possiveis, actualiza esta lista atribuindo a cada espaco numeros 
% comuns a todas as permutacoes possiveis para esse espaco.
%--------------------------------------------------------------------------------

atribui_comuns([]).

atribui_comuns([[Esp|[Perms]]|R]) :-
    numeros_comuns(Perms, L_Comuns), 
    coloca_comuns(Esp, L_Comuns),
    atribui_comuns(R).


%--------------------------------------------------------------------------------
% Auxiliar: coloca_comuns(Esp, Lista), significa que Esp e unificado com cada 
% par(Pos, Numero) da lista Lista, em que Pos e a posicao de Esp onde Numero vai
% ficar.
%--------------------------------------------------------------------------------

coloca_comuns(_, []).

coloca_comuns(Esp, [P|R]) :-
    par(P, Pos, Numero),
    nth1(Pos, Esp, Numero), !,
    coloca_comuns(Esp, R).


%--------------------------------------------------------------------------------
% retira_impossiveis(Perms_Possiveis, Novas_Perms_Possiveis), em que 
% Perms_Possiveis e uma lista de permutacoes possiveis, significa que
% Novas_Perms_Possiveis e o resultado de tirar permutacoes impossiveis de
% Perms_Possiveis.
%--------------------------------------------------------------------------------
    
retira_impossiveis(Perms_Possiveis, Novas_Perms_Possiveis) :-
    retira_impossiveis(Perms_Possiveis, Novas_Perms_Possiveis, []).

retira_impossiveis([], Novas_Perms_Possiveis, Novas_Perms_Possiveis) :- !.

retira_impossiveis([[Esp|[Perms]]|R], Novas_Perms_Possiveis, Aux) :-
    include(subsumes_term(Esp), Perms, Perms_Poss),
    length(Perms_Poss, Comp),
    Comp > 0, !, 
    append([Esp], [Perms_Poss], Perms_Esp_Poss),
    append(Aux, [Perms_Esp_Poss], Aux_2),
    retira_impossiveis(R, Novas_Perms_Possiveis, Aux_2).


%--------------------------------------------------------------------------------
% simplifica(Perms_Possiveis, Novas_Perms_Possiveis), em que Perms_Possiveis E 
% uma lista de permutacoes possiveis, significa que Novas_Perms_Possiveis e o 
% resultado de simplificar Perms_Possiveis. 
%--------------------------------------------------------------------------------

simplifica(Perms_Possiveis, Novas_Perms_Possiveis) :-
    atribui_comuns(Perms_Possiveis),
    retira_impossiveis(Perms_Possiveis, Perms_Possiveis_Aux), 
    Perms_Possiveis_Aux \== Perms_Possiveis, !,
    simplifica(Perms_Possiveis_Aux, Novas_Perms_Possiveis).

simplifica(Perms_Possiveis, Perms_Possiveis) :- 
    atribui_comuns(Perms_Possiveis),
    retira_impossiveis(Perms_Possiveis, Perms_Possiveis).


%--------------------------------------------------------------------------------
% inicializa(Puzzle, Perms_Possiveis), em que Puzzle e um puzzle, significa que 
% Perms_Possiveis e a lista de permutacoes possiveis simplificada para Puzzle.
%--------------------------------------------------------------------------------

inicializa(Puzzle, Perms_Possiveis) :-
    espacos_puzzle(Puzzle, Espacos),
    permutacoes_possiveis_espacos(Espacos, Perms_poss_esps),
    simplifica(Perms_poss_esps, Perms_Possiveis).



%======== Predicados para a resolucao de listas de permutacoes possiveis ========

%--------------------------------------------------------------------------------
% escolhe_menos_alternativas(Perms_Possiveis, Escolha), em que Perms_Possiveis e 
% uma lista de permutacoes possiveis, significa que Escolha e o elemento de 
% Perms_Possiveis escolhido.
%--------------------------------------------------------------------------------

escolhe_menos_alternativas([[Esp|[Perms]]|R], Escolha) :-
    length(Perms, Comp),
    Comp > 1, 
    escolhe_menos_alternativas_aux(R, Escolha, [Esp|[Perms]], Comp).

escolhe_menos_alternativas([_|R], Escolha) :-
    escolhe_menos_alternativas(R, Escolha).

escolhe_menos_alternativas_aux([], Escolha, Escolha, _) :- !.

escolhe_menos_alternativas_aux([[Esp|[Perms]]|R], Escolha, _, Max_Comp) :-
    length(Perms, Comp),
    Comp > 1,
    Comp < Max_Comp, 
    escolhe_menos_alternativas_aux(R, Escolha, [Esp|[Perms]], Comp).

escolhe_menos_alternativas_aux([_|R], Escolha, Aux, Max_Comp) :-
    escolhe_menos_alternativas_aux(R, Escolha, Aux, Max_Comp).


%--------------------------------------------------------------------------------
% experimenta_perm(Escolha, Perms_Possiveis, Novas_Perms_Possiveis), em que 
% Perms_Possiveis e uma lista de permutacoes possiveis, e Escolha e um dos 
% seus elementos.
%--------------------------------------------------------------------------------

experimenta_perm(Escolha, Perms_Possiveis, Novas_Perms_Possiveis) :-
    maplist(substitui(Escolha), Perms_Possiveis, Novas_Perms_Possiveis).

 
%--------------------------------------------------------------------------------
% Auxiliar: substitui(Escolha, Perm_Possivel, Nova_Perm_Possivel), significa
% que Nova_Perm_Possivel e o resultado de substituir em Perm_Possivel o elemento
% Escolha pelo elemento [Esp|[Perm]].
%--------------------------------------------------------------------------------

substitui([Esp|[Lst_Perms]], Perm_Possivel, Nova_Perm_Possivel) :-
    [Esp|[Lst_Perms]] == Perm_Possivel, !,
    member(Perm, Lst_Perms),
    Esp = Perm,
    Nova_Perm_Possivel = [Esp, [Perm]].

substitui(_, Perm_Possivel, Perm_Possivel).


%--------------------------------------------------------------------------------
% resolve_aux(Perms_Possiveis, Novas_Perms_Possiveis), em que Perms_Possiveis E 
% uma lista de permutacoes possiveis, significa que Novas_Perms_Possiveis E o 
% resultado de aplicar o algoritmo descrito a Perms_Possiveis.
%--------------------------------------------------------------------------------

resolve_aux(Perms_Possiveis, Novas_Perms_Possiveis) :-
    escolhe_menos_alternativas(Perms_Possiveis, Escolha), !,
    experimenta_perm(Escolha, Perms_Possiveis, Perms_Possiveis_Aux),
    simplifica(Perms_Possiveis_Aux, Perms_Possiveis_Simplificado),
    resolve_aux(Perms_Possiveis_Simplificado, Novas_Perms_Possiveis).

resolve_aux(Perms_Possiveis, Perms_Possiveis).


%--------------------------------------------------------------------------------
% resolve(Puz), em que Puz e um puzzle, resolve esse puzzle, isto e, apos a 
% invocacao deste predicado a grelha de Puz tem todas as variaveis substituidas 
% por numeros que respeitam as restricoes Puz.
%--------------------------------------------------------------------------------

resolve(Puz) :-
    inicializa(Puz, Perms_Possiveis),
    resolve_aux(Perms_Possiveis, _).

    
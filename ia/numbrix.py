# numbrix.py: Template para implementação do projeto de Inteligência Artificial 2021/2022.
# Devem alterar as classes e funções neste ficheiro de acordo com as instruções do enunciado.
# Além das funções e classes já definidas, podem acrescentar outras que considerem pertinentes.

# Grupo 48:
# 97856 Maria Carolina Zagalo e Melo da Silva Gomes
# 93342 Sara Filipa Dinis Marques

import sys
from search import Problem, Node, astar_search, breadth_first_tree_search, depth_first_tree_search, greedy_search, recursive_best_first_search
from copy import deepcopy
from search import compare_searchers
import sys, time

class NumbrixState:
    state_id = 0

    def __init__(self, board):
        self.board = board
        self.id = NumbrixState.state_id
        NumbrixState.state_id += 1

    def __lt__(self, other):
        return self.id < other.id
        
    # TODO: outros metodos da classe


class Board:
    """ Representação interna de um tabuleiro de Numbrix. """
    def __init__(self, dim, grid):
        self.dim = dim
        self.grid = grid
    def get_number(self, row: int, col: int) -> int:
        """ Devolve o valor na respetiva posição do tabuleiro. """
        for i in range(0, self.dim*self.dim):
            if(self.grid[i] == (row, col)):
                return i + 1
        return 0
        pass
    
    def adjacent_vertical_numbers(self, row: int, col: int) -> (int, int):
        """ Devolve os valores imediatamente abaixo e acima, 
        respectivamente. """
        
        row_below = row + 1
        row_above = row - 1

        number_below = None
        number_above = None

        if(row_below < self.dim):
            number_below = self.get_number(row_below, col)
        if(row_above >= 0):
            number_above = self.get_number(row_above, col)
        
        return (number_below, number_above)
    
    def adjacent_horizontal_numbers(self, row: int, col: int) -> (int, int):
        """ Devolve os valores imediatamente à esquerda e à direita, 
        respectivamente. """
        
        col_left = col - 1
        col_right = col + 1

        number_left = None
        number_right = None

        if(col_left >= 0):
            number_left = self.get_number(row, col_left)
        if(col_right < self.dim):
            number_right = self.get_number(row, col_right)
        
        return (number_left, number_right)

    def has_number(self, number: int):
        if(grid[number] != None):
            return True
        return False

    def to_string(self):
        board = ""

        mat = []
        for i in range(0, self.dim):
            mat.append([0]*self.dim)
        
        for num in range(0, self.dim*self.dim):
            if(self.grid[num] != None):
                (row, col) = self.grid[num]
                mat[row][col] = num + 1

        for i in range(0, self.dim):
            for j in range(0, self.dim):
                board += str(mat[i][j])
                
                if(j != self.dim - 1):
                    board += "\t"
            board += "\n"
        return board
    # TODO: outros metodos da classe
    
#@staticmethod    
def parse_instance(filename: str):
    """ Lê o ficheiro cujo caminho é passado como argumento e retorna
    uma instância da classe Board. """
    file = open(filename)

    dim = int(file.readline().strip('\n'))

    grid = [None] * dim * dim
    
    for i in range(0, dim):
        numbers_str = list(file.readline().split())
        numbers = [int(j) for j in numbers_str]

        j = 0
        for num in numbers:
            if(int(num) != 0):
                grid[int(num) - 1] = (i, j)
            j += 1
    board = Board(dim, grid)

    file.close()
    return board


class Numbrix(Problem):
    def __init__(self, board: Board):
        """ O construtor especifica o estado inicial. """
        
        super().__init__(NumbrixState(board))

    def actions(self, state: NumbrixState):
        """ Retorna uma lista de ações que podem ser executadas a
        partir do estado passado como argumento. """
        actions = []

        grid = state.board.grid
        dim = state.board.dim

        # ver se existem por exemplo 10 e 12 na mm linha/coluna e por logo o 11
        for i in range (0, dim*dim):
            if(grid[i] == None):
                continue
            if(i == dim*dim - 1 or i == dim*dim - 2):
                continue
            (row_before, col_before) = grid[i]
            if(grid[i + 1] == None and grid[i + 2] != None):
                (row_after, col_after) = grid[i + 2]
                if(row_before == row_after):
                    if(state.board.get_number(row_before, (col_before + col_after)//2) == 0):
                        actions.append((row_before, (col_before + col_after)//2, i + 2))             
                    return actions 
                elif(col_before == col_after):
                    if(state.board.get_number((row_before + row_after)//2, col_before) == 0):
                        actions.append(((row_before + row_after)//2, col_before, i + 2))                    
                    return actions 

        # ver numero minimo existente no tabuleiro
        min = dim*dim
        for num in range(0, dim*dim):
            if(grid[num] != None):
                min = num 
                break
        # ver qual o proximo numero a por seguindo a sequencia crescente ou decrescente
        number_missing = False

        # crescente
        if(min+1 == 1):
            for num in range(1, dim*dim):
                if(grid[num] == None):
                    num_before = num - 1
                    number_missing = True

                    # procura o que esta a seguir no tabuleiro
                    num_after = -1
                    for n in range(num, dim*dim):
                        if(grid[n] != None):
                            num_after = n
                            break
                    break
        # decrescente
        else:
            for num in range(min, -1, -1):
                if(grid[num] == None):
                    num_before = num + 1
                    number_missing = True
                    num_after = -1
                    for n in range(num, 0, -1):
                        if(grid[n] != None):
                            num_after = n
                            break
                    break
        # se tiver tudo com numero chega ao fim
        if(number_missing == False):
            return actions

        (row_before, col_before) = grid[num_before]
        if(num_after != -1):
            (row_after, col_after) = grid[num_after]

        (v1, v2) = state.board.adjacent_vertical_numbers(row_before, col_before)
        (h1, h2) = state.board.adjacent_horizontal_numbers(row_before, col_before)

        if(v1 == 0):
            if(num_after != -1):
                distance = abs(row_before + 1 - row_after) + abs(col_before - col_after)
                if(distance <= abs(num - num_after)):
                    actions.append((row_before + 1, col_before, num + 1))
            else:
                actions.append((row_before + 1, col_before, num + 1))
        if(v2 == 0):
            if(num_after != -1):
                distance = abs(row_before - 1 - row_after) + abs(col_before - col_after)
                if(distance <= abs(num - num_after)):
                    actions.append((row_before - 1, col_before, num + 1))
            else:
                actions.append((row_before - 1, col_before, num + 1))
        if(h1 == 0):
            if(num_after != -1):
                distance = abs(row_before - row_after) + abs(col_before - 1 - col_after)
                if(distance <= abs(num - num_after)):
                    actions.append((row_before, col_before - 1, num + 1))
            else:   
                actions.append((row_before, col_before - 1, num + 1))
        if(h2 == 0):
            if(num_after != -1):
                distance = abs(row_before - row_after) + abs(col_before + 1 - col_after)
                if(distance <= abs(num - num_after)):
                    actions.append((row_before, col_before + 1, num + 1))
            else:
                actions.append((row_before, col_before + 1, num + 1))

        return actions

    def result(self, state: NumbrixState, action):
        """ Retorna o estado resultante de executar a 'action' sobre
        'state' passado como argumento. A ação a executar deve ser uma
        das presentes na lista obtida pela execução de 
        self.actions(state). """

        (row, col, number) = action 
        new_grid = deepcopy(state.board.grid)
        new_grid[number-1] = (row, col)
        new_board = Board(state.board.dim, new_grid)
        return NumbrixState(new_board)


    def goal_test(self, state: NumbrixState):
        """ Retorna True se e só se o estado passado como argumento é
        um estado objetivo. Deve verificar se todas as posições do tabuleiro 
        estão preenchidas com uma sequência de números adjacentes. """
        grid = state.board.grid
        dim = state.board.dim
        for num in range(0, dim*dim-1):
            if(grid[num] == None):
                return False
            (row, col) = grid[num]

            (v1, v2) = state.board.adjacent_vertical_numbers(row, col)
            (h1, h2) = state.board.adjacent_horizontal_numbers(row, col)

            has_adjacent = False
            num += 1
            if(v1 != None):
                if(v1 == num + 1):
                    has_adjacent = True
            if(v2 != None):
                if(v2 == num + 1):
                    has_adjacent = True
            if(h1 != None):
                if(h1 == num + 1):
                    has_adjacent = True
            if(h2 != None):
                if(h2 == num + 1):
                    has_adjacent = True

            if(has_adjacent == False):
                return False
        return True

    def h(self, node: Node):
        """ Função heuristica utilizada para a procura A*. """
        grid = node.state.board.grid
        dim = node.state.board.dim

        heur = 0
        for num in grid:
            if(num == None):
                heur += 1
        return heur 


if __name__ == "__main__":
    # TODO:
   
    file = sys.argv[1]
    board = parse_instance(file)

    #start_time = time.time()

    problem = Numbrix(board)
    goal_node = greedy_search(problem, problem.h)
    print(goal_node.state.board.to_string(), end="")

    #print("Execution time:",time.time() - start_time, "seconds")

    #compare_searchers(problems=[Numbrix(board)],header=['Algoritmo', 'Numbrix'],searchers=[breadth_first_tree_search, depth_first_tree_search, greedy_search, astar_search])

    pass

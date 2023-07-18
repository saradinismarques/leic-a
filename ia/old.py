# numbrix.py: Template para implementação do projeto de Inteligência Artificial 2021/2022.
# Devem alterar as classes e funções neste ficheiro de acordo com as instruções do enunciado.
# Além das funções e classes já definidas, podem acrescentar outras que considerem pertinentes.

# Grupo 48:
# 97856 Maria Carolina Zagalo e Melo da Silva Gomes
# 93342 Sara Filipa Dinis Marques

import sys
from search import Problem, Node, astar_search, breadth_first_tree_search, depth_first_tree_search, greedy_search, recursive_best_first_search
from copy import deepcopy

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
    def __init__(self, dim, grid, numbers):
        self.dim = dim
        self.grid = grid
        self.numbers = numbers
        self.max = dim*dim
    def get_number(self, row: int, col: int) -> int:
        """ Devolve o valor na respetiva posição do tabuleiro. """
        return self.grid[row][col]
        pass
    
    def adjacent_vertical_numbers(self, row: int, col: int) -> (int, int):
        """ Devolve os valores imediatamente abaixo e acima, 
        respectivamente. """
        
        row_below = row + 1
        row_above = row - 1

        number_below = None
        number_above = None

        if(row_below < self.dim):
            number_below = self.grid[row_below][col]
        if(row_above >= 0):
            number_above = self.grid[row_above][col]
        
        return (number_below, number_above)
    
    def adjacent_horizontal_numbers(self, row: int, col: int) -> (int, int):
        """ Devolve os valores imediatamente à esquerda e à direita, 
        respectivamente. """
        
        col_left = col - 1
        col_right = col + 1

        number_left = None
        number_right = None

        if(col_left >= 0):
            number_left = self.grid[row][col_left]
        if(col_right < self.dim):
            number_right = self.grid[row][col_right]
        
        return (number_left, number_right)

    def has_number(self, number: int):
        for i in range(0, self.dim):
            for j in range(0, self.dim):
                if(self.get_number(i, j) == number):
                    return True
        return False

    def is_close(self, number_before: int, number_after: int):
        row_before = 0
        col_before = 0
        row_after = 0
        col_after = 0
        before_exists = False
        after_exists = False

        for i in range(0, self.dim):
            for j in range(0, self.dim):
                if(self.get_number(i, j) == number_before):
                    row_before = i
                    col_before = j
                    before_exists = True
                if(self.get_number(i, j) == number_after):
                    row_after = i
                    col_after = j
                    after_exists = True
            
        if(not before_exists or not after_exists):
            return True

        if(row_before == row_after):
            if(abs(col_before - col_after) > 2):
                return False
        elif(col_before == col_after):
            if(abs(row_before - row_after) > 2):
                return False
        elif(abs(row_before - row_after) != 1 or abs(col_before - col_after) != 1):
            return False
        return True    


    def to_string(self):
        board = ""

        for i in range(0, self.dim):
            for j in range(0, self.dim):
                board += str(self.grid[i][j])
                
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

    grid = []
    numbers = []

    for i in range(0, dim):
        grid.append(list(file.readline().split()))
        grid[i] = [int(j) for j in grid[i]]
        
        for j in range(0, dim):
            if(grid[i][j] !=  0):
                numbers.append(grid[i][j])

    board = Board(dim, grid, numbers)

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

        for i in range(0, state.board.dim):
            for j in range(0, state.board.dim):
                num = state.board.get_number(i, j)
                if((num + 1) not in state.board.numbers and (num + 2) in state.board.numbers):
                    

        min = state.board.max
        for i in state.board.numbers:
            if(i <= min):
                min = i

        number_missing = False
        if(min == 1):
            for num in range(2, state.board.max + 1):
                if(num not in state.board.numbers):
                    num_before = num - 1
                    num_after = num + 1 
                    number_missing = True

                    next_in_board = 0
                    for n in range(num_after, state.board.max + 1):
                        if(n in state.board.numbers):
                            next_in_board = n
                            break

                    break
        else:
            for num in range(min, 0, -1):
                if(num not in state.board.numbers):
                    num_before = num + 1
                    num_after = num - 1
                    number_missing = True

                    next_in_board = 0
                    for n in range(num_after, 0, -1):
                        if(n in state.board.numbers):
                            next_in_board = n
                            break
                    break

        if(number_missing == False):
            print(actions)
            return actions

        row_before = 0
        col_before = 0
        for i in range(0, state.board.dim):
            for j in range(0, state.board.dim):
                if(state.board.get_number(i, j) == num_before):
                    row_before = i
                    col_before = j


        # if(num_after in state.board.numbers):
        #     row_after = 0
        #     col_after = 0
        #     for i in range(0, state.board.dim):
        #         for j in range(0, state.board.dim):
        #             if(state.board.get_number(i, j) == num_after):
        #                 row_after = i
        #                 col_after = j

        #     if(row_before == row_after):
        #         actions.append((row_before, (col_before + col_after)//2, num))

        #     elif(col_before == col_after):
        #         actions.append(((row_before + row_after)//2, col_before, num))
            
        #     elif(abs(row_before - row_after) == 1 and abs(col_before - col_after) == 1):
        #         actions.append((row_before, col_after, num))
        #         actions.append((row_after, col_before, num))
        #     print(actions)
        #     return actions

        #print(next_in_board)
        
        if(next_in_board != 0):
            row_next_in_board = 0
            col_next_in_board = 0

            for i in range(0, state.board.dim):
                for j in range(0, state.board.dim):
                    if(state.board.get_number(i, j) == next_in_board):
                        row_next_in_board = i
                        col_next_in_board = j

        (v1, v2) = state.board.adjacent_vertical_numbers(row_before, col_before)
        (h1, h2) = state.board.adjacent_horizontal_numbers(row_before, col_before)
        print("NEXT IN BOARD")
        print(next_in_board)
        print(num_before)

        if(v1 == 0):
            if(next_in_board != 0):
                distance = abs(row_before + 1 - row_next_in_board) + abs(col_before - col_next_in_board)
                print("DISTANCE")
                print(distance)
                if(distance <= abs(num - next_in_board)):
                    actions.append((row_before + 1, col_before, num))
            else:
                actions.append((row_before + 1, col_before, num))
        if(v2 == 0):
            if(next_in_board != 0):
                distance = abs(row_before - 1 - row_next_in_board) + abs(col_before - col_next_in_board)
                print("DISTANCE")
                print(distance)
                if(distance <= abs(num - next_in_board)):
                    actions.append((row_before - 1, col_before, num))
            else:
                actions.append((row_before - 1, col_before, num))
        if(h1 == 0):
            if(next_in_board != 0):
                distance = abs(row_before - row_next_in_board) + abs(col_before - 1 - col_next_in_board)
                print("DISTANCE")
                print(distance)
                if(distance <= abs(num - next_in_board)):
                    actions.append((row_before, col_before - 1, num))
            else:   
                actions.append((row_before, col_before - 1, num))
        if(h2 == 0):
            if(next_in_board != 0):
                distance = abs(row_before - row_next_in_board) + abs(col_before + 1 - col_next_in_board)
                print("DISTANCE")
                print(distance)
                if(distance <= abs(num - next_in_board)):
                    actions.append((row_before, col_before + 1, num))
            else:
                actions.append((row_before, col_before + 1, num))

        print(actions)
        return actions

    def result(self, state: NumbrixState, action):
        """ Retorna o estado resultante de executar a 'action' sobre
        'state' passado como argumento. A ação a executar deve ser uma
        das presentes na lista obtida pela execução de 
        self.actions(state). """

        (row, col, number) = action 
        new_grid = deepcopy(state.board.grid)
        new_grid[row][col] = number
        numbers = deepcopy(state.board.numbers)
        numbers.append(number)
        new_board = Board(state.board.dim, new_grid, numbers)
        print(new_board.to_string())
        return NumbrixState(new_board)


    def goal_test(self, state: NumbrixState):
        """ Retorna True se e só se o estado passado como argumento é
        um estado objetivo. Deve verificar se todas as posições do tabuleiro 
        estão preenchidas com uma sequência de números adjacentes. """

        for i in range(0, state.board.dim):
            for j in range(0, state.board.dim):
                number = state.board.get_number(i, j)

                if(number == state.board.max):
                    continue
                if(number == 0):
                    return False
                (v1, v2) = state.board.adjacent_vertical_numbers(i, j)
                (h1, h2) = state.board.adjacent_horizontal_numbers(i, j)

                has_adjacent = False

                if(v1 != None):
                    if(v1 == number + 1):
                        has_adjacent = True
                if(v2 != None):
                    if(v2 == number + 1):
                        has_adjacent = True
                if(h1 != None):
                    if(h1 == number + 1):
                        has_adjacent = True
                if(h2 != None):
                    if(h2 == number + 1):
                        has_adjacent = True

                if(has_adjacent == False):
                    return False

        return True

    def h(self, node: Node):
        """ Função heuristica utilizada para a procura A*. """
        # TODO
        pass
    
    # TODO: outros metodos da classe


if __name__ == "__main__":
    # TODO:
    # Ler o ficheiro de input de sys.argv[1],
    # Usar uma técnica de procura para resolver a instância,
    # Retirar a solução a partir do nó resultante,
    # Imprimir para o standard output no formato indicado.
   
    file = sys.argv[1]
    board = parse_instance(file)

    # problem = Numbrix(board)
    # initial_state = NumbrixState(board)

    # actions = problem.actions(initial_state)
    # second_state = problem.result(initial_state, actions[0])
    # print(second_state.board.grid)
    # print(initial_state.board.grid)
    # #actions = problem.actions(second_state)
    #third_state = problem.result(initial_state, actions[0])
    #print(third_state.board.grid)


    # ---------- EXEMPLO 1 ----------
    # print("Initial:\n", board.to_string(), sep="")

    # print(board.adjacent_vertical_numbers(2, 2))
    # print(board.adjacent_horizontal_numbers(2, 2))

    # print(board.adjacent_vertical_numbers(1, 1))
    # print(board.adjacent_horizontal_numbers(1, 1))
    
    # ---------- EXEMPLO 2 ----------
    # problem = Numbrix(board)
    # initial_state = NumbrixState(board)

    # print(initial_state.board.get_number(2, 2))
    # result_state = problem.result(initial_state, (2, 2, 1))
    # print(result_state.board.get_number(2, 2))
 
    # ---------- EXEMPLO 3 ----------
    # problem = Numbrix(board)
    # s0 = NumbrixState(board)
    # print("Initial:\n", s0.board.to_string(), sep="")

    # s1 = problem.result(s0, (2, 2, 1))
    # s2 = problem.result(s1, (0, 2, 3))
    # s3 = problem.result(s2, (0, 1, 4))
    # s4 = problem.result(s3, (1, 1, 5))
    # s5 = problem.result(s4, (2, 0, 7))
    # s6 = problem.result(s5, (1, 0, 8))
    # s7 = problem.result(s6, (0, 0, 9))

    # print("Is goal?", problem.goal_test(s7))
    # print("Solution:\n", s7.board.to_string(), sep="")
    
    # ---------- EXEMPLO 4 ----------
    #print("Initial:\n", board.to_string(), sep="")

    problem = Numbrix(board)
    goal_node = breadth_first_tree_search(problem)
    #print("Is goal?", problem.goal_test(goal_node.state))
    print(goal_node.state.board.to_string(), end="")

    pass

# v8_ricochet_robots.py: Template para implementação do 1º projeto de Inteligência Artificial 2020/2021.
# Devem alterar as classes e funções neste ficheiro de acordo com as instruções do enunciado.
# Além das funções e classes já definidas, podem acrescentar outras que considerem pertinentes.

# Grupo 52:
# 82374 Luana Ascensão de Jesus Marques
# 93342 Sara Filipa Dinis Marques

from search import *
import sys
import copy as cp


# NAO DEVE SER ALTERADA!!!
class RRState:
    state_id = 0

    def __init__(self, board):
        self.board = board
        self.id = RRState.state_id
        RRState.state_id += 1

    def __lt__(self, other):
        """ Este método é utilizado em caso de empate na gestão da lista de abertos nas procuras informadas. """
        return self.id < other.id


# ______________________________________________________________________________________________________________________________
def collides(board, bot: str, action):
    pos = board.robot_index_position(bot)

    if action == "d" and (
            pos[0] == board.dimension - 1 or board.has_barrier(pos[0], pos[1], 'd') or board.has_barrier(pos[0] + 1,
                                                                                                         pos[1],
                                                                                                         'u') or not board.is_empty_cel(
            pos[0] + 1, pos[1])):
        # print("'{}' CAN'T GO DOWN!!!".format(bot)) #DEBUG
        return True

    elif action == "u" and (
            pos[0] == 0 or board.has_barrier(pos[0], pos[1], 'u') or board.has_barrier(pos[0] - 1, pos[1],
                                                                                       'd') or not board.is_empty_cel(
            pos[0] - 1, pos[1])):
        # print("'{}' CAN'T GO UP!!!".format(bot)) #DEBUG
        return True

    elif action == "r" and (
            pos[1] == board.dimension - 1 or board.has_barrier(pos[0], pos[1], 'r') or board.has_barrier(pos[0],
                                                                                                         pos[1] + 1,
                                                                                                         'l') or not board.is_empty_cel(
            pos[0], pos[1] + 1)):
        # print("'{}' CAN'T GO RIGHT!!!".format(bot)) #DEBUG
        return True

    elif action == "l" and (
            pos[1] == 0 or board.has_barrier(pos[0], pos[1], 'l') or board.has_barrier(pos[0], pos[1] - 1,
                                                                                       'r') or not board.is_empty_cel(
            pos[0], pos[1] - 1)):
        # print("'{}' CAN'T GO LEFT!!!".format(bot)) #DEBUG
        return True
    else:
        return False


# devolve as posiveis acoes de um robo
# devolve as posiveis acoes de um robo...uma acao eh representada por ('B', 'l'), i.e., "Blue to left"
def poss_robot_moves(bot: str, board):
    lines = cols = board.dimension
    robot = board.robot_index_position(bot)
    moves = []

    # if robot na linha superior <=> linha=0:
    if robot[0] == 0:
        #   if robot no canto esquerdo <=> coluna=0 -> acoes: down, right
        if robot[1] == 0:
            if not collides(board, bot, "d"):
                moves.append((bot, 'd'))

            if not collides(board, bot, "r"):
                moves.append((bot, 'r'))

        #   elif robot no canto direito <=> coluna=dimensao -> acoes: down, left
        elif robot[1] == cols - 1:
            if not collides(board, bot, "d"):
                moves.append((bot, 'd'))

            if not collides(board, bot, "l"):
                moves.append((bot, 'l'))

        #   else noutras celulas <=> 0 < coluna < dimensao -> acoes: down, left, right
        else:
            if not collides(board, bot, "d"):
                moves.append((bot, 'd'))

            if not collides(board, bot, "r"):
                moves.append((bot, 'r'))

            if not collides(board, bot, "l"):
                moves.append((bot, 'l'))

    # elif robot na coluna mais a esquerda (excluindo o 1o e o último) -> acoes: "up", down, right
    elif robot[1] == 0:
        if not collides(board, bot, "u"):
            moves.append((bot, 'u'))

        if not collides(board, bot, "d"):
            moves.append((bot, 'd'))

        if not collides(board, bot, "r"):
            moves.append((bot, 'r'))

    # elif robot na coluna mais a direita (excluindo o 1o e o último) -> acoes: "up", down, left
    elif robot[1] == cols - 1:
        if not collides(board, bot, "u"):
            moves.append((bot, 'u'))

        if not collides(board, bot, "d"):
            moves.append((bot, 'd'))

        if not collides(board, bot, "l"):
            moves.append((bot, 'l'))

    # elif robot na linha inferior <=> linha=dimensao:
    elif robot[0] == lines - 1:
        #   if robot no canto esquerdo <=> coluna=0 -> acoes: "up", right
        if robot[1] == 0:
            if not collides(board, bot, "u"):
                moves.append((bot, 'u'))

            if not collides(board, bot, "r"):
                moves.append((bot, 'r'))

        #   elif robot no canto direito <=> coluna=dimensao -> acoes: "up", left
        elif robot[1] == cols - 1:
            if not collides(board, bot, "u"):
                moves.append((bot, 'u'))

            if not collides(board, bot, "l"):
                moves.append((bot, 'l'))
        #   else noutras celulas <=> 0 < coluna < dimensao -> acoes: "up", left, right
        else:
            if not collides(board, bot, "u"):
                moves.append((bot, 'u'))

            if not collides(board, bot, "l"):
                moves.append((bot, 'l'))

            if not collides(board, bot, "r"):
                moves.append((bot, 'r'))
    # else celulas no meio do tabuleiro
    else:
        if not collides(board, bot, "u"):
            moves.append((bot, 'u'))

        if not collides(board, bot, "d"):
            moves.append((bot, 'd'))
        if not collides(board, bot, "l"):
            moves.append((bot, 'l'))

        if not collides(board, bot, "r"):
            moves.append((bot, 'r'))

    return moves


def possible_actions(board):
    board = cp.deepcopy(board)
    actions = []
    actions += poss_robot_moves('Y', board)
    actions += poss_robot_moves('R', board)
    actions += poss_robot_moves('G', board)
    actions += poss_robot_moves('B', board)
    return actions


# _____________________________________________________________________________________________________
class Board:
    """ Representacao interna de um tabuleiro de Ricochet Robots. """

    def __init__(self, dimension, target):
        self.board = []
        self.dimension = dimension
        self.target = target
        # lista dos tuplos das barreiras fixas do tabuleiro
        self.barriers = []
        self.barrier_pos = []
        self.robots_start = []
        self.robots = ("B", "G", "R", "Y")

    def robot_position(self, robot: str):
        """ Devolve a posição atual do robô passado como argumento. """
        for l, linha in enumerate(self.board, start=1):
            for c, coluna in enumerate(linha, start=1):
                if coluna == robot:
                    return (l, c)

    def robot_index_position(self, robot: str):
        """ Devolve a posição atual (em termos de indices) do robô passado como argumento. """
        for l, linha in enumerate(self.board):
            for c, coluna in enumerate(linha):
                if coluna == robot:
                    return (l, c)

    def board_init(self, dim):
        self.board = []
        for i in range(dim):
            self.board.append([0] * dim)

    def target(self):
        return self.target

    # TODO: outros metodos da classe
    def barriers_initial(self, pos):
        l = int(pos[0])
        c = int(pos[1])
        self.barriers.append([l - 1 if l > 0 else l, c - 1 if c > 0 else c, pos[2]])
        self.barrier_pos.append([l, c, pos[2]])

    def has_barrier(self, l, c, d):
        return [l, c, d] in self.barriers

    def has_barrier_pos(self, l, c, d):
        return [l, c, d] in self.barrier_pos

    def robots_initial(self, bot):
        l = int(bot[1])
        c = int(bot[2])
        self.robots_start.append([bot[0], l, c])
        self.board[l - 1 if l > 0 else l][c - 1 if c > 0 else c] = bot[0]

    def has_robot(self, l, c):
        return self.board[l][c] in self.robots

    # verifica se uma celula/casa esta vazia
    def is_empty_cel(self, l, c):
        return True if self.board[l][c] == 0 or self.board[l][c] == 'T' else False

    def target_acquired(self):
        robot = str(self.target[0])
        robot_final_pos = self.robot_position(robot)
        target = (int(self.target[1]), int(self.target[2]))
        if robot_final_pos[0] == target[0] and robot_final_pos[1] == target[1]:
            return True
        else:
            return False

    def add_barrier(self, pos):
        l = int(pos[0])
        c = int(pos[1])
        d = str(pos[2])

        if is_empty_cel(l, c):
            self.board[l][c] = d
        else:
            prev_info = self.board[l][c]
            self.board[l][c] = [prev_info, d]

    def add_target(self):
        t = self.target
        l = int(t[1])
        c = int(t[2])
        self.board[l - 1 if l > 0 else l][c - 1 if c > 0 else c] = 'T'

    # auxiliar
    def print_board(self):
        self.add_target()
        for i in range(self.dimension):
            print(self.board[i])
        print()


# Funcoes independentes__________________________________________________________________________
def update_board(board: Board, action):
    bot = action[0]
    dir = action[1]
    dim = board.dimension
    pos = board.robot_index_position(bot)
    l = pos[0]
    c = pos[1]

    while not collides(board, action[0], action[1]):
        board.board[l][c] = 0
        l = new_line(l, dir, dim)
        c = new_column(c, dir, dim)
        board.board[l][c] = bot
    return board


def new_line(pos, action, dim):
    if action == 'd':
        if pos < dim - 1:
            return pos + 1
        else:
            return dim - 1
    elif action == 'u':
        if pos > 0:
            return pos - 1
        else:
            return 0
    else:
        return pos


def new_column(pos, action, dim):
    if action == 'r':
        if pos < dim - 1:
            return pos + 1
        else:
            return dim - 1
    elif action == 'l':
        if pos > 0:
            return pos - 1
        else:
            return 0
    else:
        return pos

# funcao auxiliar
cleanwhsp = lambda string: string.replace(' ', '')


# def cleanwhsp(string:str) -> str:
#   return string.replace(" ", "")

def parse_instance(filename: str) -> Board:
    """ Lê o ficheiro cujo caminho é passado como argumento e retorna
    uma instância da classe Board. """
    # TO DO
    myproblem = open(filename)

    # ler dimensao do tabuleiro
    dim = int(myproblem.readline().strip('\n'))

    # ler posicao de cada robo...e as barreiras que eles criam
    robot1 = list(next(myproblem).split())
    robot2 = list(next(myproblem).split())
    robot3 = list(next(myproblem).split())
    robot4 = list(next(myproblem).split())

    # ler posicao do alvo
    alvo = list(next(myproblem).split())

    # criar tabuleiro
    newBoard = Board(dim, alvo)

    newBoard.board_init(dim)

    newBoard.robots_initial(robot1)
    newBoard.robots_initial(robot2)
    newBoard.robots_initial(robot3)
    newBoard.robots_initial(robot4)
    # ler nr total de barreiras
    bars = int(next(myproblem))
    # ler e guardar posicao de cada barreira
    while bars:
        pos = list(next(myproblem).split())
        newBoard.barriers_initial(pos)
        bars -= 1
    # no final do processamento fechar o ficheiro input
    myproblem.close()
    return newBoard


def print_output(solution_list):
    print(len(solution_list))
    for tup in solution_list:
        print("%s %s", tup[0], tup[1])


def has_up(board: Board, l, c):

    if l <= 1:
        return True

    if board.has_barrier_pos(l, c, 'u') or board.has_barrier_pos(l - 1, c, 'd'):
        return True

    for robot in board.robots:
        if robot != board.target[0]:
            robot_pos = board.robot_position(robot)
            if robot_pos[0] == l - 1:
                return True
    return False


def has_down(board: Board, l, c):

    if l >= board.dimension:
        return True

    if board.has_barrier_pos(l, c, 'd') or board.has_barrier_pos(l + 1, c, 'u'):
        return True

    for robot in board.robots:
        if robot != board.target[0]:
            robot_pos = board.robot_position(robot)
            if robot_pos[0] == l + 1:
                return True
    return False

def has_left(board: Board, l, c):

    if c <= 1:
        return True

    if board.has_barrier_pos(l, c, 'l') or board.has_barrier_pos(l, c - 1, 'r'):
        return True

    for robot in board.robots:
        if robot != board.target[0]:
            robot_pos = board.robot_position(robot)
            if robot_pos[1] == c - 1:
                return True
    return False


def has_right(board: Board, l, c):

    if c >= board.dimension:
        return True

    if board.has_barrier_pos(l, c, 'r') or board.has_barrier_pos(l, c + 1, 'l'):
        return True

    for robot in board.robots:
        if robot != board.target[0]:
            robot_pos = board.robot_position(robot)
            if robot_pos[1] == c + 1:
                return True
    return False

def available_vertical_way(board: Board, pos1, pos2):
    if pos1 >= pos2:
        max = pos1
        min = pos2
    else:
        max = pos2
        min = pos1

    for i in range(min+1, max):
        if has_up(board, i, pos1) or has_down(board, i, pos1):
            return False
        else:
            return True


def available_horizontal_way(board: Board, pos1, pos2):
    if pos1 >= pos2:
        max = pos1
        min = pos2
    else:
        max = pos2
        min = pos1

    for i in range(min + 1, max):
        if has_right(board, pos1, i) or has_left(board, pos1, i):
            return False
        else:
            return True

def find_new_vertical_support(board: Board):
    target_pos = (int(board.target[1]), int(board.target[2]))

    for robot in board.robots:
        if robot != board.target[0]:
            robot_pos = board.robot_position(robot)
            if available_vertical_way(board, robot_pos[0], target_pos[0]):
                if robot_pos[1] == target_pos[1] and (has_down(board, target_pos[0] + 1, target_pos[1]) or has_up(board, target_pos[0] - 1, target_pos[1])):
                    return 1
                elif robot_pos[1] == target_pos[1]-1 and (has_down(board, target_pos[0], target_pos[1]-1) or has_up(board, target_pos[0], target_pos[1]-1)):
                    return 1
                elif robot_pos[1] == target_pos[1]+1 and (has_down(board, target_pos[0], target_pos[1]-1) or has_up(board, target_pos[0], target_pos[1]+1)):
                    return 1
            elif available_horizontal_way(board, robot_pos[1], target_pos[1]):
                if available_vertical_way(board, target_pos[0], robot_pos[0]):
                    if robot_pos[1] == target_pos[1] and (has_down(board, target_pos[0] + 1, target_pos[1]) or has_up(board, target_pos[0] - 1, target_pos[1])):
                        return 2
                    elif robot_pos[1] == target_pos[1] - 1 and (has_down(board, target_pos[0], target_pos[1] - 1) or has_up(board, target_pos[0], target_pos[1] - 1)):
                        return 2
                    elif robot_pos[1] == target_pos[1] + 1 and (has_down(board, target_pos[0], target_pos[1] - 1) or has_up(board, target_pos[0], target_pos[1] + 1)):
                        return 2
        return False

def find_new_horizontal_support(board: Board):
    target_pos = (int(board.target[1]), int(board.target[2]))

    for robot in board.robots:
        if robot != board.target[0]:
            robot_pos = board.robot_position(robot)
            if available_horizontal_way(board, robot_pos[0], target_pos[0]):
                if robot_pos[0] == target_pos[0] and (has_right(board, target_pos[0], target_pos[1]+1) or has_left(board, target_pos[0], target_pos[1]-1)):
                    return 1
                elif robot_pos[0] == target_pos[0]-1 and (has_down(board, target_pos[0]-1, target_pos[1]) or has_up(board, target_pos[0]-1, target_pos[1])):
                    return 1
                elif robot_pos[0] == target_pos[0]+1 and (has_down(board, target_pos[0]+1, target_pos[1]) or has_up(board, target_pos[0]+1, target_pos[1])):
                    return 1
            elif available_vertical_way(board, robot_pos[1], target_pos[1]):
                if available_horizontal_way(board, target_pos[0], robot_pos[0]):
                    if robot_pos[0] == target_pos[0] and (has_right(board, target_pos[0], target_pos[1] + 1) or has_left(board, target_pos[0],target_pos[1] - 1)):
                        return 2
                    elif robot_pos[0] == target_pos[0] - 1 and (has_down(board, target_pos[0] - 1, target_pos[1]) or has_up(board, target_pos[0] - 1,target_pos[1])):
                        return 2
                    elif robot_pos[0] == target_pos[0] + 1 and (has_down(board, target_pos[0] + 1, target_pos[1]) or has_up(board, target_pos[0] + 1,target_pos[1])):
                        return 2
        return False



# ________________________________________________________________________
class RicochetRobots(Problem):
    def __init__(self, board: Board):
        """ O construtor especifica o estado inicial. """
        # TODO: self.initial = ...
        self.initial = RRState(board)
        self.board = self.initial.board

    def actions(self, state: RRState):
        """ Retorna uma lista de ações que podem ser executadas a
        partir do estado passado como argumento. """
        # TODO
        return possible_actions(state.board)

    def result(self, state: RRState, action):
        """ Retorna o estado resultante de executar a 'action' sobre
        'state' passado como argumento. A ação retornada deve ser uma
        das presentes na lista obtida pela execução de
        self.actions(state). """
        # TODO
        board_copy = cp.deepcopy(state.board)
        return RRState(update_board(board_copy, action))

    def goal_test(self, state: RRState):
        """ Retorna True se e só se o estado passado como argumento é
        um estado objetivo. Deve verificar se o alvo e o robô da
        mesma cor ocupam a mesma célula no tabuleiro. """
        # TODO
        return state.board.target_acquired()

    def h(self, node: Node):
        """ Função heuristica utilizada para a procura A*. """
        # TODO

        if node.action:
            board_copy = cp.deepcopy(node.state.board)
            board_after_action = update_board(board_copy, node.action)
            target = node.state.board.target[0]
            target_pos = (int(node.state.board.target[1]), int(node.state.board.target[2]))
            robot_pos = board_after_action.robot_position(target)

            if target_pos == robot_pos:
                return 0

            elif has_up(board_after_action, target_pos[0], target_pos[1]) or has_down(board_after_action, target_pos[0], target_pos[1]):
                if robot_pos[1] == target_pos[1]:
                    return 1
                elif has_left(board_after_action, robot_pos[0], target_pos[1]):
                    return 2
                elif has_right(board_after_action, robot_pos[0], target_pos[1]):
                    return 2

            elif has_left(board_after_action, target_pos[0], target_pos[1]) or has_right(board_after_action, target_pos[0], target_pos[1]):
                if robot_pos[0] == target_pos[0]:
                    return 1
                elif has_down(board_after_action, target_pos[0], robot_pos[1]):
                    return 2
                elif has_up(board_after_action, target_pos[0], robot_pos[1]):
                    return 2
            else:
                return 3
        return 1



# _____________________________________________________________________________________-
if __name__ == "__main__":
    # TODO:
    # Ler o ficheiro de input de sys.argv[1],
    # Usar uma técnica de procura para resolver a instância,
    # Retirar a solução a partir do nó resultante,
    # Imprimir para o standard output no formato indicado.

    instance_file = sys.argv[1]
    board = parse_instance(instance_file)

    problem = RicochetRobots(board)

    # solution_node = breadth_first_tree_search(problem)
    # solution_node = depth_first_tree_search(problem)
    solution_node = astar_search(problem)
    # solution_node = greedy_search(problem)

    print(len(solution_node.solution()))
    for i in solution_node.solution():
        print(*i, sep=' ')




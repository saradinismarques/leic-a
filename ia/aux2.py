def actions(self, state: NumbrixState):
        """ Retorna uma lista de ações que podem ser executadas a
        partir do estado passado como argumento. """
        
        actions = []
        without_numbers = []
        for i in range(0, state.board.dim):
            for j in range(0, state.board.dim):
                number = state.board.get_number(i, j)
                if(number == 0):
                    (v1, v2) = state.board.adjacent_vertical_numbers(i, j)
                    
                    with_number = False

                    if(v1 != None and v1 != 0):
                        if(state.board.has_number(v1 + 1) == False and v1 + 1 <= state.board.dim*state.board.dim):
                            actions.append((i, j, v1 + 1))
                        if(state.board.has_number(v1 - 1) == False and v1 - 1 > 0):
                            actions.append((i, j, v1 - 1))
                        with_number = True
                    if(v2 != None and v2 != 0):
                        if(state.board.has_number(v2 + 1) == False and v2 + 1 <= state.board.dim*state.board.dim):
                            actions.append((i, j, v2 + 1))
                        if(state.board.has_number(v2 - 1) == False and v2 - 1 > 0):
                            actions.append((i, j, v2 - 1))
                        with_number = True


                    (h1, h2) = state.board.adjacent_horizontal_numbers(i, j)

                    if(h1 != None and h1 != 0):
                        if(state.board.has_number(h1 + 1) == False and h1 + 1 <= state.board.dim*state.board.dim):
                            actions.append((i, j, h1 + 1))
                        if(state.board.has_number(h1 - 1) == False and h1 - 1 > 0):
                            actions.append((i, j, h1 - 1))
                        with_number = True


                    if(h2 != None and h2 != 0):
                        if(state.board.has_number(h2 + 1) == False and h2 + 1 <= state.board.dim*state.board.dim):
                            actions.append((i, j, h2 + 1))
                        if(state.board.has_number(h2 - 1) == False and h2 - 1 > 0):
                            actions.append((i, j, h2 - 1))
                        with_number = True
               
                    if(with_number == False):
                        without_numbers.append((i, j))

        num_missing = []
        for n in range(1, state.board.dim*state.board.dim+1):
            exists = False
            if(state.board.has_number(n)):
                exists = True
            for (i, j, num) in actions:
                if(n == num):
                    exists = True
            if(exists == False):
                num_missing.append(n)
        
        for (i, j) in without_numbers:
            for n in num_missing:
                actions.append((i, j, n))

        for (i, j, num1) in actions:
            certain = True
            for (k, l, num2) in actions:
                if((i, j, num1) == (k, l, num2)):
                    continue
                if(num1 == num2):
                    certain = False
            
            if(certain):
                for (k, l, num2) in actions:
                    if((i, j, num1) == (k, l, num2)):
                        continue
                    if((i, j) == (k, l)):
                        actions.remove((k, l, num2))

        print(actions)           
        return actions

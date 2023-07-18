## Grupo 48
**Francisca Ordaz:** ist193710

**Sara Marques:**  ist93342

**Maria Gomes:** ist97856

Nesta terceira parte do projeto, assumimos que os relógios estão sincronizados. Temos um vector com *n* entradas para cada um dos servidores, sendo que cada servidor tem um identificador *i*, que remete a uma posição de cada vector. Inicialmente todos eles estão inicializados com o valor 00:00. A cada update que é feito, o “vector clock” do servidor respetivo é atualizado com o valor do tempo na entrada vector_clock[i], sendo *i* o identificador que demos ao servidor quando este foi criado.

Periodicamente, o servidor procura sincronizar-se com os restantes, enviando os seus updates aos outros servidores que ainda não tenham sido atualizados com esses updates. Para cada um dos servidores, a nossa função updateVectorClock verifica se a entrada *i* do vector clock do nosso servidor está mais à frente (ou seja foi atualizado desde o último gossip) do que a entrada *i* do vector clock do outro servidor, sendo *i* neste caso o identificador do nosso servidor. Utilizamos um hashmap que atribui a cada operação feita o seu tempo respectivo. Isto permite-nos saber quais são as operações que é necessário propagar para o outro servidor. 

De forma a evitar propagar vezes sem conta os mesmo “updates” devemos atualizar o vector clock do outro servidor.
Sabendo quais as operações de “update” que foram feitas, fazemos o respectivo pedido ao servidor: “openUpdate” caso tenham sido abertas as inscrições, “enrollUpdate” caso tenha sido adicionado um novo aluno à turma, entre outros.
#include "PD_server.h"
#include "PD_client.h"

int flag;

void checkVLC();

int main(int argc, char * argv[]) {
    flag = 1;
    strcpy(UID,"\0");
    strcpy(password,"\0");
    parsingArguments(argc, argv);
    openSocket(TEJO, asPORT);
    connectServer(pdPORT);

    selection();

    closeServer();
    closeSocket();
}

void selection(){
    fd_set inputs, testfds;
    int out_fds;
    char message[SIZE];

    FD_ZERO(&inputs); //Clear inputs
    FD_SET(0, &inputs); // Set standard input channel on
    //FD_SET(fd, &inputs); // Set socket channel on
    FD_SET(fd_server, &inputs); // Set socket channel on

    while(1){
        testfds = inputs;

        out_fds = select(FD_SETSIZE, &testfds, (fd_set *)NULL, (fd_set *)NULL, NULL);

        /*printf("out_fds: %d\n", out_fds);
        printf("FD_ISSET: fd_server %d fd %d stdin %d\n", FD_ISSET(fd_server,&testfds), 
            FD_ISSET(fd,&testfds), FD_ISSET(0,&testfds));*/

        if(out_fds < 0){
            /*error*/
            perror("select");
            exit(1);
        }
        if(FD_ISSET(0,&testfds)){
            strcpy(message,parsingSTDinArguments());
            if(!strcmp(message, "UNR  \n")){
                puts("EXITING");
                flag = 0;
            } else{
                reg(message);
                flag = readFromSocket();
            }

        } 
        if(FD_ISSET(fd_server,&testfds)){
            receiveMessage();
            checkVLC();
        }
        if(!flag) //recebeu 'RUN OK\n'
            break;
    }
}

void checkVLC(){
    char RVC[SIZE];
    strcpy(RVC, "RVC ");
    //printf("uid_as: %s, UID: %s", uid_as, UID);
    if (!strcmp(uid_as,UID)){
        strcat(RVC, UID);
        strcat(RVC, " ");
        strcat(RVC, "OK\n");
        sendStatusReply(RVC);
    } else {
        strcat(RVC, UID);
        strcat(RVC, " ");
        strcat(RVC, "OK\n");
        sendStatusReply(RVC);
    }
}
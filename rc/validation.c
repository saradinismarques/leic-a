#include <ctype.h>

#define UID_SIZE 6
#define PASS_SIZE 9
#define GID_SIZE 3
#define GNAME_SIZE 25
#define FNAME_SIZE 25
#define EXT_SIZE 4
#define MID_SIZE 5

#define SUCCESS 1
#define ERROR 0

int validUID(char *UID) {
    if(strlen(UID) == UID_SIZE-1) {
        for(int i = 0; i < UID_SIZE-1; i++) {
            if(!isdigit(UID[i])) {
                puts("ERROR: UID is not a number");
                return ERROR;
            }
        }
        return SUCCESS;
    }
    puts("ERROR: UID size is wrong");
    return ERROR;
}

int validPass(char *pass) {
    if(strlen(pass) == PASS_SIZE-1) {
        for(int i = 0; i < PASS_SIZE-1; i++) {
            if(!isalnum(pass[i])) {
                puts("ERROR: Password is not alphanumeric");
                return ERROR;
            }
        }
        return SUCCESS;
    }
    puts("ERROR: Password size is wrong");
    return ERROR;
}

int validGID(char *GID) {
    if(strcmp(GID, "0") == 0)
        strcat(GID, "0");
    if(strlen(GID) == GID_SIZE-1) {
        for(int i = 0; i < GID_SIZE-1; i++) {
            if(!isdigit(GID[i])) {
                puts("ERROR: GID is not a number");
                return ERROR;
            }
        }
        return SUCCESS;
    }
    puts("ERROR: GID size is wrong");
    return ERROR;
}

int validGName(char *GName) {
    int GName_size = strlen(GName);

    if (GName_size <= GNAME_SIZE-1) {
        for(int i = 0; i < GName_size; i++) {
            if(!isalnum(GName[i]) && GName[i] != '-' && GName[i] != '_') {
                puts("ERROR: Group name is not alphanumeric or '-' or '_'");
                return ERROR;
            }
        }
        return SUCCESS;
    }
    puts("ERROR: Group name size is wrong");
    return ERROR;
}

int validText(char *text) {
    if(strlen(text) <= 240) {
        return SUCCESS;
    }
    puts("ERROR: Text size is wrong");
    return ERROR;
}

int validFName(char *FName) {
    if(strlen(FName) == 0) 
        return SUCCESS;

    int FName_size = strlen(FName);
    char FName_copy[FName_size];
    strcpy(FName_copy, FName);

    if(strlen(FName_copy) <= FNAME_SIZE-1) {
        char *file = strtok(FName_copy, ".");
        if(file == NULL) {
            puts("ERROR: File name is not in the correct form");
            return ERROR;
        }
        int file_size = strlen(file);

        for(int i = 0; i < file_size; i++) {
            if(!isalnum(file[i]) && file[i] != '-' && file[i] != '_') {
                puts("ERROR: File name is not alphanumeric or '-' or '_'");
                return ERROR;
            }
        }
        
        char *ext = strtok(NULL, ".");
        if(ext == NULL) {
            puts("ERROR: File name is not in the correct form");
            return ERROR;
        }
        if(strlen(ext) == EXT_SIZE-1) {
            for(int i = 0; i < EXT_SIZE-1; i++) {
                if(!isalnum(ext[i])){
                    puts("ERROR: File name extension is not alphanumeric");
                    return ERROR;
                }
            }
            return SUCCESS;
        }
        puts("ERROR: File name extension size is wrong");
        return ERROR;
    }
    puts("ERROR: File name size is wrong");
    return ERROR;
}

int validMID(char *MID) {
    if (strlen(MID) == MID_SIZE-1) {
        for(int i = 0; i < MID_SIZE-1; i++) {
            if(!isdigit(MID[i])) {
                puts("ERROR: MID is not a number");
                return ERROR;
            }
        }
        return SUCCESS;
    }
    puts("ERROR: MID size is wrong.");
    return ERROR;
}

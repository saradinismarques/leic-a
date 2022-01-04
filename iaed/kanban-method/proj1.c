/*
 * File: proj1.c
 * Author: Sara Marques
 * Number: 93342
 * Description: Kanban Method - Task management system in a project.
*/

#include "proj1.h"

/*------------------------------Main Function-------------------------------*/

/* Declares the variables that will store all the data in the system. Reads 
the command given in a loop and executes the corresponding function until it 
receives the command 'q'. After that returns 0 if no problem has occured. */
int main() {
    /* Array that stores all the tasks in the system. */
    Task tasks[MAXTASKS];

    /* Array that stores all the users in the system. */
    User users[MAXUSERS];

    /* Array that stores all the activities in the system. */
    Activity activities[MAXACTIVITIES];

    /* Counters for the number of values stored in the three arrays. */
    int taskCounter = 0, userCounter = 0, activityCounter = 3;

    /* Current time in the system. */
    int currentTime = 0;

    int c;

    /* Initializes the array activities with three inicial activities. */
    strcpy(activities[0].name, TODO);
    strcpy(activities[1].name, INPROGRESS);
    strcpy(activities[2].name, DONE);


    while((c = getchar()) != 'q') {
        switch (c) {
            case 't':
                taskCounter = addTask(tasks, taskCounter);
                break;
            case 'l':
                listTasks(tasks, taskCounter);
                break;
            case 'n':
                currentTime = advanceTime(currentTime);
                break;
            case 'u':
                userCounter = addUser(users, userCounter);
                break;
            case 'm':
                moveTask(tasks, users, activities, taskCounter, userCounter, 
                        activityCounter, currentTime);
                break;
            case 'd':
                listTasksByActivity(tasks, activities, taskCounter, 
                                    activityCounter);
                break;
            case 'a':
                activityCounter = addActivity(activities, activityCounter);
                break;
        }
    }
    return 0;
}

/*----------------------------Auxiliary Functions----------------------------*/

/* Reads given string which can contain white spaces. It is also passed the 
maximum size the string can have. */
void readString(char string[], int maxSize) {
    int i = 0, c;

    c = getchar();

    /* Checks if it has reached the end. */
    while(c != '\n' && c != EOF && i < maxSize) {
        string[i++] = c;
        c = getchar();
    }

    /* Terminates string. */
    string[i] = '\0';
}

/* Checks if the command was given with or without arguments. */
int hasArgs() {
    int c = getchar();

    /* Checks next character. */
    if(c == '\n' || c == EOF) {
        /* Has no arguments. */
        return NO; 
    } else {
        /* Has arguments. */
        return YES; 
    }
}

/* Checks if task with given description exists in the array tasks. */
int taskExists(Task tasks[], int taskCounter, char description[]) {
    int i;

    for(i = 0; i < taskCounter; i++) {
        if(strcmp(tasks[i].description, description) == 0) {
            /* Task found. */
            return YES; 
        }
    }
    /* Task not found. */
    return NO; 
}

/* Checks if user with given name exists in the array users. */
int userExists(User users[], int userCounter, char name[]) {
    int i;

    for(i = 0; i < userCounter; i++) {
        if(strcmp(users[i].name, name) == 0) {
             /* User found. */
            return YES;
        }
    }
    /* User not found. */
    return NO; 
}

/* Checks if activity with given name exists in the array activities. */
int activityExists(Activity activities[], int activityCounter, char name[]) {
    int i;

    for(i = 0; i < activityCounter; i++) {
        if(strcmp(activities[i].name, name) == 0) { 
            /* Activity found. */
            return YES; 
        }
    }
    /* Activity not found. */
    return NO; 
}

/* Quicksort algorithm. Sorts given array of type Task according to given 
comparison type. It is also passed the integers left and right which are 
the indexes of the left and right positions in the array. */
void quicksort(Task toOrder[], int left, int right, int type) {
    int i, j, pivot;

    if(left < right) {
        
        /* Chooses left element for pivot. */
        pivot = left;
        i = left;
        j = right;

        while(i < j) {
            while(less(toOrder[i], toOrder[pivot], type) && i < right) 
                i++;

            while(!less(toOrder[j], toOrder[pivot], type)) 
                j--;
                
            if(i < j) 
                exch(toOrder[i], toOrder[j]);
        }
        exch(toOrder[pivot], toOrder[j]);

        /* Recursive call of the function. */
        quicksort(toOrder, left, j - 1, type);
        quicksort(toOrder, j + 1, right, type);
    }
}

/*-----------------------------Command Funtions-----------------------------*/

/* Adds new task to the system. */
int addTask(Task tasks[], int taskCounter) {
    int duration;
    char description[TASKDESCRIPTION];

    /* Reads input - duration, description. */
    scanf("%d ", &duration);
    readString(description, TASKDESCRIPTION);

    /* Checks errors. */
    if(taskCounter == MAXTASKS) {
        printf(MANYTASKS);
        return taskCounter;
    } else if(taskExists(tasks, taskCounter, description)) {
        printf(DUPLICATEDESCRIPTION);
        return taskCounter;
    } else if(duration <= 0) {
        printf(INVALIDDURATION);
        return taskCounter;
    }

    /* Adds task to the array tasks. */
    tasks[taskCounter].id = taskCounter + 1;
    tasks[taskCounter].predictedDuration = duration;
    strcpy(tasks[taskCounter].description, description);
    strcpy(tasks[taskCounter].activity.name, TODO);
    tasks[taskCounter].start = 0;

    printf("task %d\n", tasks[taskCounter].id); 

    /* Returns incremented number of tasks. */
    return ++taskCounter;
}

/* Lists all tasks (alphabetically ordered) if the command was given without
arguments or specified tasks (by given order) otherwise. */
void listTasks(Task tasks[], int taskCounter) {
    int id, i;
    
    /* Auxiliary array that will be sorted. */   
    Task orderedTasks[MAXTASKS]; 

    /* If no arguments are passed. */
    if(!hasArgs()) {

        /* Copies all elements of array tasks to the auxiliary array. */
        for(i = 0; i < taskCounter; i++) 
            orderedTasks[i] = tasks[i];

        /* Sorts the auxiliary array alphabetically. */
        quicksort(orderedTasks, 0, taskCounter - 1, BYDESCRIPTION);

        for(i = 0; i < taskCounter; i++) {
            printf("%d %s #%d %s\n", orderedTasks[i].id, 
                    orderedTasks[i].activity.name, 
                    orderedTasks[i].predictedDuration, 
                    orderedTasks[i].description);
        }

    /* If arguments are passed. */
    } else {

        /* Reads input - id(s). */
        while(scanf("%d", &id)) {

            /* Checks errors. */
            if(id > taskCounter || id <= 0)  
                printf("%d: "NOTASK, id);
            else 
                printf("%d %s #%d %s\n", id, tasks[id - 1].activity.name, 
                        tasks[id - 1].predictedDuration, 
                        tasks[id - 1].description);
        }
    }
}

/* Advances time in the system. */
int advanceTime(int currentTime) {
    int duration;

    /* Reads input - duration. */
    scanf("%d", &duration);

    /* Checks errors. */
    if(duration < 0) {
        printf(INVALIDTIME);
        return currentTime;
    }

    printf("%d\n", currentTime + duration);
    
    /* Returns new current time. */
    return currentTime + duration;
}

/* Lists all users (by creation time order) if the command was given without
arguments or adds new user to the system otherwise. */
int addUser(User users[], int userCounter) {
    int i;

    /* If no arguments are passed. */
    if(!hasArgs()) {

        for(i = 0; i < userCounter; i++) {
            printf("%s\n", users[i].name);
        }
    
    /* If arguments are passed. */
    } else {
        char user[USERNAME]; 
        
        /* Reads input - user. */
        scanf(" %s", user);
    
        /* Checks errors. */
        if(userExists(users, userCounter, user)) {
            printf(DUPLICATEUSER);
            return userCounter;
        } else if(userCounter == MAXUSERS) {
            printf(MANYUSERS);
            return userCounter;
        }

        /* Adds user to the array users. */
        strcpy(users[userCounter].name, user);
    }

    /* Returns incremented number of users. */
    return ++userCounter;
}

/* Moves a task from an activity to another and if completed calculates
the time it took since it started being done (duration) and the difference
between the duration and the predicted time (slack). */
void moveTask(Task tasks[], User users[], Activity activities[], int taskCounter, 
                int userCounter, int activityCounter, int currentTime) {
    int id;
    char user[USERNAME], activity[ACTIVITYNAME];
    	
    /* Reads input - id, user, activity. */
    scanf("%d %s ", &id, user);
    readString(activity, ACTIVITYNAME);

    /* Checks errors. */
    if(id > taskCounter) {
        printf(NOTASK);
        return;
    } else if (strcmp(activity, tasks[id - 1].activity.name) == 0) {
        return;
    } else if(strcmp(activity, activities[0].name) == 0 ) {
        printf(TASKSTARTED); 
        return;
    } else if(!userExists(users, userCounter, user)) {
        printf(NOUSER);
        return;
    } else if(!activityExists(activities, activityCounter, activity)) {
        printf(NOACTIVITY);
        return;
    }

    /* If task has not started yet keeps the time when it began. */
    if(strcmp(tasks[id - 1].activity.name, activities[0].name) == 0) {
        tasks[id - 1].start = currentTime;
    }

    /* Changes task activity. */
    strcpy(tasks[id - 1].activity.name, activity);          

    /* If task is completed calculates duration and slack. */
    if(strcmp(activity, activities[2].name) == 0) {
        int duration, slack;

        duration = currentTime - tasks[id - 1].start;
        slack = duration - tasks[id - 1].predictedDuration;

        printf("duration=%d slack=%d\n", duration, slack);
    } 
}

/* Lists all tasks that are in a specific activity by ascending order of the
time when it started being done and alphabetically by description if the time 
is the same. */
void listTasksByActivity(Task tasks[], Activity activities[], int taskCounter, 
                            int activityCounter) {
    int i, length = 0;
    char activity[ACTIVITYNAME];

    /* Auxiliary array that will be sorted. */
    Task orderedTasks[MAXTASKS]; 

    /* Reads input - activity. */
    getchar();
    readString(activity, ACTIVITYNAME);

    /* Checks errors. */
    if(!activityExists(activities, activityCounter, activity)) {
        printf(NOACTIVITY);
        return;
    }

    /* Copies all elements of array tasks to the auxiliary array. */
    for(i = 0; i < taskCounter; i++) {
        if(tasks[i].id > 0 && (strcmp(tasks[i].activity.name, activity) == 0)) {
            orderedTasks[length++] = tasks[i];
        }
    }

    /* Sorts the auxiliary array by beginning time. */
    quicksort(orderedTasks, 0, length - 1, BYSTARTTIME);

    for(i = 0; i < length; i++) {
        printf("%d %d %s\n", orderedTasks[i].id, orderedTasks[i].start, 
                orderedTasks[i].description);
    }
}

/* Lists all activities (by creation time order) if the command was given without
arguments or adds new activity to the system otherwise. */
int addActivity(Activity activities[], int activityCounter) {
    int i, length;
    char activity[ACTIVITYNAME];

    /* If no arguments are passed. */
    if(!hasArgs()) {
        for(i = 0; i < activityCounter; i++) {
            printf("%s\n", activities[i].name);
        }

    /* If arguments are passed. */
    } else {

        /* Reads input - activity. */
        readString(activity, ACTIVITYNAME);

        length = strlen(activity);

        /* Checks errors. */
        if(activityExists(activities, activityCounter, activity)) {
            printf(DUPLICATEACTIVITY);
            return activityCounter;
        }

        for(i = 0; i < length; i++) {
            if(activity[i] >= 'a' && activity[i] <= 'z') {
                printf(INVALIDDESCRIPTION);
                return activityCounter;
            }
        }

        if(activityCounter == MAXACTIVITIES) {
            printf(MANYACTIVITIES);
            return activityCounter;
        }

        /* Adds activity to the array activities. */
        strcpy(activities[activityCounter].name, activity);
    } 

    /* Returns incremented number of activities. */
    return ++activityCounter;  
}

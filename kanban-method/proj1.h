/*
 * File: proj1.h
 * Author: Sara Marques
 * Number: 93342
 * Description: Kanban Method - Task management system in a project (Header file).
*/

#include <stdio.h>
#include <string.h>

/*-----------------------------Array Sizes-----------------------------*/

/* Maxium number of tasks in the system. */
#define MAXTASKS 10000

/* Maxium size of a task description. */
#define TASKDESCRIPTION 51

/* Maxium number of activities in the system. */
#define MAXACTIVITIES 10

/* Maxium size of an activity name. */
#define ACTIVITYNAME 21 

/* Maxium number of users in the system. */
#define MAXUSERS 50

/* Maxium size of an user name. */
#define USERNAME 21


/*----------------------------Error Messages----------------------------*/

/* Tasks. */
#define MANYTASKS "too many tasks\n"
#define NOTASK "no such task\n"
#define DUPLICATEDESCRIPTION "duplicate description\n"
#define INVALIDDURATION "invalid duration\n"
#define TASKSTARTED "task already started\n"

/* Users. */
#define MANYUSERS "too many users\n"
#define NOUSER "no such user\n"
#define DUPLICATEUSER "user already exists\n"

/* Activities. */
#define MANYACTIVITIES "too many activities\n"
#define NOACTIVITY "no such activity\n"
#define DUPLICATEACTIVITY "duplicate activity\n"
#define INVALIDDESCRIPTION "invalid description\n"

/* Time. */
#define INVALIDTIME "invalid time\n"


/*------------------------------Quicksort------------------------------*/

/* Comparison type. */
#define BYDESCRIPTION 0
#define BYSTARTTIME 1

/* Abstractions to compare two tasks. */
#define keyA(A) (A.description)
#define keyN(A) (A.start)
#define alphabeticallyOrder(A, B) (strcmp(keyA(A), keyA(B)) <= 0 ? 1 : 0)
#define numericalOrder(A, B) (keyN(A) - keyN(B) == 0 ? alphabeticallyOrder(A, B) : keyN(A) <= keyN(B))
#define less(A, B, T) (T == 0 ? alphabeticallyOrder(A, B) : numericalOrder(A, B))                      

/* Exchanges two tasks */
#define exch(A, B) {Task temp = A; A = B; B = temp;}


/*--------------------------------Others--------------------------------*/

/* Inicial activities on the array activities. */
#define TODO "TO DO"
#define INPROGRESS "IN PROGRESS"
#define DONE "DONE"

/* Returning values depending if certain condition is verified or not. */
#define NO 0
#define YES 1


/*------------------------------Structures------------------------------*/

/* Structure of Activity */
typedef struct {
    char name[ACTIVITYNAME];
} Activity;

/* Structure of User */
typedef struct {
    char name[USERNAME];
} User;

/* Structure of Task */
typedef struct {
    int id;
    char description[TASKDESCRIPTION];
    User user;
    Activity activity;
    int predictedDuration;
    int start;
} Task;

/*--------------------------Function Prtotypes--------------------------*/

void readString(char string[], int maxSize);
int hasArgs();

int taskExists(Task tasks[], int taskCounter, char description[]);
int userExists(User users[], int userCounter, char name[]);
int activityExists(Activity activities[], int activityCounter, char name[]);

void quicksort(Task task[], int left, int right, int comparisonType);

int addTask(Task tasks[], int taskCounter);
void listTasks(Task tasks[], int taskCounter);
int advanceTime(int currentTime);
int addUser(User users[], int userCounter);
void moveTask(Task tasks[], User users[], Activity activities[], int taskCounter, int userCounter, int activityCounter, int currentTime);
void listTasksByActivity(Task tasks[], Activity activities[], int taskCounter, int activityCounter);
int addActivity(Activity activities[], int activityCounter);

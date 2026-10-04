/*
This code uses AI generated code from Chat GPT
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "schedulers.h"
#include "cpu.h"

struct node *head = NULL;
int task_count = 0;

void add(char *name, int priority, int burst)
{
    Task *newTask = malloc(sizeof(Task));

    if (newTask == NULL) {
        fprintf(stderr, "Error allocating memory.\n");
        exit(1);
    }

    newTask->name = strdup(name);
    newTask->priority = priority;
    newTask->burst = burst;
    newTask->tid = 0;

    insert(&head, newTask);

    task_count++;
}

Task *pickNextTask(void)
{
    struct node *current;
    struct node *previous = NULL;

    if (head == NULL) {
        return NULL;
    }

    current = head;

    while (current->next != NULL) {
        previous = current;
        current = current->next;
    }

    return current->task;
}

void removeTask(Task *task)
{
    struct node *current = head;
    struct node *previous = NULL;

    while (current != NULL) {

        if (current->task == task) {

            if (previous == NULL) {
                head = current->next;
            }
            else {
                previous->next = current->next;
            }

            free(current);
            return;
        }

        previous = current;
        current = current->next;
    }
}

void schedule(void)
{
    int current_time = 0;

    double total_turnaround = 0;
    double total_waiting = 0;
    double total_response = 0;

    while (head != NULL) {

        Task *task = pickNextTask();

        int waiting_time = current_time;
        int response_time = current_time;

        printf("Running task = [%s] [%d] [%d]\n",
               task->name,
               task->priority,
               task->burst);

        run(task, task->burst);

        current_time += task->burst;

        int turnaround_time = current_time;

        total_waiting += waiting_time;
        total_response += response_time;
        total_turnaround += turnaround_time;

        removeTask(task);

        free(task->name);
        free(task);
    }

    if (task_count > 0) {
        printf("\nAverage Turnaround Time = %.2f\n",
               total_turnaround / task_count);

        printf("Average Waiting Time = %.2f\n",
               total_waiting / task_count);

        printf("Average Response Time = %.2f\n",
               total_response / task_count);
    }
}
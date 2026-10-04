/*
This code uses AI generated code from Chat GPT
*/
// Shortest-Job-First Scheduler Implementation
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "schedulers.h"
#include "cpu.h"

typedef struct sjfnode {
    Task *task;
    struct sjfnode *next;
} SJFNode;

static SJFNode *head = NULL;
static int task_count = 0;

void add(char *name, int priority, int burst)
{
    Task *t = malloc(sizeof(Task));

    if (t == NULL) {
        fprintf(stderr, "Failed to allocate Task\n");
        exit(1);
    }

    t->name = strdup(name);
    t->priority = priority;
    t->burst = burst;
    t->tid = 0;

    SJFNode *node = malloc(sizeof(SJFNode));

    if (node == NULL) {
        fprintf(stderr, "Failed to allocate SJFNode\n");
        free(t->name);
        free(t);
        exit(1);
    }

    node->task = t;
    node->next = head;
    head = node;

    task_count++;
}

static SJFNode *find_shortest(SJFNode **previous)
{
    if (head == NULL) {
        return NULL;
    }

    SJFNode *shortest = head;
    SJFNode *shortest_previous = NULL;

    SJFNode *current = head;
    SJFNode *current_previous = NULL;

    while (current != NULL) {

        if (current->task->burst < shortest->task->burst) {
            shortest = current;
            shortest_previous = current_previous;
        }

        current_previous = current;
        current = current->next;
    }

    *previous = shortest_previous;

    return shortest;
}

void schedule(void)
{
    if (task_count == 0) {
        printf("No tasks to schedule.\n");
        return;
    }

    int current_time = 0;

    double total_turnaround = 0.0;
    double total_waiting = 0.0;
    double total_response = 0.0;

    while (head != NULL) {

        SJFNode *previous = NULL;
        SJFNode *node = find_shortest(&previous);

        Task *t = node->task;

        int start_time = current_time;

        int waiting_time = start_time;
        int response_time = start_time;

        run(t, t->burst);

        current_time += t->burst;

        int completion_time = current_time;

        int turnaround_time = completion_time;

        total_waiting += waiting_time;
        total_response += response_time;
        total_turnaround += turnaround_time;

        if (previous == NULL) {
            head = node->next;
        }
        else {
            previous->next = node->next;
        }

        free(t->name);
        free(t);
        free(node);
    }

    printf("Average Turnaround Time: %.2f\n",
           total_turnaround / task_count);

    printf("Average Waiting Time: %.2f\n",
           total_waiting / task_count);

    printf("Average Response Time: %.2f\n",
           total_response / task_count);
}
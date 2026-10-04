/*
This code uses AI generated code from Chat GPT
*/
// First-Come, First-Served Scheduler Implementation
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "schedulers.h"
#include "cpu.h"

typedef struct fcfsnode {
    Task *task;
    struct fcfsnode *next;
} FCFSNode;

static FCFSNode *head = NULL;
static FCFSNode *tail = NULL;
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

    FCFSNode *node = malloc(sizeof(FCFSNode));

    if (node == NULL) {
        fprintf(stderr, "Failed to allocate FCFSNode\n");
        free(t->name);
        free(t);
        exit(1);
    }

    node->task = t;
    node->next = NULL;

    if (head == NULL) {
        head = tail = node;
    }
    else {
        tail->next = node;
        tail = node;
    }

    task_count++;
}

static FCFSNode *pop_head(void)
{
    if (head == NULL) {
        return NULL;
    }

    FCFSNode *node = head;

    head = head->next;

    if (head == NULL) {
        tail = NULL;
    }

    node->next = NULL;

    return node;
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

        FCFSNode *node = pop_head();

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
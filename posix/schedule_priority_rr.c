#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "schedulers.h"
#include "cpu.h"

/*
 * Priority scheduling with Round-Robin.
 * Higher numeric values represent higher priority.
 * Tasks with the same priority use Round-Robin scheduling.
 */

typedef struct prrnode {
    Task *task;
    int remaining;
    int started;
    int start_time;
    struct prrnode *next;
} PRRNode;

static PRRNode *head = NULL;
static PRRNode *tail = NULL;
static int task_count = 0;

void add(char *name, int priority, int burst) {
    Task *t = malloc(sizeof(Task));

    if (!t) {
        fprintf(stderr, "Failed to allocate Task\n");
        exit(1);
    }

    t->name = strdup(name);
    t->priority = priority;
    t->burst = burst;
    t->tid = 0;

    PRRNode *node = malloc(sizeof(PRRNode));

    if (!node) {
        fprintf(stderr, "Failed to allocate PRRNode\n");
        exit(1);
    }

    node->task = t;
    node->remaining = burst;
    node->started = 0;
    node->start_time = -1;
    node->next = NULL;

    if (head == NULL) {
        head = tail = node;
    } else {
        tail->next = node;
        tail = node;
    }

    task_count++;
}

static PRRNode *pop_highest_priority(void) {
    if (head == NULL) {
        return NULL;
    }

    PRRNode *best = head;
    PRRNode *best_prev = NULL;

    PRRNode *prev = head;
    PRRNode *current = head->next;

    while (current != NULL) {
        if (current->task->priority > best->task->priority) {
            best = current;
            best_prev = prev;
        }

        prev = current;
        current = current->next;
    }

    if (best_prev == NULL) {
        head = best->next;
    } else {
        best_prev->next = best->next;
    }

    if (best == tail) {
        tail = best_prev;
    }

    best->next = NULL;

    return best;
}

static void push_tail(PRRNode *node) {
    node->next = NULL;

    if (head == NULL) {
        head = tail = node;
    } else {
        tail->next = node;
        tail = node;
    }
}

void schedule() {
    if (task_count == 0) {
        printf("No tasks to schedule.\n");
        return;
    }

    int current_time = 0;
    int completed = 0;

    double total_turnaround = 0.0;
    double total_waiting = 0.0;
    double total_response = 0.0;

    while (completed < task_count) {
        PRRNode *node = pop_highest_priority();

        if (node == NULL) {
            break;
        }

        Task *t = node->task;

        if (!node->started) {
            node->started = 1;
            node->start_time = current_time;
            total_response += node->start_time;
        }

        int slice;

        if (node->remaining > QUANTUM) {
            slice = QUANTUM;
        } else {
            slice = node->remaining;
        }

        run(t, slice);

        node->remaining -= slice;
        current_time += slice;

        if (node->remaining <= 0) {
            int turnaround = current_time;
            int waiting = turnaround - t->burst;

            total_turnaround += turnaround;
            total_waiting += waiting;

            free(t->name);
            free(t);
            free(node);

            completed++;
        } else {
            push_tail(node);
        }
    }

    printf("\nAverage Turnaround Time: %.2f\n",
           total_turnaround / task_count);

    printf("Average Waiting Time: %.2f\n",
           total_waiting / task_count);

    printf("Average Response Time: %.2f\n",
           total_response / task_count);
}
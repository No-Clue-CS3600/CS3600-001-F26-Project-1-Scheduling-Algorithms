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
// the file name is schedule_rr.c, namely because the repository provided used this name
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "schedulers.h"
#include "cpu.h"

/*
 * Round-Robin scheduler using QUANTUM from cpu.h.
 * All tasks arrive at time 0.
 */

typedef struct rrnode {
    Task *task;
    int remaining;
    int started;
    int start_time;
    struct rrnode *next;
} RRNode;

static RRNode *head = NULL;
static RRNode *tail = NULL;
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

    RRNode *n = malloc(sizeof(RRNode));
    if (!n) {
        fprintf(stderr, "Failed to allocate RRNode\n");
        exit(1);
    }
    n->task = t;
    n->remaining = burst;
    n->started = 0;
    n->start_time = -1;
    n->next = NULL;

    if (head == NULL) {
        head = tail = n;
    } else {
        tail->next = n;
        tail = n;
    }
    task_count++;
}

static RRNode *pop_head() {
    if (!head) return NULL;
    RRNode *n = head;
    head = head->next;
    if (!head) tail = NULL;
    n->next = NULL;
    return n;
}

static void push_tail(RRNode *n) {
    n->next = NULL;
    if (!head) {
        head = tail = n;
    } else {
        tail->next = n;
        tail = n;
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

    const int arrival_time = 0;

    while (completed < task_count) {
        RRNode *node = pop_head();
        if (!node) break;

        Task *t = node->task;

        if (!node->started) {
            node->started = 1;
            node->start_time = current_time;
            total_response += (node->start_time - arrival_time);
        }

        int slice = (node->remaining > QUANTUM) ? QUANTUM : node->remaining;

        run(t, slice);
        node->remaining -= slice;
        current_time += slice;

        if (node->remaining <= 0) {
            int completion_time = current_time;
            int turnaround = completion_time - arrival_time;
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

    printf("Average Turnaround Time: %.2f\n", total_turnaround / task_count);
    printf("Average Waiting Time: %.2f\n", total_waiting / task_count);
    printf("Average Response Time: %.2f\n", total_response / task_count);
}

/**
 * Priority scheduling (non-preemptive, ties broken by input order).
 */

#include <stdio.h>
#include <stdlib.h>

#include "task.h"
#include "list.h"
#include "cpu.h"
#include "schedulers.h"

struct node *head = NULL;
int nextTid = 0;

void add(char *name, int priority, int burst) {
    Task *t = malloc(sizeof(Task));
    t->name = name;
    t->tid = nextTid++;
    t->priority = priority;
    t->burst = burst;
    insert(&head, t);
}

Task *pickNextTask() {
    Task *best = head->task;
    struct node *cur = head->next;

    while (cur != NULL) {
        if (cur->task->priority >= best->priority)
            best = cur->task;
        cur = cur->next;
    }
    return best;
}

void schedule() {
    int time = 0;
    int count = 0;
    int totalTurnaround = 0, totalWaiting = 0, totalResponse = 0;

    while (head != NULL) {
        Task *t = pickNextTask();
        run(t, t->burst);

        totalResponse += time;
        totalWaiting += time;
        time += t->burst;
        totalTurnaround += time;
        count++;

        delete(&head, t);
        free(t);
    }

    printf("\nAverage turnaround time = %.2f\n", (double) totalTurnaround / count);
    printf("Average waiting time    = %.2f\n", (double) totalWaiting / count);
    printf("Average response time   = %.2f\n", (double) totalResponse / count);
}

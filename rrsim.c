#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedlist.h"

#define MAX_BUFFER_LEN 80

taskval_t *event_list = NULL;
taskval_t *event_list_2 = NULL;
enum State { IDLE, DISPATCHING, EXITING, RUNNING };

void print_task(taskval_t *t, void *arg) {
  printf("task %03d: %5d %3.2f %3.2f\n", t->id, t->arrival_time, t->cpu_request,
         t->cpu_used);
}

void increment_count(taskval_t *t, void *arg) {
  int *ip;
  ip = (int *)arg;
  (*ip)++;
}

void print_sim(taskval_t *t, enum State state, float tick, FILE *stream) {
  fprintf(stream, "[%f] ", tick);
  switch (state) {
    case IDLE:
      fprintf(stream, "IDLE\n");
      break;
    case DISPATCHING:
      fprintf(stream, "DISPATCHING\n");
      break;
    case RUNNING:
      fprintf(stream, "id=%05d req=%3.2f used=%3.2f\n", t->id, t->cpu_request,
              t->cpu_used);
      break;
    case EXITING:
      fprintf(stream, "id=%05d EXIT w=%3.2f ta=%3.2f\n", t->id,
              tick - ((t->arrival_time) + (t->cpu_request)),
              tick - (t->arrival_time));
      break;
    default:
      fprintf(stream, "Error invalid state");
  }
}

int done(taskval_t *t, int quantum, int qlen) {
  float temp = t->cpu_request - t->cpu_used;
  if (quantum > qlen) {
    return 2;
  }
  if (temp <= 0) {
    return 1;
  } else {
    return 0;
  }
}

void run_first_come_first_serve(int dlen) {
  taskval_t *curr;
  float tick = 0;
  int dispatch = 0;
  char buf[100];
  sprintf(buf, "./data/first_come/dispatch_%d.txt", dlen);
  FILE *fp = fopen(buf, "w");

  while ((curr = peek_front(event_list_2))) {
    if (curr != NULL && curr->arrival_time > tick) {
      print_sim(curr, IDLE, tick, fp);
      tick++;
      continue;
    }
    if (dispatch < dlen) {
      print_sim(curr, DISPATCHING, tick, fp);
      tick++;
      dispatch++;
      continue;
    }
    float temp = curr->cpu_request - curr->cpu_used;
    if (temp <= 0) {
      print_sim(curr, EXITING, tick, fp);
      end_task(curr);
      event_list_2 = remove_front(event_list_2);
      dispatch = 0;
    } else {
      print_sim(curr, RUNNING, tick, fp);
      curr->cpu_used++;
      tick++;
    }
  }
}

void run_simulation(int qlen, int dlen) {
  taskval_t *curr;
  taskval_t *ready_queue = NULL;
  int quantum = 1;
  float tick = 0;
  int dispatch = 0;
  int finished = 0;
  // Used for data generation
  // char buf[100];
  // sprintf(buf, "./data/round_robin/quantum_%d_dispatch_%d.txt", qlen, dlen);
  // FILE *fp = fopen(buf, "w");

  while ((curr = peek_front(event_list)) || peek_front(ready_queue)) {
    if (curr != NULL && curr->arrival_time <= tick) {
      event_list = remove_front(event_list);
      ready_queue = add_end(ready_queue, curr);
    }
    curr = peek_front(ready_queue);
    if (curr == NULL) {
      print_sim(curr, IDLE, tick, stdout);
      tick++;
      continue;
    }
    if (dispatch < dlen) {
      print_sim(curr, DISPATCHING, tick, stdout);
      tick++;
      dispatch++;
      continue;
    }
    finished = done(curr, quantum, qlen);
    if (!finished) {
      print_sim(curr, RUNNING, tick, stdout);
      curr->cpu_used++;
      quantum++;
      tick++;
      continue;
    }

    if (finished == 1) {
      print_sim(curr, EXITING, tick, stdout);
      end_task(curr);
      ready_queue = remove_front(ready_queue);
    } else {
      ready_queue = remove_front(ready_queue);
      ready_queue = add_end(ready_queue, curr);
    }
    dispatch = 0;
    quantum = 1;
  }
}

int main(int argc, char *argv[]) {
  char input_line[MAX_BUFFER_LEN];
  int i;
  int task_num;
  int task_arrival;
  float task_cpu;
  int quantum_length = -1;
  int dispatch_length = -1;

  taskval_t *temp_task;

  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--quantum") == 0 && i + 1 < argc) {
      quantum_length = atoi(argv[i + 1]);
    } else if (strcmp(argv[i], "--dispatch") == 0 && i + 1 < argc) {
      dispatch_length = atoi(argv[i + 1]);
    }
  }

  if (quantum_length == -1 || dispatch_length == -1) {
    fprintf(stderr, "usage: %s --quantum <num> --dispatch <num>\n", argv[0]);
    exit(1);
  }

  while (fgets(input_line, MAX_BUFFER_LEN, stdin)) {
    sscanf(input_line, "%d %d %f", &task_num, &task_arrival, &task_cpu);
    temp_task = new_task();
    temp_task->id = task_num;
    temp_task->arrival_time = task_arrival;
    temp_task->cpu_request = task_cpu;
    temp_task->cpu_used = 0.0;
    event_list = add_end(event_list, temp_task);
    temp_task = new_task();
    temp_task->id = task_num;
    temp_task->arrival_time = task_arrival;
    temp_task->cpu_request = task_cpu;
    temp_task->cpu_used = 0.0;
    event_list_2 = add_end(event_list_2, temp_task);
  }
  run_simulation(quantum_length, dispatch_length);
  run_first_come_first_serve(dispatch_length);

  return (0);
}

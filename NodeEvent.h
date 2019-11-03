#ifndef HOMEWORK_NODEEVENT_H
#define HOMEWORK_NODEEVENT_H

#include "Node.h"

typedef struct Event {
    Node *node;
} Event;

typedef struct EventQueue {
    Event *queue;
    size_t count;
    size_t capacity;
} EventQueue;

EventQueue evqueue_create();

void evqueue_push(EventQueue *queue, Event event);

Event evqueue_pop(EventQueue *queue);

void evqueue_free(EventQueue *queue);

#endif //HOMEWORK_NODEEVENT_H
#include "NodeEvent.h"
#include "debugmalloc.h"

EventQueue evqueue_create() {
    EventQueue queue;
    queue.capacity = 1;
    queue.count = 0;
    queue.queue = (Event *)malloc(sizeof(Event));
    return queue;
}

void evqueue_push(EventQueue *queue, Event event) {
    /*for (size_t i = 0; i < queue->count; i++)
        if (queue->queue[i].node == event.node) {
            for (size_t j = i + 1; j < queue->count; j++)
                queue[j - 1] = queue[j];
            queue->count--;
            break;
        }*/
    if (queue->capacity <= queue->count) {
        queue->capacity *= 2;
        Event *newMem = (Event *)realloc(queue->queue, sizeof(Event) * queue->capacity);
        if (!newMem) {
            log_error("Couldn't allocate array!\n");
            return;
        }
        queue->queue = newMem;
    }
    queue->queue[queue->count] = event;
    queue->count++;
}

Event evqueue_pop(EventQueue *queue) {
    Event head = queue->queue[0];
    for (int i = 1; i < queue->count; i++)
        queue->queue[i - 1] = queue->queue[i];
    queue->count--;
    return head;
}

void evqueue_free(EventQueue *queue) {
    free(queue->queue);
}
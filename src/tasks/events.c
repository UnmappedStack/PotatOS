#include "../utils/include/printf.h"
#include "include/events.h"
#include "../mem/include/vector.h"
#include "../mem/include/kheap.h"

EventQueue new_event_queue() {
    Vector *list = new_vector(sizeof(Event));
    EventQueue to_return;
    to_return.queue = list;
    return to_return;
}

void add_event(EventQueue q, uint64_t event_type, uint64_t data0, uint64_t data1) {
    Event new_event = (Event) {
        .do_handle = true,
        .event_id  = event_type,
        .data0     = data0,
        .data1     = data1
    };
    Event *event_addr = (Event*) malloc(sizeof(Event));
    *event_addr = new_event;
    vector_push(q.queue, (uintptr_t) event_addr);
}

Event* poll(EventQueue q) {
    if ((q.queue)->length == 0) {
        Event *to_return = (Event*) malloc(sizeof(Event));
        *to_return = (Event) {
            .do_handle = false
        };
        return to_return;
    } else {
        Event *this_event = (Event*) vector_at(q.queue, (q.queue)->length - 1);
        vector_pop(q.queue, (q.queue)->length - 1);
        return this_event;
    }
}

Event* peek(EventQueue q) {
    if ((q.queue)->length == 0) {
        Event *to_return = (Event*) malloc(sizeof(Event));
        *to_return = (Event) {
            .do_handle = false
        };
        return to_return;
    } else {
        Event *this_event = (Event*) vector_at(q.queue, (q.queue)->length - 1);
        return this_event;
    }
}

void cleanup_event(Event *event) {
    free(event);
}
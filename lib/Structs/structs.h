#include <stdint.h>

#ifndef INCLUDE_STRUCTS_H
#define INCLUDE_STRUCTS_H

// Struct for the current status of the program
struct Status {
    int cap;
    int rejected;
};

// Just an enum for a type of buttonpress
enum EventType {
    None = 255,
    Enter = 1,
    Leave = 2,
    Reject = 3
};

// A custom datatype that stores a button press
struct Event {
    enum EventType eventType;
    uint64_t timestamp;
};

#endif

#include "Wire.h"
#include "debugmalloc.h"

Wire wire_create(Node *origin, int originPin) {
    Wire wire;
    wire.origin = origin;
    wire.originPin = originPin;
    wire.conCap = 1;
    wire.conCount = 0;
    wire.connections = (Connection *)malloc(sizeof(Connection));
    return wire;
}

void wire_add(Wire *wire, Node *other, int otherPin) {
    if (wire->conCap <= wire->conCount) {
        wire->conCap *= 2;
        Connection *newCon = (Connection *)realloc(wire->connections, sizeof(Connection) * wire->conCap);
        if (!newCon) {
            log_error("Couldn't allocate new memory!\n");
            return;
        }
        wire->connections = newCon;
    }
    wire->connections[wire->conCount].dest = other;
    wire->connections[wire->conCount].pin = otherPin;
    wire->connections[wire->conCount].wireComp = component_create_wire_between(
        &wire->origin->component,
        &other->component,
        wire->originPin + wire->origin->component.funData.inC,
        otherPin,
        COMPSIZE,
        COMPTHICKNESS,
        wire->origin->renderer
    );
    wire->conCount++;
}

void wire_erase(Wire *wire, int id) {
    for (int i = id + 1; id < wire->conCount; i++)
        wire->connections[i - 1] = wire->connections[i];
    wire->conCount--;
}

void wire_send_value(Wire *wire, bool value) {
    for (size_t i = 0; i < wire->conCount; i++)
        node_set_inval(wire->connections[i].dest, wire->connections[i].pin, value);
}

void wire_free(Wire *wire) {
    free(wire->connections);
}
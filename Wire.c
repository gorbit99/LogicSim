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

void wire_add(Wire *wire, int other, int otherPin, Node *nodes) {
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
        &nodes[other].component,
        wire->originPin + wire->origin->component.funData.inC,
        otherPin,
        COMPSIZE,
        COMPTHICKNESS,
        wire->origin->renderer
    );
    wire->conCount++;
}

void wire_erase(Wire *wire, int id) {
    for (size_t i = id + 1; i < wire->conCount; i++)
        wire->connections[i - 1] = wire->connections[i];
    wire->conCount--;
}

void wire_send_value(Wire *wire, bool value, Node *nodes) {
    for (size_t i = 0; i < wire->conCount; i++)
        node_set_inval(&nodes[wire->connections[i].dest], wire->connections[i].pin, value);
}

void wire_free(Wire *wire) {
    free(wire->connections);
}

void connection_reposition(Connection *conn, Node *origin, int originPin, Node *dest) {
    component_free_data(&conn->wireComp);
    conn->wireComp = component_create_wire_between(
            &origin->component,
            &dest->component,
            originPin + origin->component.funData.inC,
            conn->pin,
            COMPSIZE,
            COMPTHICKNESS,
            origin->renderer
    );
}

void wire_reposition(Wire *wire, Node *nodes) {
	for (size_t i = 0; i < wire->conCount; i++) {
        connection_reposition(&wire->connections[i],
            wire->origin,
            wire->originPin,
            &nodes[wire->connections[i].dest]);
	}
}

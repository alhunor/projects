#include <queue>
#include "ipc.h"

// do nothing functions - for later use in paralllel execution
MUTEX createMutex(char* name, bool owned) { return 0;}
void removeMutex(MUTEX m) {}
void getMutex(MUTEX m) {}
void releaseMutex(MUTEX m) {}


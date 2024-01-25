#include "Globals.h"

Application app = {
    .zoom_rate = { 1.0, 1.0 },
    .win = NULL,
    .ren = NULL,
    .font = NULL,
    .is_running = 1,
    .is_error = 0,
    .is_loaded = 0,
    // .key_status = NULL
    .mutex = NULL
};
#include "datacontainer.h"
#include "common.h"

void DataContainer::addStep(struct step_val *sv, bool &redo, bool &undo) {
    int ebt_fidx = sv->e.y * 4 + sv->e.x;
    int vbt_fidx = sv->v.y * 4 + sv->v.x;
    steps[cur_idx++] = (unsigned char) (ebt_fidx * 16 + vbt_fidx);
    last_idx = cur_idx;
    redo = false;
    undo = cur_idx > 0;
};

static void extract_button_bidxs(unsigned char val, struct step_val *sv) {
    int ebt_fidx = val / 16;
    int vbt_fidx = val % 16;
    sv->e.x = ebt_fidx % 4;
    sv->e.y = ebt_fidx / 4;
    sv->v.x = vbt_fidx % 4;
    sv->v.y = vbt_fidx / 4;
}

void DataContainer::undoStep(int &ebt_x, int &ebt_y, bool &redo, bool &undo) {
    if (cur_idx == 0) return;
    cur_idx--;
    int val = (int) steps[cur_idx];
    struct step_val sv;
    extract_button_bidxs(val, &sv);
    ebt_x = sv.e.x;
    ebt_y = sv.e.y;
    redo = cur_idx < last_idx;
    undo = cur_idx > 0;
}

void DataContainer::redoStep(int &vbt_x, int &vbt_y, bool &redo, bool &undo) {
    if (cur_idx == last_idx) return;
    int val = (int) steps[cur_idx];
    struct step_val sv;
    extract_button_bidxs(val, &sv);
    vbt_x = sv.v.x;
    vbt_y = sv.v.y;
    cur_idx++;
    redo = cur_idx < last_idx;
    undo = cur_idx > 0;
}

time_t DataContainer::getSessionTime() {
    time_t c_time = time(0);
    return c_time - t_start;
}

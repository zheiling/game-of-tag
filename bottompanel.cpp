#include "bottompanel.h"
#include "common.h"
#include <cstdio>
#include <time.h>
#include "gamefield.h"

BottomPanel::BottomPanel(GameField *a_gfptr) : gf_ptr(a_gfptr) {
    ptr = new Fl_Group(spacing, tp_h+gf_h, win_w-spacing*2, bp_text_size+spacing*2, 0);
    ptr->box(FL_BORDER_BOX);
    updateText();
    b_text_ptr = new Fl_Box(spacing*2, tp_h+gf_h+spacing, win_w-spacing*4, bp_text_size, f_text);
    b_text_ptr->labelsize(bp_text_size);
    b_text_ptr->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    b_text_ptr->labelfont(FL_COURIER);
    ptr->end();
}

void BottomPanel::updateText() {
    time_t time_spent = gf_ptr->getSessionTime();
    struct tm ts;
    gmtime_r(&time_spent, &ts);
    strftime(t_text, 30, "Time spent: %H:%M:%S", &ts);
    sprintf(f_text, "Steps made: %d. Current: %d. %s", gf_ptr->getStepsCount(), gf_ptr->getCurrentStep(), t_text);
}

#include "bottompanel.h"
#include "common.h"
#include <cstdio>
#include <time.h>
#include "gamefield.h"

BottomPanel::BottomPanel(GameField *a_gfptr, DataContainer *a_dptr) : gf(a_gfptr), data(a_dptr) {
    ptr = new Fl_Group(spacing, tp_h+gf_h, win_w-spacing*2, bp_text_size+spacing*2, 0);
    ptr->box(FL_BORDER_BOX);
    updateText();
    ui_widget = new Fl_Box(spacing*2, tp_h+gf_h+spacing, win_w-spacing*4, bp_text_size, f_text);
    ui_widget->labelsize(bp_text_size);
    ui_widget->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    ui_widget->labelfont(FL_COURIER);
    ptr->end();
}

void BottomPanel::updateText() {
    time_t time_spent = data->getSessionTime();
    struct tm ts;
    gmtime_r(&time_spent, &ts);
    strftime(t_text, 30, "Time spent: %H:%M:%S", &ts);
    sprintf(f_text, "Steps made: %d. Current: %d. %s", data->getStepsCount(), data->getCurrentStep(), t_text);
}

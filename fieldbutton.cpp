#include "fieldbutton.h"
#include "toppanel.h"
#include "common.h"
#include <FL/fl_ask.H>
#include <cstdio>

FieldButton::FieldButton(int a_x, int a_y, int a_v, GameField *a_gptr) : x(a_x), y(a_y), v(a_v), g_ptr(a_gptr) {
    b_ptr = new Fl_Button((y+1)*spacing + y*m_but, tp_h + (x+1)*spacing + x*m_but, m_but, m_but);
    setValue(this, v, true);
    b_ptr->labelsize(m_label_size);
    b_ptr->labelcolor(FL_BLUE);
    b_ptr->callback(pressCallback, (void *)this);
    if (v == -1) {
        hide();
    }
}

FieldButton::~FieldButton() {
    delete b_ptr;
}

void FieldButton::setValue(FieldButton* fb, int v, bool matrix_sync) {
    char b_label[3];
    sprintf(b_label, "%d", v);
    fb->b_ptr->copy_label(b_label);
    fb->v = v;
    if (matrix_sync) {
        fb->g_ptr->val_matrix[fb->x][fb->y] = v;
    }
    if (v == -1) {
        fb->g_ptr->setEmptyPos(fb->x, fb->y);
    }
}

 void FieldButton::processWinCase(FieldButton *fb) {
    char text[100];
    char t_text[9];
    time_t time_spent = fb->g_ptr->getSessionTime();
    struct tm ts;
    gmtime_r(&time_spent, &ts);
    strftime(t_text, 9, "%H:%M:%S", &ts);
    sprintf(text, "You win!\nTime spent: %s\nSteps made: %d\nDo you want to start a new game?", t_text, fb->g_ptr->getStepsCount());
    switch(fl_choice_n(text, "Exit", "Start (new seed)", "Start (same seed)")) {
    case 2: {
        fb->g_ptr->genValPositions();
        fb->g_ptr->syncButtons();
        fb->g_ptr->resetSteps();
        fb->g_ptr->top_panel->updateDoButtonStatuses(false, false);
        fb->g_ptr->updateBottomText();
        return;
    }
    case 1: {
        fb->g_ptr->genNewSeed();
        fb->g_ptr->genValPositions();
        fb->g_ptr->syncButtons();
        fb->g_ptr->resetSteps();
        fb->g_ptr->top_panel->updateDoButtonStatuses(false, false);
        fb->g_ptr->updateBottomText();
        return;
    }
    default:
        exit(0);
    }
}

void FieldButton::pressCallback(Fl_Widget *b, void *user) {
    FieldButton *fb = (FieldButton*) user;
    struct step_val sv;
    fb->g_ptr->getEmptyPosition(sv.e.x, sv.e.y);
    sv.v.x = fb->x;
    sv.v.y = fb->y;
    if (fb->g_ptr->swapWithEmpty(fb)) {
        bool redo, undo;
        fb->g_ptr->data->addStep(&sv, redo, undo);
        if (fb->g_ptr->checkEndGame()) {
            processWinCase(fb);
        }
        fb->g_ptr->top_panel->updateDoButtonStatuses(redo, undo);
    }
    fb->g_ptr->updateBottomText();
}

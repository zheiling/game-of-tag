#include "fieldbutton.h"
#include "toppanel.h"
#include "common.h"
#include <FL/fl_ask.H>
#include <cstdio>

FieldButton::FieldButton(int a_x, int a_y, int a_v, GameField *a_gptr) : x(a_x), y(a_y), v(a_v), gf(a_gptr) {
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
        fb->gf->val_matrix[fb->x][fb->y] = v;
    }
    if (v == -1) {
        fb->gf->setEmptyPos(fb->x, fb->y);
    }
}

 void FieldButton::processWinCase(FieldButton *fb) {
    char text[100];
    char t_text[9];
    time_t time_spent = fb->gf->data->getSessionTime();
    struct tm ts;
    gmtime_r(&time_spent, &ts);
    strftime(t_text, 9, "%H:%M:%S", &ts);
    sprintf(text, "You win!\nTime spent: %s\nSteps made: %d\nDo you want to start a new game?", t_text, fb->gf->data->getStepsCount());
    switch(fl_choice_n(text, "Exit", "Start (new seed)", "Start (same seed)")) {
    case 2: {
        fb->gf->genValPositions();
        fb->gf->syncButtons();
        fb->gf->resetSteps();
        fb->gf->top_panel->updateDoButtonStatuses(false, false);
        fb->gf->updateBottomText();
        return;
    }
    case 1: {
        fb->gf->data->genNewSeed();
        fb->gf->genValPositions();
        fb->gf->syncButtons();
        fb->gf->resetSteps();
        fb->gf->top_panel->updateDoButtonStatuses(false, false);
        fb->gf->updateBottomText();
        return;
    }
    default:
        exit(0);
    }
}

void FieldButton::pressCallback(Fl_Widget *b, void *user) {
    FieldButton *fb = (FieldButton*) user;
    struct step_val sv;
    fb->gf->getEmptyPosition(sv.e.x, sv.e.y);
    sv.v.x = fb->x;
    sv.v.y = fb->y;
    if (fb->gf->swapWithEmpty(fb)) {
        bool redo, undo;
        fb->gf->data->addStep(&sv, redo, undo);
        if (fb->gf->checkEndGame()) {
            processWinCase(fb);
        }
        fb->gf->top_panel->updateDoButtonStatuses(redo, undo);
    }
    fb->gf->updateBottomText();
}

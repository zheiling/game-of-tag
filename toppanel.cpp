#include "toppanel.h"
#include "common.h"
#include <FL/Fl_Window.H>
#include <FL/fl_ask.H>
#include <cstring>

void TopPanel::newCallback(Fl_Widget *w, void *user) {
    TopPanel *tp = (TopPanel*) user;
    const char *seed_input = fl_input("Enter seed:");
    int seed;
    if (!strlen(seed_input)) {
        seed = tp->gf_ptr->genNewSeed();
    } else {
        seed = atoi(seed_input);
    }
    tp->gf_ptr->genValPositions(seed);
    tp->gf_ptr->syncButtons();
    tp->gf_ptr->resetSteps();
    tp->updateDoButtonStatuses(false, false);
    tp->gf_ptr->updateBottomText();
};

void TopPanel::redoCallback(Fl_Widget *w, void *user) {
    TopPanel *tp = (TopPanel*) user;
    bool redo, undo;
    tp->gf_ptr->redoStep(redo, undo);
    tp->updateDoButtonStatuses(redo, undo);
    tp->gf_ptr->updateBottomText();
}

void TopPanel::undoCallback(Fl_Widget *w, void *user) {
    TopPanel *tp = (TopPanel*) user;
    bool redo, undo;
    tp->gf_ptr->undoStep(redo, undo);
    tp->updateDoButtonStatuses(redo, undo);
    tp->gf_ptr->updateBottomText();
}

void TopPanel::reloadCallback(Fl_Widget *w, void *user) {
    TopPanel *tp = (TopPanel*) user;
    tp->gf_ptr->genValPositions(tp->gf_ptr->getCurrentSeed());
    tp->gf_ptr->syncButtons();
    tp->gf_ptr->resetSteps();
    tp->updateDoButtonStatuses(false, false);
    tp->gf_ptr->updateBottomText();
}

TopPanel::TopPanel(GameField *a_gfptr) : gf_ptr(a_gfptr) {
    w_ptr = new Fl_Group(spacing, spacing, win_w - spacing*2, t_size + spacing*2, 0);
    w_ptr->box(FL_BORDER_BOX);

    // new button:
    buttons.b_new = new Fl_Button(spacing*2, spacing*2, t_size, t_size, "@refresh");
    buttons.b_new->labelsize(tp_l_size);
    buttons.b_new->labelcolor(FL_CYAN);
    buttons.b_new->callback(newCallback, (void*)this);
    buttons.b_new->tooltip("Generate new game");

    // save button:
    buttons.b_save = new Fl_Button(spacing*3 + t_size, spacing*2, t_size, t_size, "@filesave");
    buttons.b_save->labelsize(tp_l_size);
    buttons.b_save->tooltip("Save game");

    // undo button:
    buttons.b_undo = new Fl_Button(spacing*4 + t_size*3, spacing*2, t_size, t_size, "@undo");
    buttons.b_undo->labelsize(tp_l_size);
    buttons.b_undo->labelcolor(FL_CYAN);
    buttons.b_undo->tooltip("Undo");
    buttons.b_undo->deactivate();
    buttons.b_undo->callback(undoCallback, (void *) this);

    // redo button:
    buttons.b_redo = new Fl_Button(spacing*5 + t_size*4, spacing*2, t_size, t_size, "@redo");
    buttons.b_redo->labelsize(tp_l_size);
    buttons.b_redo->labelcolor(FL_CYAN);
    buttons.b_redo->tooltip("Redo");
    buttons.b_redo->deactivate();
    buttons.b_redo->callback(redoCallback, (void *) this);

    // reload button:
    buttons.b_reload = new Fl_Button(spacing*7 + t_size*6, spacing*2, t_size, t_size, "@reload");
    buttons.b_reload->labelsize(tp_l_size);
    buttons.b_reload->labelcolor(FL_DARK_CYAN);
    buttons.b_reload->tooltip("Reload with the same seed");
    buttons.b_reload->callback(reloadCallback, (void *) this);
    w_ptr->end();
}

void TopPanel::updateDoButtonStatuses(bool redo, bool undo) {
    if (redo && !buttons.b_redo->active()) buttons.b_redo->activate();
    if (!redo && buttons.b_redo->active()) buttons.b_redo->deactivate();
    //
    if (undo && !buttons.b_undo->active()) buttons.b_undo->activate();
    if (!undo && buttons.b_undo->active()) buttons.b_undo->deactivate();
}

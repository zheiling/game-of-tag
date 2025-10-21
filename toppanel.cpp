#include "toppanel.h"
#include "common.h"
#include <FL/Fl_Window.H>
#include <FL/fl_ask.H>
#include <cstring>

void TopPanel::newCallback(Fl_Widget *w, void *user) {
    TopPanel *tp = (TopPanel*) user;
    const char *seed_input = fl_input("Enter seed:");
    if (!seed_input) return;
    int seed;
    if (!strlen(seed_input)) {
        seed = tp->data->genNewSeed();
    } else {
        seed = atoi(seed_input);
    }
    tp->data->genValPositions(seed);
    tp->gf->syncButtons();
    tp->gf->resetSteps();
    tp->updateDoButtonStatuses(false, false);
    tp->gf->updateBottomText();
};

void TopPanel::redoCallback(Fl_Widget *w, void *user) {
    TopPanel *tp = (TopPanel*) user;
    bool redo, undo;
    tp->gf->redoStep(redo, undo);
    tp->updateDoButtonStatuses(redo, undo);
    tp->gf->updateBottomText();
}

void TopPanel::openSessionCallback(Fl_Widget *w, void *user) {
    TopPanel *tp = (TopPanel*) user;
    tp->data->openSessionFile(tp->gf);
    int &last_idx = tp->data->last_idx;
    int &cur_idx = tp->data->cur_idx;
    tp->updateDoButtonStatuses(last_idx > cur_idx, cur_idx > 0);
    tp->gf->updateBottomText();
}

void TopPanel::saveSessionCallback(Fl_Widget *w, void *user) {
    TopPanel *tp = (TopPanel*) user;
    tp->data->saveSessionFile();
}

void TopPanel::undoCallback(Fl_Widget *w, void *user) {
    TopPanel *tp = (TopPanel*) user;
    bool redo, undo;
    tp->gf->undoStep(redo, undo);
    tp->updateDoButtonStatuses(redo, undo);
    tp->gf->updateBottomText();
}

void TopPanel::reloadCallback(Fl_Widget *w, void *user) {
    TopPanel *tp = (TopPanel*) user;
    tp->data->genValPositions(tp->data->getCurrentSeed());
    tp->gf->syncButtons();
    tp->gf->resetSteps();
    tp->updateDoButtonStatuses(false, false);
    tp->gf->updateBottomText();
}

TopPanel::TopPanel(GameField *a_gfptr, DataContainer *a_data) : gf(a_gfptr), data(a_data) {
    ui_widget = new Fl_Group(spacing, spacing, win_w - spacing*2, t_size + spacing*2, 0);
    ui_widget->box(FL_BORDER_BOX);

    // new button:
    buttons.b_new = new Fl_Button(spacing*2, spacing*2, t_size, t_size, "@refresh");
    buttons.b_new->labelsize(tp_l_size);
    buttons.b_new->labelcolor(FL_CYAN);
    buttons.b_new->callback(newCallback, (void*)this);
    buttons.b_new->tooltip("Generate new game");

    // Open session button:
    buttons.b_open = new Fl_Button(spacing*3 + t_size, spacing*2, t_size, t_size, "@fileopen");
    buttons.b_open->labelsize(tp_l_size);
    buttons.b_open->tooltip("Open session");
    buttons.b_open->callback(openSessionCallback, (void *) this);

    // Save session button:
    buttons.b_save = new Fl_Button(spacing*4 + t_size*2, spacing*2, t_size, t_size, "@filesave");
    buttons.b_save->labelsize(tp_l_size);
    buttons.b_save->tooltip("Save game");
    buttons.b_save->callback(saveSessionCallback, (void *) this);

    // undo button:
    buttons.b_undo = new Fl_Button(spacing*5 + t_size*4, spacing*2, t_size, t_size, "@undo");
    buttons.b_undo->labelsize(tp_l_size);
    buttons.b_undo->labelcolor(FL_CYAN);
    buttons.b_undo->tooltip("Undo");
    buttons.b_undo->deactivate();
    buttons.b_undo->callback(undoCallback, (void *) this);

    // redo button:
    buttons.b_redo = new Fl_Button(spacing*6 + t_size*5, spacing*2, t_size, t_size, "@redo");
    buttons.b_redo->labelsize(tp_l_size);
    buttons.b_redo->labelcolor(FL_CYAN);
    buttons.b_redo->tooltip("Redo");
    buttons.b_redo->deactivate();
    buttons.b_redo->callback(redoCallback, (void *) this);

    // reload button:
    buttons.b_reload = new Fl_Button(spacing*8 + t_size*7, spacing*2, t_size, t_size, "@reload");
    buttons.b_reload->labelsize(tp_l_size);
    buttons.b_reload->labelcolor(FL_DARK_CYAN);
    buttons.b_reload->tooltip("Reload with the same seed");
    buttons.b_reload->callback(reloadCallback, (void *) this);
    ui_widget->end();
}

void TopPanel::updateDoButtonStatuses(bool redo, bool undo) {
    if (redo && !buttons.b_redo->active()) buttons.b_redo->activate();
    if (!redo && buttons.b_redo->active()) buttons.b_redo->deactivate();
    //
    if (undo && !buttons.b_undo->active()) buttons.b_undo->activate();
    if (!undo && buttons.b_undo->active()) buttons.b_undo->deactivate();
}

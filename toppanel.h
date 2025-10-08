#ifndef TOPPANEL_H
#define TOPPANEL_H

#include "gamefield.h"
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>

class TopPanel
{
    struct {
        Fl_Button *b_new;
        Fl_Button *b_save;
        Fl_Button *b_undo;
        Fl_Button *b_redo;
        Fl_Button *b_reload;
    } buttons;
    GameField *gf_ptr;
    Fl_Group *w_ptr;
    static void newCallback(Fl_Widget *w, void *user);
    static void undoCallback(Fl_Widget *w, void *user);
    static void redoCallback(Fl_Widget *w, void *user);
    static void reloadCallback(Fl_Widget *w, void *user);
public:
    TopPanel(GameField *a_gfptr);
    void updateDoButtonStatuses(bool redo, bool undo);
};

#endif // TOPPANEL_H

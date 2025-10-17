#ifndef TOPPANEL_H
#define TOPPANEL_H

#include "gamefield.h"
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>

class TopPanel
{
    struct {
        Fl_Button *b_new;
        Fl_Button *b_open;
        Fl_Button *b_save;
        Fl_Button *b_undo;
        Fl_Button *b_redo;
        Fl_Button *b_reload;
    } buttons;
    GameField *gf;
    Fl_Group *ui_widget;
    DataContainer *data;
    static void newCallback(Fl_Widget *w, void *user);
    static void undoCallback(Fl_Widget *w, void *user);
    static void redoCallback(Fl_Widget *w, void *user);
    static void reloadCallback(Fl_Widget *w, void *user);
    static void openSessionCallback(Fl_Widget *w, void *user);
    static void saveSessionCallback(Fl_Widget *w, void *user);
public:
    TopPanel(GameField *a_gfptr, DataContainer *d_ptr);
    void updateDoButtonStatuses(bool redo, bool undo);
};

#endif // TOPPANEL_H

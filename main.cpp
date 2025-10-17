#include "bottompanel.h"
#include "common.h"
#include "gamefield.h"
#include "toppanel.h"
#include <stdlib.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/fl_ask.H>

int main(int argc, char **argv)
{
    Fl::args(argc, argv);
    int win_h = tp_h + gf_h + bp_h;
    Fl_Window *win = new Fl_Window(win_w, win_h, "Piatnashki");
    DataContainer *data = new DataContainer();
    GameField *gf = new GameField(data);
    TopPanel *tp = new TopPanel(gf, data);
    BottomPanel *bp = new BottomPanel(gf, data);
    gf->setTopPanel(tp);
    gf->setBottomPanel(bp);
    win->end();
    win->show();
    return Fl::run();
}

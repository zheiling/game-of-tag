#ifndef FIELDBUTTON_H
#define FIELDBUTTON_H

#include "gamefield.h"
#include <FL/Fl_Button.H>
class FieldButton {
    friend class GameField;
    int x, y, v;
    GameField *gf;
    DataContainer *data;
    Fl_Button* b_ptr;
    FieldButton(int a_x, int a_y, int a_v, GameField *a_gptr);
    ~FieldButton();
    static void pressCallback(Fl_Widget *, void *);
    static void setValue(FieldButton* fb, int v, bool matrix_sync);
    static void processWinCase(FieldButton* fb);
public:
    void show() {b_ptr->show();}
    void hide() {b_ptr->hide();}
};


#endif // FIELDBUTTON_H

#ifndef BOTTOMPANEL_H
#define BOTTOMPANEL_H

#include <FL/Fl_Box.H>
#include <FL/Fl_Window.H>
class GameField;

class BottomPanel
{
    Fl_Group *ptr;
    GameField *gf_ptr;
    Fl_Widget *b_text_ptr;
    char t_text[30];
    char f_text[70];
public:
    BottomPanel(GameField *a_gfptr);
    void updateText();
    const char *getText() const {return f_text;}
};

#endif // BOTTOMPANEL_H

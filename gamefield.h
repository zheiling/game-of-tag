#ifndef GAMEFIELD_H
#define GAMEFIELD_H

#include "datacontainer.h"
#include "bottompanel.h"

class FieldButton;
class TopPanel;

class GameField {
    friend class FieldButton;
    FieldButton *button_matrix[4][4];
    DataContainer *data;
    TopPanel *top_panel;
    BottomPanel *bottom_panel;
    bool swapWithEmpty(FieldButton *);
    FieldButton *getEmptyButton() const;
    FieldButton *getButton(int x, int y) const {return button_matrix[x][y];}
    void doStep(bool is_redo, bool &redo, bool &undo);
public:
    void   syncButtons();
    void   undoStep(bool &redo, bool &undo);
    void   redoStep(bool &redo, bool &undo);
    void   redoStep();
    void   setTopPanel(TopPanel *tp) {top_panel = tp;}
    void   setBottomPanel(BottomPanel *bp) {bottom_panel = bp;}
    void   resetSteps() {data->resetSteps();}
    void   updateBottomText(){ bottom_panel->updateText(); }
    bool   checkEndGame () const;
    GameField(DataContainer *a_data);
    ~GameField();
};

#endif // GAMEFIELD_H

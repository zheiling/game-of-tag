#ifndef GAMEFIELD_H
#define GAMEFIELD_H

#include "datacontainer.h"
#include "bottompanel.h"

class FieldButton;
class TopPanel;

class GameField {
    friend class FieldButton;
    FieldButton *button_matrix[4][4];
    int val_matrix[4][4];
    int e_pos[2]; // empty button position
    DataContainer *data;
    TopPanel *top_panel;
    BottomPanel *bottom_panel;
    void setEmptyPos(int x, int y) {
        e_pos[0] = x;
        e_pos[1] = y;
    }
    bool swapWithEmpty(FieldButton *);
    FieldButton *getEmptyButton() const {return button_matrix[e_pos[0]][e_pos[1]];}
    FieldButton *getButton(int x, int y) const {return button_matrix[x][y];}
    void doStep(bool is_redo, bool &redo, bool &undo);
    void getEmptyPosition(int &x, int &y) {x = e_pos[0]; y = e_pos[1];}
public:
    void   genValPositions(int seed);
    void   genValPositions();
    void   syncButtons();
    void   undoStep(bool &redo, bool &undo);
    void   redoStep(bool &redo, bool &undo);
    void   setTopPanel(TopPanel *tp) {top_panel = tp;}
    void   setBottomPanel(BottomPanel *bp) {bottom_panel = bp;}
    void   resetSteps() {data->resetSteps();}
    int    getStepsCount() const {return data->getStepCount();}
    int    getCurrentStep() const {return data->getCurrentStep();}
    void   updateBottomText(){ bottom_panel->updateText(); }
    int    genNewSeed(){return data->genNewSeed();}
    int    getCurrentSeed() const {return data->seed;}
    time_t getSessionTime() const {return data->getSessionTime();}
    bool   checkEndGame () const;
    GameField();
    ~GameField();
};

#endif // GAMEFIELD_H

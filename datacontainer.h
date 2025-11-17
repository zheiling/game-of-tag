#ifndef DATACONTAINER_H
#define DATACONTAINER_H
#include <ctime>

class GameField;
class FieldButton;

class DataContainer
{
    friend class GameField;
    friend class TopPanel;
    friend class FieldButton;

    unsigned char steps[10000];

    int last_idx = 0;
    int cur_idx = 0;
    int seed;
    time_t t_start;

    int val_matrix[4][4];
    int empty_pos[2]; // empty button position
    void findEmptyPosition();
public:
    DataContainer() {time(&t_start); seed = (int) t_start;};
    ~DataContainer() {};
    bool redoStep();
    void getEmptyPosition(int &x, int &y) {x = empty_pos[0]; y = empty_pos[1];}
    void addStep(struct step_val *sv, bool &redo, bool &undo);
    void undoStep(int &ebt_x, int &ebt_y, bool &redo, bool &undo);
    void redoStep(int &ebt_x, int &ebt_y, bool &redo, bool &undo);
    void resetSteps() {last_idx=0; cur_idx=0; time(&t_start);}
    void openSessionFile(GameField *gf);
    void saveSessionFile();
    int  getStepsCount() {return last_idx;}
    int  getCurrentStep() {return cur_idx;}
    int  genNewSeed() {return seed = time(0);}
    int  getCurrentSeed() const {return seed;}
    void setValue(int x, int y, int v);
    void genValPositions(int seed);
    void genValPositions();
    void setEmptyPos(int x, int y) {
        empty_pos[0] = x;
        empty_pos[1] = y;
    }
    bool swapWithEmpty(int x, int y);
    time_t getSessionTime();
};

#endif // DATACONTAINER_H

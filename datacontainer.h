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
public:
    DataContainer() {time(&t_start); seed = (int) t_start;};
    void addStep(struct step_val *sv, bool &redo, bool &undo);
    void undoStep(int &ebt_x, int &ebt_y, bool &redo, bool &undo);
    void redoStep(int &ebt_x, int &ebt_y, bool &redo, bool &undo);
    int  getStepsCount() {return last_idx;}
    int  getCurrentStep() {return cur_idx;}
    void resetSteps() {last_idx=0; cur_idx=0; time(&t_start);}
    int  genNewSeed() {return seed = time(0);}
    int  getCurrentSeed() const {return seed;}
    time_t getSessionTime();
    void openSessionFile(GameField *gf);
    void saveSessionFile();
    ~DataContainer() {};
};

#endif // DATACONTAINER_H

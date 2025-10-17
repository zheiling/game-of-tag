#include "gamefield.h"
#include <cstdlib>
#include "fieldbutton.h"

GameField::GameField(DataContainer *a_data) {
    data = a_data;
    genValPositions();
    for (int line = 0; line < 4; line++) {
        for (int col = 0; col < 4; col++) {
            button_matrix[line][col] = new FieldButton(line, col, val_matrix[line][col], this);
        }
    }
}

GameField::~GameField() {
    delete data;
    for (int line = 0; line < 4; line++) {
        for (int col = 0; col < 4; col++) {
            delete button_matrix[line][col];
        }
    }
}

int extractFromArray(int idx, char *arr) {
    int result = arr[idx];
    int i = idx;
    for (; arr[i] || i==0; i++) arr[i] = arr[i+1];
    arr[i-1] = 0;
    return result;
}

void GameField::genValPositions(int seed) {
    srand(seed);
    char empt_idxs[17];
    for (int i=0; i<16; i++) empt_idxs[i] = i;
    empt_idxs[16] = 0;

    for(int i=0; i<16; i++) {
        int x, y;
        if (i == 15) {
            x = empt_idxs[0] / 4;
            y = empt_idxs[0] % 4;
            val_matrix[x][y]=-1;
            break;
        }
        int e_idx = rand() % (16-i);
        int m_idx = extractFromArray(e_idx, empt_idxs);
        x = m_idx / 4;
        y = m_idx % 4;
        int val = i - 1;
        if (val == -1) val++;
        val_matrix[x][y]=val;
    }
}

void GameField::genValPositions() {
    genValPositions(data->seed);
}

bool GameField::swapWithEmpty(FieldButton *fb) {
    FieldButton *efb = getEmptyButton();
    if (abs(efb->x - fb->x) + abs(efb->y - fb->y) != 1) return false;
    FieldButton::setValue(efb, fb->v, true);
    efb->show();
    setEmptyPos(fb->x, fb->y);
    fb->hide();
    return true;
}

void GameField::syncButtons() {
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            int val = val_matrix[x][y];
            FieldButton *b = button_matrix[x][y];
            FieldButton::setValue(b, val, false);
            if (val == -1) { b->hide(); } else {
                b->show();
            }
        }
    }
}

void GameField::doStep(bool is_redo, bool &redo, bool &undo) {
    int x, y;
    is_redo ? data->redoStep(x, y, redo, undo) : data->undoStep(x, y, redo, undo);
    if (x == -1 || y == -1) {
        return ;
    };
    FieldButton *fb = button_matrix[x][y];
    GameField::swapWithEmpty(fb);
}

void GameField::redoStep(bool &redo, bool &undo) {
    return doStep(true, redo, undo);
}

void GameField::redoStep() {
    bool redo, undo; // заглушки
    return doStep(true, redo, undo);
}

void GameField::undoStep(bool &redo, bool &undo) {
    return doStep(false, redo, undo);
}

bool GameField::checkEndGame() const {
    // TODO: переписать функцию
    bool eg_situation = false; // end game situation
    for(int x=0; x<4; x++) {
        for (int y=0; y<4; y++) {
            int nx = x, ny = y+1;
            if (y == 0 && x==0 && val_matrix[x][y] == 0) continue;
            if (x==2 && y==3) { eg_situation = true; goto exit; }
            if (y == 3) {
                ny=0;nx++;
            }
            if (val_matrix[nx][ny] - val_matrix[x][y] != 1) goto exit;
        }
    }
exit:
    return eg_situation;
}

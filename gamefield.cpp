#include "gamefield.h"
#include "fieldbutton.h"

GameField::GameField(DataContainer *a_data) {
    data = a_data;
    data->genValPositions();
    for (int line = 0; line < 4; line++) {
        for (int col = 0; col < 4; col++) {
            button_matrix[line][col] = new FieldButton(line, col, data->val_matrix[line][col], this);
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

bool GameField::swapWithEmpty(FieldButton *fb) {
    data->swapWithEmpty(fb->x, fb->y);
    syncButtons();
    return true;
}

void GameField::syncButtons() {
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            int val = data->val_matrix[x][y];
            FieldButton *b = button_matrix[x][y];
            if (val != b->v) {
                FieldButton::setValue(b, val, false);
                if (val == -1) { b->hide(); } else {
                    b->show();
                }
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
    syncButtons();
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

FieldButton *GameField::getEmptyButton() const {
    int x, y;
    data->getEmptyPosition(x, y);
    return button_matrix[x][y];
}

bool GameField::checkEndGame() const {
    bool eg_situation = false; // end game situation
    for(int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            int nx = x, ny = y+1;
            if (y == 0 && x == 0 && data->val_matrix[x][y] == 0) continue;
            if (x == 2 && y == 3) { eg_situation = true; goto exit; }
            if (y == 3) {
                ny=0;nx++;
            }
            if (data->val_matrix[nx][ny] - data->val_matrix[x][y] != 1) goto exit;
        }
    }
exit:
    return eg_situation;
}

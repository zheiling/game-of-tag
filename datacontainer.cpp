#include <FL/Fl_File_Chooser.H>
#include <FL/fl_ask.H>
#include <cerrno>
#include <fcntl.h>
#include <cstring>

#include "datacontainer.h"
#include "common.h"
#include "gamefield.h"
#include "utils/json.h"

#define ERR_READ_FILE "Error while reading the file!"

void DataContainer::setValue(int x, int y, int v) {
    val_matrix[x][y] = v;
    if (v == -1) setEmptyPos(x, y);
}

bool DataContainer::swapWithEmpty(int x, int y) {
    int ex, ey;
    getEmptyPosition(ex, ey);
    if (!abs(ex - x) || !abs(ey - y)) {
        int vx, vy;
        if (abs(ex - x)) {
            for (int i = ex - x; i != 0; i > 0 ? i-- : i++) {
                vx = i < 0 ? ex + 1 : ex - 1;
                setValue(ex, ey, val_matrix[vx][y]);
                setValue(vx, ey, -1);
                ex += vx - ex;
            }
        } else {
            for (int i = ey - y; i != 0; i > 0 ? i-- : i++) {
                vy = i < 0 ? ey + 1 : ey - 1;
                setValue(ex, ey, val_matrix[x][vy]);
                setValue(ex, vy, -1);
                ey += vy - ey;
            }
        }
        return true;
    } else return false;
}

void DataContainer::addStep(struct step_val *sv, bool &redo, bool &undo) {
    int ebt_fidx = sv->e.y * 4 + sv->e.x;
    int vbt_fidx = sv->v.y * 4 + sv->v.x;
    steps[cur_idx++] = (unsigned char) (ebt_fidx * 16 + vbt_fidx);
    last_idx = cur_idx;
    redo = false;
    undo = cur_idx > 0;
};

static void extract_button_bidxs(unsigned char val, struct step_val *sv) {
    int ebt_fidx = val / 16;
    int vbt_fidx = val % 16;
    sv->e.x = ebt_fidx % 4;
    sv->e.y = ebt_fidx / 4;
    sv->v.x = vbt_fidx % 4;
    sv->v.y = vbt_fidx / 4;
}

void DataContainer::undoStep(int &ebt_x, int &ebt_y, bool &redo, bool &undo) {
    if (cur_idx == 0) return;
    cur_idx--;
    int val = (int) steps[cur_idx];
    struct step_val sv;
    extract_button_bidxs(val, &sv);
    ebt_x = sv.e.x;
    ebt_y = sv.e.y;
    swapWithEmpty(sv.e.x, sv.e.y);
    redo = cur_idx < last_idx;
    undo = cur_idx > 0;
}

void DataContainer::redoStep(int &vbt_x, int &vbt_y, bool &redo, bool &undo) {
    if (cur_idx == last_idx) return;
    int val = (int) steps[cur_idx];
    struct step_val sv;
    extract_button_bidxs(val, &sv);
    vbt_x = sv.v.x;
    vbt_y = sv.v.y;
    cur_idx++;
    swapWithEmpty(sv.v.x, sv.v.y);
    redo = cur_idx < last_idx;
    undo = cur_idx > 0;
}

bool DataContainer::redoStep() {
    if (cur_idx == last_idx) return true;
    int val = (int) steps[cur_idx];
    struct step_val sv;
    extract_button_bidxs(val, &sv);
    if (!swapWithEmpty(sv.v.x, sv.v.y)) return false;
    cur_idx++;
    return true;
}

time_t DataContainer::getSessionTime() {
    time_t c_time = time(0);
    return c_time - t_start;
}

static int extractFromArray(int idx, char *arr) {
    int result = arr[idx];
    int i = idx;
    for (; arr[i] || i==0; i++) arr[i] = arr[i+1];
    arr[i-1] = 0;
    return result;
}

void DataContainer::genValPositions(int seed) {
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
            setEmptyPos(x, y);
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

void DataContainer::genValPositions() {
    genValPositions(seed);
}

void DataContainer::findEmptyPosition() {
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            if (val_matrix[x][y] == -1) {
                setEmptyPos(x, y);
                break;
            }
        }
    }
}

static char n_seed[] = "seed";
static char n_steps[] = "steps";
static char n_time[] = "time";
static char n_current[] = "current";

static i_field_t fields[] = {
    {.name = n_seed, .type = J_INT},
    {.name = n_steps, .type = J_A_CHAR},
    {.name = n_time, .type = J_TIME},
    {.name = n_current, .type = J_INT},
};

typedef struct {
    int seed;
    int current;
    unsigned char *steps;
    time_t time;
} data_struct;

void process_o_field(o_field_t *field, data_struct *data) {
    if (!strcmp(field->name, n_seed)) {
        mempcpy(&(data->seed), field->data, sizeof(int));
    }
    if (!strcmp(field->name, n_time)) {
        mempcpy(&(data->time), field->data, sizeof(time_t));
    }
    if (!strcmp(field->name, n_current)) {
        mempcpy(&(data->current), field->data, sizeof(int));
    }
    if (!strcmp(field->name, n_steps)) {
        data->steps = (unsigned char *) field->data;
    }
}

void DataContainer::openSessionFile(GameField *gf) {
    data_struct data = {.seed = 0, .current = 0, .steps = 0, .time = 0};
    char *fname = fl_file_chooser("Choose session file", "*.json", 0);
    FILE *f = fopen(fname, "r");
    if (!f) {
        fl_alert("Can't open the file!");
        return;
    }
    fseek(f, 0, SEEK_END);
    size_t f_size = ftell(f);
    rewind(f);
    if (f_size == 0) {
        if (errno) {
            fl_alert(ERR_READ_FILE);
        } else {
            fl_alert("The file is empty!");
        }
        fclose(f);
        return;
    }
    char *f_local = new char[f_size];
    fread(f_local, 1, f_size, f);
    fclose(f);

    o_field_t *ofield;

    for (;;) {
        ofield = parse_json_field(f_local, f_size, fields);
        if (ofield == NULL) break;
        process_o_field(ofield, &data);
        clear_json_field(ofield);
    }

    delete[] f_local;

    seed = data.seed;
    if (!data.steps) return;
    int steps_count = strlen((char *) data.steps);
    last_idx = steps_count-1;
    mempcpy(steps, data.steps, steps_count);
    free(data.steps);
    t_start = time(0) - data.time;
    cur_idx = 0;
    genValPositions();
    findEmptyPosition();
    for (int i=data.current; i>0; i--) {
        if (!redoStep()) {
            fl_alert("Corrupted steps history!");
            genValPositions(); // gen again
            cur_idx = 0;
            last_idx = 0;
            break;
        }
    }
    gf->syncButtons();
}

void DataContainer::saveSessionFile() {
    char fname[128];
    sprintf(fname, "PT_%d.json", seed);
    Fl_File_Chooser *fcd = new Fl_File_Chooser(fname, "*.json", Fl_File_Chooser::CREATE, "Save session");
    fcd->show();
    if (!fcd->value()) return;
    FILE *f = fopen(fcd->value(), "w");
    if (!f) return;
    char *f_steps = new char[(last_idx+1)*4];
    char *f_steps_pos = f_steps;
    int nw;
    for (int i = 0; i < last_idx - 1; i++) {
       sprintf(f_steps_pos, "%u,%n", steps[i], &nw);
        f_steps_pos += nw;
    }
    sprintf(f_steps_pos, "%u", steps[last_idx-1]);
    fprintf(f, "{ \"seed\": %d, \"time\" : %ld, \"steps\": [%s], \"current\": %d }\n", \
            seed, time(0) - t_start, f_steps, cur_idx);
    delete[] f_steps;
    fclose(f);
}

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
    redo = cur_idx < last_idx;
    undo = cur_idx > 0;
}

time_t DataContainer::getSessionTime() {
    time_t c_time = time(0);
    return c_time - t_start;
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

    // TODO: checking for the feasibility of the steps
    delete[] f_local;

    seed = data.seed;
    if (!data.steps) return;
    int steps_count = strlen((char *) data.steps);
    last_idx = steps_count-1;
    mempcpy(steps, data.steps, steps_count);
    free(data.steps);
    t_start = time(0) - data.time;
    cur_idx = 0;
    gf->genValPositions();
    gf->syncButtons(); // TODO: make synchronization and step execution virtual
    for (int i=data.current; i>0; i--) {
        gf->redoStep();
    }
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

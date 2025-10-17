#include <FL/Fl_File_Chooser.H>
#include <FL/fl_ask.H>
#include <cctype>
#include <cerrno>
#include <fcntl.h>

#include "datacontainer.h"
#include "common.h"
#include "gamefield.h"

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

void DataContainer::openSessionFile(GameField *gf) {
    char *fname = fl_file_chooser("Choose session file", "*.json", 0);
    FILE *f = fopen(fname, "r");
    if (!f) {
        fl_alert("Can't open the file!");
        return;
    }
    time_t ctime;
    int cseed, n_read, ccur_idx, clast_idx;
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
    int res = sscanf(f_local, "{ \"seed\": %d, \"time\" : %ld, \"steps\" : [%n",
                     &cseed, &ctime, &n_read);
    if (res < 2) {
        delete[] f_local;
        fl_alert(ERR_READ_FILE);
        return;
    }
    unsigned char c = 0;
    clast_idx = 0;
    for (int i = n_read-1;i++;) {
        switch(f_local[i]) {
        case ',':
            steps[clast_idx++] = c;
            c = 0;
            continue;
        case ']':
            steps[clast_idx++] = c;
            n_read = i+1;
            goto exit_loop;
        default:
            if (isspace(f_local[i])) continue;
            c = c * 10 + f_local[i] - '0';
        }
    }
exit_loop:
    // TODO: проверка на выполнимость шагов
    res = sscanf(f_local+n_read, ", \"current\": %d", &ccur_idx);
    if (clast_idx < ccur_idx) {
        fl_alert("Data in the file is not correct");
        return;
    }
    if (res < 1) {
        fl_alert(ERR_READ_FILE);
    }
    delete[] f_local;

    seed = cseed;
    last_idx = clast_idx;
    steps[last_idx] = 0;
    t_start = time(0) - ctime;
    gf->genValPositions();
    gf->syncButtons(); // TODO: сделать синхронизацию и выполнение шагов виртуальным
    for (int i=ccur_idx; i>0; i--) {
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

#ifndef COMMON_H
#define COMMON_H

#define SCALE 3

enum {
    spacing = 5 * SCALE,
    m_but = 100 * SCALE,
    m_label_size = 40 * SCALE,
    win_w = spacing*5 + m_but*4,
    t_size = 30 * SCALE,
    tp_l_size = 12 * SCALE,
    bp_text_size = 12 * SCALE,
    tp_h = t_size + spacing*3,
    gf_h = spacing*5 + m_but*4,
    bp_h = spacing*3 + bp_text_size,
};

struct step_val {
    struct {int x; int y;} e;
    struct {int x; int y;} v;
};

#endif // COMMON_H

#ifndef UI_INTERFACE_H
#define UI_INTERFACE_H

/**
 * Lightweight structure to hold a mthread status
 * on messages
 */
struct ui_mthread {
        int id;
        int progress;
        double value;
};

/**
 * To notify that the ui_mthread state has changed
 */
typedef void (* ui_on_updated)(struct ui_mthread * thread);

/**
 * To notify that the checked state for a ui_mthread has changed 
 */
typedef void (* ui_on_checked_state_changed)(int id, int checked);

#endif

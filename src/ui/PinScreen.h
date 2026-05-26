#pragma once

#include "lvgl.h"

class AppUI;

class PinScreen {
public:
    explicit PinScreen(AppUI *ui);

    lv_obj_t *screen() { return screen_; }
    void reset();

private:
    AppUI    *ui_;
    lv_obj_t *screen_    = nullptr;
    lv_obj_t *dots_[4]   = {};
    lv_obj_t *errorLabel_= nullptr;
    lv_obj_t *cancelBtn_ = nullptr;

    char     entered_[5] = {};  // up to 4 digits + null terminator
    int      len_        = 0;

    lv_timer_t *errorTimer_ = nullptr;

    void build();
    void pushDigit(char d);
    void backspace();
    void validate();
    void showError();
    void updateDots();

    static void onKey(lv_event_t *e);
    static void onBackspace(lv_event_t *e);
    static void onCancel(lv_event_t *e);
    static void onErrorTimer(lv_timer_t *t);
};

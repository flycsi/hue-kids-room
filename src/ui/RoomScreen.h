#pragma once

#include "lvgl.h"
#include "../hue/HueClient.h"

class AppUI;

class RoomScreen {
public:
    static constexpr int MAX_ROOMS = 16;

    explicit RoomScreen(AppUI *ui);

    lv_obj_t *screen() { return screen_; }

    /** Show loading state and kick off background HTTP fetch */
    void startFetch();

    /** Called from fetch task (inside LVGL lock) */
    void onFetchDone(HueGroup *groups, int count);
    void onFetchError();

private:
    struct Entry { int id; char name[32]; };

    AppUI    *ui_;
    lv_obj_t *screen_    = nullptr;
    lv_obj_t *statusLbl_ = nullptr;
    lv_obj_t *list_      = nullptr;

    Entry entries_[MAX_ROOMS];
    int   entryCount_ = 0;

    void build();
    void showLoading();
    void clearList();

    static void onRoomTap(lv_event_t *e);
    static void onCancel(lv_event_t *e);
};

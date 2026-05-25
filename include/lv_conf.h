/**
 * lv_conf.h — LVGL 9.x configuration for Waveshare ESP32-C6 Touch AMOLED 2.16
 * 480×480 RGB565 — QSPI SH8601 — capacitive touch
 */

#if 1  /* Set to "1" to enable */
#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/*====================
   COLOR SETTINGS
 *====================*/
#define LV_COLOR_DEPTH 16

/*=========================
   STDLIB WRAPPER SETTINGS
 *=========================*/
/* LV_STDLIB_CLIB uses malloc/free/realloc from libc — no fixed pool limit.
   Required because the circle shadow buffer can reach 57 KB which overflows
   the builtin 64 KB pool once object metadata is added. */
#define LV_USE_STDLIB_MALLOC    LV_STDLIB_CLIB
#define LV_USE_STDLIB_STRING    LV_STDLIB_CLIB
#define LV_USE_STDLIB_SPRINTF   LV_STDLIB_CLIB

/*====================
   HAL SETTINGS
 *====================*/
#define LV_DEF_REFR_PERIOD   33   /* [ms] ~30 FPS */
#define LV_DPI_DEF           130  /* [px/inch] — 2.16" 480px */

/*================
 *  DRAW SETTINGS
 *================*/
#define LV_DRAW_BUF_STRIDE_ALIGN   1
#define LV_DRAW_BUF_ALIGN          4

/*=====================
 *  COMPILER SETTINGS
 *====================*/
#define LV_ATTRIBUTE_MEM_ALIGN      __attribute__((aligned(4)))
#define LV_ATTRIBUTE_MEM_ALIGN_SIZE 4
#define LV_ATTRIBUTE_LARGE_CONST
#define LV_EXPORT_CONST_INT(int_value) struct _silence_single_declaration
#define LV_USE_LARGE_COORD  0

/*=================
 * LOGGING
 *=================*/
#define LV_USE_LOG      1
#define LV_LOG_LEVEL    LV_LOG_LEVEL_WARN
#define LV_LOG_PRINTF   1
#define LV_LOG_USE_TIMESTAMP 0

/*=================
 *  ASSERT
 *=================*/
#define LV_USE_ASSERT_NULL          1
#define LV_USE_ASSERT_MALLOC        1
#define LV_USE_ASSERT_STYLE         0
#define LV_USE_ASSERT_MEM_INTEGRITY 0
#define LV_USE_ASSERT_OBJ           0
#define LV_ASSERT_HANDLER_INCLUDE   <stdint.h>
#define LV_ASSERT_HANDLER           while(1);

/*================
 * OBJECT SETTINGS
 *================*/
#define LV_USE_OBJ_PROPERTY 0

/*=====================================
 *   INPUT DEVICE SETTINGS
 *====================================*/
#define LV_INDEV_DEF_READ_PERIOD          33   /* [ms] */
#define LV_INDEV_DEF_SCROLL_LIMIT         10
#define LV_INDEV_DEF_SCROLL_THROW         10
#define LV_INDEV_DEF_LONG_PRESS_TIME      400  /* [ms] */
#define LV_INDEV_DEF_LONG_PRESS_REP_TIME  100  /* [ms] */
#define LV_INDEV_DEF_GESTURE_LIMIT        50
#define LV_INDEV_DEF_GESTURE_MIN_VELOCITY 3

/*=================
 * FEATURE CONFIG
 *=================*/
#define LV_USE_ANIM        1
#define LV_USE_TIMER       1
#define LV_USE_OBSERVER    1
#define LV_USE_SNAPSHOT    0
#define LV_USE_REFR_DEBUG  0
#define LV_USE_SYSMON      0
#define LV_USE_PROFILER    0
#define LV_USE_PERF_MONITOR 0
#define LV_USE_MEM_MONITOR  0
#define LV_USE_CACHE        0
#define LV_USE_OBJ_ID       0
#define LV_USE_STYLE_ITEM_DECOR_LINE 1

/*=================
 *  LAYOUT ENGINES
 *=================*/
#define LV_USE_FLEX   1
#define LV_USE_GRID   0

/*================
 * FONT SETTINGS
 *================*/
#define LV_FONT_MONTSERRAT_8   0
#define LV_FONT_MONTSERRAT_10  0
#define LV_FONT_MONTSERRAT_12  0
#define LV_FONT_MONTSERRAT_14  1
#define LV_FONT_MONTSERRAT_16  0
#define LV_FONT_MONTSERRAT_18  0
#define LV_FONT_MONTSERRAT_20  0
#define LV_FONT_MONTSERRAT_22  0
#define LV_FONT_MONTSERRAT_24  1
#define LV_FONT_MONTSERRAT_26  0
#define LV_FONT_MONTSERRAT_28  0
#define LV_FONT_MONTSERRAT_30  0
#define LV_FONT_MONTSERRAT_32  0
#define LV_FONT_MONTSERRAT_34  0
#define LV_FONT_MONTSERRAT_36  0
#define LV_FONT_MONTSERRAT_38  0
#define LV_FONT_MONTSERRAT_40  0
#define LV_FONT_MONTSERRAT_42  0
#define LV_FONT_MONTSERRAT_44  0
#define LV_FONT_MONTSERRAT_46  0
#define LV_FONT_MONTSERRAT_48  1

#define LV_FONT_DEFAULT  &lv_font_montserrat_14
#define LV_FONT_FMT_TXT_LARGE 0
#define LV_USE_FONT_SUBPX     0
#define LV_USE_FONT_PLACEHOLDER 1

/*=================
 *  TEXT SETTINGS
 *=================*/
#define LV_TXT_ENC             LV_TXT_ENC_UTF8
#define LV_TXT_BREAK_CHARS     " ,.;:-_"
#define LV_TXT_LINE_BREAK_LONG_LEN    0
#define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN  3
#define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 3
#define LV_TXT_COLOR_CMD "#"
#define LV_USE_BIDI     0
#define LV_USE_ARABIC_PERSIAN_CHARS 0

/*=====================
 *  WIDGET USAGE
 *====================*/
#define LV_USE_ANIMIMG      0
#define LV_USE_ARC          1
#define LV_USE_BAR          1
#define LV_USE_BTN          1   /* compat alias */
#define LV_USE_BUTTON       1
#define LV_USE_BTNMATRIX    0
#define LV_USE_CALENDAR     0
#define LV_USE_CANVAS       0
#define LV_USE_CHART        0
#define LV_USE_CHECKBOX     0
#define LV_USE_COLORWHEEL   0   /* removed in LVGL v9 */
#define LV_USE_DROPDOWN     0
#define LV_USE_IMG          1
#define LV_USE_IMAGEBUTTON  0
#define LV_USE_KEYBOARD     0
#define LV_USE_LABEL        1
#define LV_USE_LED          1
#define LV_USE_LINE         1
#define LV_USE_LIST         0
#define LV_USE_MENU         0
#define LV_USE_METER        0
#define LV_USE_MSGBOX       0
#define LV_USE_ROLLER       0
#define LV_USE_SCALE        0
#define LV_USE_SLIDER       1
#define LV_USE_SPINBOX      0
#define LV_USE_SPINNER      1
#define LV_USE_SWITCH       1
#define LV_USE_TABLE        0
#define LV_USE_TABVIEW      0
#define LV_USE_TEXTAREA     0
#define LV_USE_TILEVIEW     0
#define LV_USE_WIN          0

/*================
 *  THEME
 *================*/
#define LV_USE_THEME_DEFAULT    1
#if LV_USE_THEME_DEFAULT
    #define LV_THEME_DEFAULT_DARK   1
    #define LV_THEME_DEFAULT_GROW   1
    #define LV_THEME_DEFAULT_TRANSITION_TIME 80
#endif
#define LV_USE_THEME_SIMPLE     0
#define LV_USE_THEME_MONO       0

/*====================
 *  MISCELLANEOUS
 *====================*/
#define LV_USE_MONKEY     0
#define LV_USE_GRIDNAV    0
#define LV_USE_FRAGMENT   0
#define LV_USE_IMGFONT    0
#define LV_USE_OBSERVER   1
#define LV_USE_MSGBOX     0
#define LV_USE_TABVIEW    0
#define LV_USE_TILEVIEW   0
#define LV_USE_WIN        0
#define LV_USE_SPAN       0

/*================
 *  DEMO USAGE
 *================*/
#define LV_USE_DEMO_WIDGETS         0
#define LV_USE_DEMO_KEYPAD_AND_ENCODER 0
#define LV_USE_DEMO_BENCHMARK       0
#define LV_USE_DEMO_RENDER          0
#define LV_USE_DEMO_STRESS          0
#define LV_USE_DEMO_MUSIC           0
#define LV_USE_DEMO_FLEX_LAYOUT     0
#define LV_USE_DEMO_MULTILANG       0
#define LV_USE_DEMO_TRANSFORM       0
#define LV_USE_DEMO_SCROLL          0

#endif /* LV_CONF_H */
#endif /* End of "Content enable" */

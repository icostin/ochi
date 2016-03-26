#ifndef _OCHI_INTERN_H
#define _OCHI_INTERN_H

#define OINI_TWX (1 << 0)
#define OINI_CONTENT_WIN (1 << 1)
#define OINI_TOP_BAR (1 << 2)
#define OINI_STATUS_BAR (1 << 3)

#define OAM_HELP 0
#define OAM_UI 1

#define OM_NORMAL 0
#define OM_CMD 1

#if _DEBUG
#include <stdlib.h>
#include <stdio.h>
#define L(...) ((void) fprintf(stderr, "%s:%u:%s(): ", __FILE__, __LINE__, __FUNCTION__), (void) fprintf(stderr, __VA_ARGS__), (void) fprintf(stderr, "\n"), (void) fflush(stderr))
#define A(_cond) ((_cond)) ? (void) 0 : (L("*** ASSERT FAILED *** %s", #_cond), abort())
#else
#define L(...) ((void) 0)
#define A(_cond) ((void) 0)
#endif

#if _DEBUG >= 2
#define L2(...) L(__VA_ARGS__)
#else
#define L2(...) ((void) 0)
#endif

#define SET_ATTR(_attr, _bg, _fg, _mode) \
    ((_attr).bg = (_bg), (_attr).fg = (_fg), (_attr).mode = (_mode))

#define TC(_expr) \
    do { if ((ts = (_expr))) { \
        L("twx expression (%s) failed: %u", #_expr, ts); \
        return ts; \
    } } while (0)

typedef struct ochi_s ochi_t;
struct ochi_s
{
    twx_win_t root_win;
    twx_t * twx;
    twx_win_t * content_win;
    twx_win_t * top_bar;
    twx_win_t * status_bar;
    twx_win_t * cmd_win;
    acx1_attr_t temp_attr;
    unsigned int init_state;
    acx1_attr_t a;
    acx1_attr_t top_bar_attr[2];
    acx1_attr_t status_bar_attr[2];
    acx1_attr_t cmd_attr[3];
    unsigned int app_mode;
    unsigned int mode;
    char err_msg[0x100];
};

uint8_t ochi_init
(
    ochi_t * ochi,
    unsigned int argc,
    uint8_t const * const * argv
);

uint8_t ui_init (ochi_t * ochi);
void attr_init (ochi_t * ochi);
void ochi_finish (ochi_t * ochi);

twx_status_t ZLX_CALL root_geom (twx_win_t * win,
                                 unsigned int sr, unsigned int sc,
                                 unsigned int h, unsigned int w);

twx_status_t ZLX_CALL root_handler (twx_win_t * win, unsigned int evt,
                                    twx_event_info_t * ei);

#endif /* _OCHI_INTERN_H */


#include <string.h>
#include <stdio.h>
#include <zlx.h>
#include <hbs.h>
#include <acx1.h>
#include <twx.h>
#include "intern.h"

HBS_MAIN(ochi_main);

static twx_win_class_t root_wcls =
{
    root_handler,
    twx_nop_finish,
    0,
    "ochi/root"
};

static char const * main_shortcuts =
    " \a\001:\a\000 cmd  \a\001F1\a\000 help  \a\001Alt-x\a\000 exit";

/* ochi_init ****************************************************************/
uint8_t ochi_init
(
    ochi_t * ochi,
    unsigned int argc,
    uint8_t const * const * argv
)
{
    (void) argv;

    memset(ochi, 0, sizeof(*ochi));
    attr_init(ochi);

    ochi->app_mode = argc == 1 ? OAM_HELP : OAM_UI;
    return 0;
}

/* attr_init ****************************************************************/
void attr_init (ochi_t * ochi)
{
    SET_ATTR(ochi->a, ACX1_DARK_BLUE, ACX1_LIGHT_GRAY, ACX1_NORMAL);
    SET_ATTR(ochi->top_bar_attr[0], ACX1_GRAY, ACX1_BLACK, ACX1_NORMAL);
    SET_ATTR(ochi->top_bar_attr[1], ACX1_GRAY, ACX1_DARK_RED, ACX1_NORMAL);
    SET_ATTR(ochi->status_bar_attr[0], ACX1_GRAY, ACX1_BLACK, ACX1_NORMAL);
    SET_ATTR(ochi->status_bar_attr[1], ACX1_GRAY, ACX1_DARK_RED, ACX1_NORMAL);
    SET_ATTR(ochi->cmd_attr[TWX_ITXT_ATTR_PFX], ACX1_DARK_MAGENTA, ACX1_LIGHT_GREEN, ACX1_NORMAL);
    SET_ATTR(ochi->cmd_attr[TWX_ITXT_ATTR_TXT], ACX1_DARK_MAGENTA, ACX1_LIGHT_YELLOW, ACX1_NORMAL);
    SET_ATTR(ochi->cmd_attr[TWX_ITXT_ATTR_MARK], ACX1_DARK_YELLOW, ACX1_LIGHT_MAGENTA, ACX1_NORMAL);
}

/* ui_init ******************************************************************/
uint8_t ui_init (ochi_t * ochi)
{
    twx_status_t ts;

    ts = twx_create(&ochi->twx);
    L("twx_create: %u", ts);
    if (ts)
    {
        snprintf(ochi->err_msg, sizeof(ochi->err_msg) - 1,
                 "twx init error (%u)", ts);
        return 126;
    }
    ochi->init_state |= OINI_TWX;

    ochi->root_win.wcls = &root_wcls;
    ochi->root_win.twx = ochi->twx;
    L("creating blank...");
    ts = twx_blank_win_create(ochi->twx, &ochi->content_win, &ochi->a, '.');
    L("blank content: %u", ts);
    if (ts)
    {
        snprintf(ochi->err_msg, sizeof(ochi->err_msg) - 1,
                 "failed to create root window (%u)\n", ts);
        return 126;
    }
    ochi->init_state |= OINI_CONTENT_WIN;

    ts = twx_htxt_win_create(ochi->twx, &ochi->top_bar, &ochi->top_bar_attr[0],
                             ZLX_ITEM_COUNT(ochi->top_bar_attr),
                             " \a\001ochi\a\000 | binary inspection tool");
    L("htxt top: %u", ts);
    ochi->init_state |= OINI_TOP_BAR;

    ts = twx_htxt_win_create(ochi->twx, &ochi->status_bar, 
                             &ochi->status_bar_attr[0],
                             ZLX_ITEM_COUNT(ochi->status_bar_attr),
                             main_shortcuts);
    L("htxt status: %u", ts);
    ochi->init_state |= OINI_TOP_BAR;

    L("setting root...");
    twx_set_root(&ochi->root_win);
    L("set root done");
    twx_post_win_focus(&ochi->root_win);
    L("focus on root done");

    return 0;
}

/* ui_run *******************************************************************/
uint8_t ui_run (ochi_t * ochi)
{
    twx_status_t ts;
    ts = twx_run(ochi->twx);
    if (ts) return 1;
    return 0;
}

/* ochi_finish **************************************************************/
void ochi_finish (ochi_t * ochi)
{
    if ((ochi->init_state & OINI_CONTENT_WIN))
        twx_win_destroy(ochi->content_win);
    if ((ochi->init_state & OINI_TOP_BAR))
        twx_win_destroy(ochi->top_bar);
    if ((ochi->init_state & OINI_STATUS_BAR))
        twx_win_destroy(ochi->status_bar);
    if ((ochi->init_state & OINI_TWX))
        twx_destroy(ochi->twx);
}

/* ochi_main ****************************************************************/
uint8_t ZLX_CALL ochi_main (unsigned int argc, uint8_t const * const * argv)
{
    ochi_t ochi;
    uint8_t rc;

#if _DEBUG
    fprintf(stderr, "ochi (an=%u, a0=%s)!\n", argc, argv[0]);
    fprintf(stderr, "* using %s\n", zlx_lib_name);
    fprintf(stderr, "* using %s\n", hbs_lib_name);
    fprintf(stderr, "* using %s\n", acx1_name());
    fprintf(stderr, "* using %s\n", twx_lib_name);
    acx1_logging(3, stderr);
#endif

    do
    {
        rc = ochi_init(&ochi, argc, argv);
        L("ochi_init: %u", rc);
        if (rc) break;

        switch (ochi.app_mode)
        {
        case OAM_HELP:
            printf(
 "ochi - binary inspection tool - by Costin Ionescu\n"
 "Usage: ochi [OPTIONS] [FILES]\n"
                   );
            break;
        case OAM_UI:
            rc = ui_init(&ochi);
            if (rc) break;
            L("ui_init: %u", rc);

            rc = ui_run(&ochi);
            L("ui_run: %u", rc);
            break;
        }
    }
    while (0);

    ochi_finish(&ochi);
    if (ochi.err_msg[0])
    {
        fprintf(stderr, "ochi error: %s\n", ochi.err_msg);
    }

    return 0;
}

/* root_handler *************************************************************/
twx_status_t ZLX_CALL root_handler (twx_win_t * win, unsigned int evt, 
                                    twx_event_info_t * ei)
{
    ochi_t * ochi = (ochi_t *) win;
    twx_status_t ts = TWX_OK;
    unsigned int sr, sc, h, w;
    uint32_t km;

    L("enter root_handler(evt=%s)", twx_event_name(evt));
    switch (evt)
    {
    case TWX_GEOM:
        sr = ei->geom.scr_row;
        sc = ei->geom.scr_col;
        h = ei->geom.height;
        w = ei->geom.width;
        twx_win_write_geom(win, sr, sc, h, w);
        TC(twx_win_geom(ochi->top_bar, sr, sc, 1, w));
        TC(twx_win_geom(ochi->status_bar, sr + h - 1, sc, 1, w));
        if (h > 2)
        {
            TC(twx_win_geom(ochi->content_win, sr + 1, sc, h - 2, w));
        }
        break;

    case TWX_DRAW:
        L("drawing context...");
        TC(twx_win_draw(ochi->content_win));
        L("drawing top bar...");
        TC(twx_win_draw(ochi->top_bar));
        if (ochi->mode == OM_NORMAL) 
        {
            L("drawing status bar...");
            TC(twx_win_draw(ochi->status_bar));
        }
        if (ochi->mode == OM_CMD) 
        {
            L("drawing cmd bar...");
            TC(twx_win_draw(ochi->cmd_win));
        }
        break;

    case TWX_INVALIDATE:
        twx_win_refresh(ochi->top_bar);
        twx_win_refresh(ochi->content_win);
        if (ochi->mode == OM_NORMAL) twx_win_refresh(ochi->status_bar);
        else if (ochi->mode == OM_CMD) twx_win_refresh(ochi->cmd_win);
        break;

    case TWX_KEY:
        km = ei->km;
        if (km == (ACX1_ALT | 'x'))
        {
            twx_shutdown(win->twx, TWX_OK);
        }
        else if (km == ':')
        {
            TC(twx_itxt_win_create(ochi->twx, &ochi->cmd_win,
                                   &ochi->cmd_attr[0], 
                                   ZLX_ITEM_COUNT(ochi->cmd_attr),
                                   "cmd: ", 
                                   "cucu いろはにほ rucu ch:中文 ja:日本語",
                                   win, 0));
            TC(twx_win_geom(ochi->cmd_win, win->scr_row + win->height - 1, 
                            win->scr_col, 1, win->width / 3));
            TC(twx_win_focus(ochi->cmd_win));
            //twx_refresh(ochi->twx, TWX_UPDATE);
            ochi->mode = OM_CMD;
        }
        break;

    case TWX_ITXT_ENTERED:
        if (ei->id == 0)
        {
            L("cmd text entered");
            TC(twx_win_focus(win));
            twx_win_destroy(ochi->cmd_win);
            ochi->mode = OM_NORMAL;
            twx_win_refresh(ochi->status_bar);
        }
        break;

    case TWX_ITXT_CANCELLED:
        if (ei->id == 0)
        {
            L("cmd text cancelled");
            TC(twx_win_focus(win));
            twx_win_destroy(ochi->cmd_win);
            ochi->mode = OM_NORMAL;
            twx_win_refresh(ochi->status_bar);
        }
        break;

    default:
        ts = twx_default_handler(win, evt, ei);
    }

    L("root_handler(evt=%s) -> %u", twx_event_name(evt), ts);
    return ts;
}


#pragma once
#include "editMode.h"
#include "palette.h"
#include "RPoint.h"
#include "RRect.h"
#include "RHandle.h"

class editModeSelect: public editMode
{
    public:
        editModeSelect();
        virtual ~editModeSelect();

        bool    on_button_press_event(GdkEventButton *event);
        bool    on_button_release_event(GdkEventButton *event);
        bool    on_motion_notify_event(GdkEventMotion *event);
        bool    on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
        void    init_mode();

        RHandle *HitHandle(int mx,int my);
        void    InitHandles();


    private:
        Gdk::Point	*m_start_pt;
        Gdk::Point	*m_end_pt;
        int         m_x1, m_y1, m_x2, m_y2;

        bool        m_f_draw_handles;
        RHandle     *m_handles[4];
        RHandle     *m_select_handle;
        guint32     m_handle_color;

        int         m_start_pix_x,m_start_pix_y;

};
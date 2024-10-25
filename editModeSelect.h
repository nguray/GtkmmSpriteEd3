#pragma once
#include "editMode.h"
#include "palette.h"
#include "RPoint.h"
#include "RRect.h"
#include "RHandle.h"
#include <memory>

class editModeSelect: public editMode
{
    public:
        editModeSelect();
        virtual ~editModeSelect();

        bool    on_button_press_event(Gtk::Widget *w, GdkEventButton *event);
        bool    on_button_release_event(Gtk::Widget *w, GdkEventButton *event);
        bool    on_motion_notify_event(Gtk::Widget *w, GdkEventMotion *event);
        bool    on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
        void    init_mode();

        std::shared_ptr<RHandle> HitHandle(int mx,int my);
        void    InitHandles();


    private:
        std::shared_ptr<Gdk::Point> m_start_pt;
        std::shared_ptr<Gdk::Point> m_end_pt;
        int         m_x1, m_y1, m_x2, m_y2;

        bool        m_f_draw_handles;
        std::shared_ptr<RHandle> m_handles[4];
        std::shared_ptr<RHandle> m_select_handle;
        guint32     m_handle_color;

        int         m_start_pix_x,m_start_pix_y;

};
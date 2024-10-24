#pragma once

#include "editMode.h"
#include "palette.h"

class editModePencil: public editMode
{
    public:
        editModePencil();
        virtual ~editModePencil();


        int     m_pixelX,m_pixelY;
        int     m_lastPixelX,m_lastPixelY;

        bool    on_button_press_event(Gtk::Widget *w, GdkEventButton *event);
        bool    on_button_release_event(Gtk::Widget *w, GdkEventButton *event);
        bool    on_motion_notify_event(Gtk::Widget *w, GdkEventMotion *event);
        bool    on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
        void    init_mode();
        //bool    PtInEditArea(int x, int y);


    protected:

};

#pragma once

#include "editMode.h"
#include "palette.h"
#include "RPoint.h"

class editModeFill: public editMode
{
    public:
        editModeFill();
        virtual ~editModeFill();

        bool    on_button_press_event(GdkEventButton *event);
        bool    on_button_release_event(GdkEventButton *event);
        bool    on_motion_notify_event(GdkEventMotion *event);
        bool    on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
        void    init_mode();


    protected:

    private:

        gboolean    floodFillGetColor(Glib::RefPtr<Gdk::Pixbuf> pixbuf, gint x, gint y,guint32 *col);
        void        plot_pixel(Glib::RefPtr<Gdk::Pixbuf> pixbuf,gint x,gint y, guchar red, guchar green, guchar blue, guchar alpha);
        void        FloodFill(Glib::RefPtr<Gdk::Pixbuf> pixbuf, gint fillX, gint fillY,guint32 fillColor);


};


#pragma once

#include "editMode.h"
#include "RHandle.h"
#include <memory>

class editModeEllipse: public editMode
{
    public:
        editModeEllipse();
        virtual ~editModeEllipse();

        void    DrawHorizontalLine(Glib::RefPtr<Gdk::Pixbuf> pixbuf, int xLeft, int xRight, int y, guint32 col);
        void    FillEllipse(Glib::RefPtr<Gdk::Pixbuf> pixbuf, int left, int top, int right, int bottom, guint32 col);
        void    DrawFillEllipse(Glib::RefPtr<Gdk::Pixbuf> pixbuf, int startX,int startY, int endX, int endY, guint32 col);

        void    BorderEllipse(Glib::RefPtr<Gdk::Pixbuf> pixbuf, int left,
                                  int top, int right, int bottom, guint32 col);

        void    DrawEllipse(Glib::RefPtr<Gdk::Pixbuf> pixbuf,
                                      int startX, int startY, int endX,
                                      int endY, guint32 col);

        bool    on_button_press_event(Gtk::Widget *w, GdkEventButton *event) override;
        bool    on_button_release_event(Gtk::Widget *w, GdkEventButton *event) override;
        bool    on_motion_notify_event(Gtk::Widget *w, GdkEventMotion *event) override;
        bool    on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
        void    init_mode() override;
        
        std::shared_ptr<RHandle> HitHandle(int mx,int my);
        void    InitHandles();
        bool    RectInEditArea(RRect &rect);

    protected:

    private:
        std::shared_ptr<Gdk::Point>	m_start_pt;
        std::shared_ptr<Gdk::Point>	m_end_pt;
        int         m_x1, m_y1, m_x2, m_y2;
        std::shared_ptr<RHandle> m_handles[4];
        std::shared_ptr<RHandle> m_select_handle;
        guint32     m_handle_color;

        int         m_start_pix_x,m_start_pix_y;
        RRect       m_rect_current_pix;
        RRect       m_rect_current_pix_sav;
        guint       m_current_color;
        bool        m_current_fill_mode;

};

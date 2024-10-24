#pragma once

#include "gtkmm.h"
#include "RRect.h"
#include <vector>

class editMode
{
public:

    typedef sigc::signal<void, Glib::RefPtr<Gdk::Pixbuf>> type_signal_new_sprite;
    type_signal_new_sprite signal_new_sprite();
    type_signal_new_sprite      m_signal_new_sprite;

    editMode();
    virtual ~editMode();

    static int  m_offset;
    static int  m_nbHPixels;
    static int  m_nbVPixels;

    //-- Cairo Draw origin and scale
    static int      m_origin_x;
    static int      m_origin_y;
    static double   m_scale;

    //-- For moving cairo draw origin
    static int      m_start_origin_x;
    static int      m_start_origin_y;
    static int      m_start_x;
    static int      m_start_y;

    static guint32     m_foreGroundColor;
    static guint32     m_backGroundColor;

    static bool                        m_f_new_sprite;
    static Glib::RefPtr<Gdk::Pixbuf>   m_sprite;
    
    static Glib::RefPtr<Gdk::Pixbuf>   m_select_pixbuf;
    static RRect                       m_rect_copy_pix;
    static Glib::RefPtr<Gdk::Pixbuf>   m_copy_pixbuf;
    static RRect                       m_rect_select_pix_sav;
    static Glib::RefPtr<Gdk::Window>   m_refGdkWindow;

    static Glib::RefPtr<Gdk::Pixbuf>   m_start_state;


    void    SaveStartState();
    void    RestoreStartState();

    bool    MouseToPixel(int mx,int my,int &pixelX,int &pixelY);
    bool    PixelToMouse(int pixelX,int pixelY,int &mouseX,int &mouseY);
    void    put_pixel (Glib::RefPtr<Gdk::Pixbuf> pixbuf, int x, int y,
                            guchar red, guchar green, guchar blue, guchar alpha);
    void    get_pixel (Glib::RefPtr<Gdk::Pixbuf> pixbuf, int x, int y,
                            guchar *red, guchar *green, guchar *blue, guchar *alpha);
    void    CopyPixBuf(Glib::RefPtr<Gdk::Pixbuf> destPixbuf,Glib::RefPtr<Gdk::Pixbuf> srcPixbuf);

    void    BlitPixBuf(Glib::RefPtr<Gdk::Pixbuf> destPixbuf,int destX,int destY,
                        Glib::RefPtr<Gdk::Pixbuf> srcPixbuf,RRect srcRect);
    void    FillPixBuf(Glib::RefPtr<Gdk::Pixbuf> destPixbuf,RRect destRect,guint32 fillColor);
    
    void    line(Glib::RefPtr<Gdk::Pixbuf> pixbuf, gint x0, gint y0,gint x1,gint y1,guint32 c);

    bool    PtInEditArea(int x, int y);

    void    flip_horizontaly(Glib::RefPtr<Gdk::Pixbuf> pixbufSrc,Glib::RefPtr<Gdk::Pixbuf> pixbufDes);
    void    flip_verticaly(Glib::RefPtr<Gdk::Pixbuf> pixbufSrc,Glib::RefPtr<Gdk::Pixbuf> pixbufDes);
    void    rotate_left(Glib::RefPtr<Gdk::Pixbuf> pixbufSrc,Glib::RefPtr<Gdk::Pixbuf> pixbufDes);
    void    rotate_right(Glib::RefPtr<Gdk::Pixbuf> pixbufSrc,Glib::RefPtr<Gdk::Pixbuf> pixbufDes);

    static int     m_select_state;
    static RRect   m_rect_select_pix;
    static bool    m_select_move_flag;

    virtual bool on_button_press_event(Gtk::Widget *w, GdkEventButton *event)=0;
    virtual bool on_button_release_event(Gtk::Widget *w, GdkEventButton *event)=0;
    virtual bool on_motion_notify_event(Gtk::Widget *w, GdkEventMotion *event)=0;
    virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr)=0;
    virtual void init_mode()=0;

protected:


};

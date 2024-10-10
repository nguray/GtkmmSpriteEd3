#pragma once

#include <gtkmm.h>
#include <gtkmm/cssprovider.h>
#include "editMode.h"
#include "editModePencil.h"
#include "editModeRect.h"
#include "editModeEllipse.h"
#include "editModeFill.h"
#include "editModeSelect.h"

class editArea : public Gtk::Widget
{
    public:

        enum{
            EM_SELECT=1,
            EM_PENCIL,
            EM_RECTANGLE,
            EM_ELLIPSE,
            EM_FILL,
            EM_PICK_COLOR
        };

        editArea();
        ~editArea();

        //signal accessor:
        typedef sigc::signal<void, gint,guint32> type_signal_forecolor_pick;
        type_signal_forecolor_pick signal_forecolor_pick();

        typedef sigc::signal<void> type_signal_sprite_modified;
        type_signal_sprite_modified signal_sprite_modified();

        typedef sigc::signal<void, guint32> type_signal_backcolor_pick;
        type_signal_backcolor_pick signal_backcolor_pick();

        typedef sigc::signal<void, Glib::RefPtr<Gdk::Pixbuf>> type_signal_new_sprite;
        type_signal_new_sprite signal_new_sprite();


        void    on_palette_color_select(bool f, guint32 selColor);
        void    SetSprite(Glib::RefPtr<Gdk::Pixbuf> sprite);
        void    SetForegroundColor(guint32 newColor);
        void    SetBackgroundColor(guint32 newColor);
        void    SetEditMode(int imode);

        void    CopySelect();
        void    PasteCopy();
        void    CutSelect();
        void    Undo();
        void    Redo();
        
        void    flip_horizontaly();
        void    flip_verticaly();
        void    rotate_left();
        void    rotate_right();


        int     m_mode;
        int     m_id_palette_color;
        bool    m_f_pick_color_mode;
        void    on_pick_color_mode(int id_color);

        Glib::RefPtr<Gdk::Pixbuf> GetSprite();

        editMode        *m_edit_mode;
        editModePencil  m_edit_mode_pencil;
        editModeRect    m_edit_mode_rect;
        editModeEllipse m_edit_mode_ellipse;
        editModeFill    m_edit_mode_fill;
        editModeSelect  m_edit_mode_select;


    protected:

        type_signal_forecolor_pick  m_signal_forecolor_pick;
        type_signal_backcolor_pick  m_signal_backcolor_pick;
        type_signal_sprite_modified m_signal_sprite_modified;
        type_signal_new_sprite      m_signal_new_sprite;


        Glib::RefPtr<Gdk::Window>           m_refGdkWindow;
        Glib::RefPtr<Gtk::CssProvider>      m_refStyleProvider;

        virtual void get_preferred_width_vfunc(int& minimum_width, int& natural_width) const;
        virtual void get_preferred_height_for_width_vfunc(int width, int& minimum_height, int& natural_height) const;
        virtual void get_preferred_height_vfunc(int& minimum_height, int& natural_height) const;
        virtual void get_preferred_width_for_height_vfunc(int height, int& minimum_width, int& natural_width) const;

        virtual void on_size_allocate(Gtk::Allocation& allocation);
        virtual void on_map();
        virtual void on_unmap();
        virtual void on_realize();
        virtual void on_unrealize();
        virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
        virtual bool on_button_press_event(GdkEventButton *event);
        virtual bool on_button_release_event(GdkEventButton *event);
        virtual bool on_motion_notify_event(GdkEventMotion *event);
        virtual bool on_scroll_event(GdkEventScroll *event);
        virtual bool on_key_press_event (GdkEventKey* key_event);
        virtual bool on_key_release_event (GdkEventKey* key_event);

        void     on_image_received(const Glib::RefPtr<Gdk::Pixbuf>& pixbuf);


    private:

        Glib::RefPtr<Gdk::Cursor> 			m_refPickColorCursor;

        //bool    MouseToPixel(int mx,int my,int &pixelX,int &pixelY);
        void    get_pixel (Glib::RefPtr<Gdk::Pixbuf> pixbuf, int x, int y, guchar *red, guchar *green, guchar *blue, guchar *alpha);
        void    put_pixel (Glib::RefPtr<Gdk::Pixbuf> pixbuf, int x, int y, guchar red, guchar green, guchar blue, guchar alpha);

};


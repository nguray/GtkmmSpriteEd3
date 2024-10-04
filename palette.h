#pragma once

#include <gtkmm.h>


class palette : public Gtk::Widget
{
    public:

        enum
        {
            RGB_R_MASK = 0xFF << 24,
            RGB_G_MASK = 0xFF << 16,
            RGB_B_MASK = 0xFF << 8,
            RGB_A_MASK = 0xFF
        };
        
        palette();
        ~palette();

        //signal accessor:
        typedef sigc::signal<void, bool, guint32> type_signal_color_select;
        type_signal_color_select signal_color_select();

        typedef sigc::signal<void, int> type_signal_pick_color;
        type_signal_pick_color signal_pick_color();

        bool        m_f_pick_color_mode;

        guint32     m_foreGroundColor;
        guint32     m_backGroundColor;

        Gdk::RGBA   m_selColor;

        static guint8  GetRGBAlpha(guint32 rgb);

        static guint8  GetRGBRed(guint32 rgb);

        static guint8  GetRGBGreen(guint32 rgb);

        static guint8  GetRGBBlue(guint32 rgb);

        static guint32 RGBA(guint8 r,guint8 g,guint8 b,guint8 a);
        
        void on_set_foreground_color(gint id,guint32 color);

        void on_set_background_color(guint32 color);

        void Save(std::string pathName, std::string fileName);
        bool Load(std::string pathName, std::string fileName);


    protected:

        type_signal_color_select m_signal_color_select;
        type_signal_pick_color m_signal_pick_color;

        int     Coordinates2Index(int x,int y);
        bool    Coordinates2Color(int x,int y,guint32 &selColor);
        void    DrawCellFrame(const Cairo::RefPtr<Cairo::Context>& cr,int x,int y,int cs);


        Glib::RefPtr<Gdk::Window>         m_refGdkWindow;
        Glib::RefPtr<Gtk::CssProvider>    m_refStyleProvider;

        gint            m_cellSize;
        gint            m_nbRows,m_nbColumns;

        guint32         *m_tblColors;

        gint            m_pick_id;
        Gtk::Menu       *m_Menu_Popup;
        Gtk::MenuItem   *m_pick_color_item;

        Glib::RefPtr<Gdk::Cursor> 	m_refPickColorCursor;
 
        void    on_menu_popup_pick_color();

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
        virtual bool on_motion_notify_event(GdkEventMotion *event);


    private:


};


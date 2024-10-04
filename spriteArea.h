#pragma once

#include <gtkmm.h>
#include <vector>

class spriteArea : public Gtk::Widget
{
    public:
        spriteArea();
        ~spriteArea();

        //signal accessor:
        typedef sigc::signal<void, Glib::RefPtr<Gdk::Pixbuf>> type_signal_sprite_pick;
        type_signal_sprite_pick signal_sprite_pick();

        int     m_cellSize;
        int     m_nbCells;

        int     m_top, m_bottom;
        int     m_left, m_right;
        int     m_sprite_width;
        int     m_sprite_height;
        int     m_id_select;

        Glib::RefPtr<Gdk::Pixbuf> m_liste_sprites[8];
		Glib::ustring 	m_liste_names[8];
		bool			m_liste_modif_flags[8];

        int     CoordYtoIndex(int y);
		void 	SetSprite(Glib::RefPtr<Gdk::Pixbuf> newSprite);
		Glib::RefPtr<Gdk::Pixbuf>	GetSprite();
		Glib::ustring 				GetSpriteName();
		void 						SetSpriteName(Glib::ustring name);
        bool                        CreateNewSprite();

        void    on_sprite_modified();

        void    PlaySequence();

        void    PauseSequence();

        void    InitSprites();
        void    SaveSpritesProj(std::string pathName,std::string fileName);
        void    LoadSpritesProj(std::string fullPathName);


        int     m_i_sequence;
        sigc::connection m_connection_timer;
        bool    on_timeout(int timer_number);

    protected:

        type_signal_sprite_pick m_signal_sprite_pick;

        Glib::RefPtr<Gdk::Window>         m_refGdkWindow;
        Glib::RefPtr<Gtk::CssProvider>    m_refStyleProvider;

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

        int             m_y_right_click;
        Gtk::Menu       *m_Menu_Popup;
        Gtk::MenuItem   *m_move_up_item;
        Gtk::MenuItem   *m_move_down_item;
        Gtk::MenuItem   *m_delete_item;
        Gtk::MenuItem   *m_separator_item;

        void    on_menu_popup_move_up();
        void    on_menu_popup_move_down();
        void    on_menu_popup_delete();

    private:
    
};


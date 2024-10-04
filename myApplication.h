
#pragma once

#include <gtkmm.h>
#include "editArea.h"
#include "spriteArea.h"
#include "palette.h"

class myApplication : public Gtk::Application
{
protected:
    myApplication();

public:
    static Glib::RefPtr<myApplication> create();

    Glib::RefPtr<Gio::SimpleActionGroup> actionGroup;
    Glib::RefPtr<Gtk::IconTheme>        m_iconTheme;
    Glib::RefPtr<Gdk::Pixbuf> m_pauseImage;
    Glib::RefPtr<Gdk::Pixbuf> m_playImage;
    Glib::RefPtr<Gdk::Pixbuf> m_playIcon;
    bool    m_f_play;
    Gtk::ToolButton *m_playButton;
    Gtk::ToolButton *m_SelectToolButton;
    Gtk::ToolButton *m_PencilToolButton;
    Gtk::ToolButton *m_RectangleToolButton;
    Gtk::ToolButton *m_CircleToolButton;
    Gtk::ToolButton *m_FloodFillToolButton;
    Gtk::ToolButton *m_CurrentToolButton;

    Gtk::Window*        m_window;
    Gtk::AboutDialog    m_Dialog;
    editArea            m_edit_area;
    palette             m_palette;
    spriteArea          m_sprite_area;
    unsigned            m_ContextId;
    Gtk::Statusbar      m_Statusbar;

    Gtk::VBox           m_vbox1;
    Gtk::HBox           m_hbox1;

    void    resetToolbarIcons(); 
    void    on_set_edit_sprite(Glib::RefPtr<Gdk::Pixbuf> sprite);

protected: 
    // Override default signal handlers:
    void    on_activate() override;
    void    on_open(const Gio::Application::type_vec_files& files,
                    const Glib::ustring& hint) override;
    void    on_drop_drag_data_received(
                    const Glib::RefPtr<Gdk::DragContext>& context, int, int,
                    const Gtk::SelectionData& selection_data, guint, guint time);

private:
    void    on_action_quit();
    void    on_select_mode();
    void    on_pencil_mode();
    void    on_rectangle_mode();
    void    on_ellipse_mode();
    void    on_fill_mode();
    void    on_play_mode();
    void    on_menu_new();
    void    on_menu_open();
    void    on_menu_file_save();
    void    on_menu_file_save_as();
    void    on_menu_about();
    void    on_about_dialog_response(int response_id);
    void    on_action_edit_undo();
    void    on_action_edit_cut();
    void    on_action_edit_copy();
    void    on_action_edit_paste();
    void    on_action_flip_verticaly();
    void    on_action_flip_horizontaly();
    void    on_action_swing_left();
    void    on_action_swing_right();

};
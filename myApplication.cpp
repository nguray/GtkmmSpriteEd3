
#include "myApplication.h"
#include "gdk/gdk.h"
#include "glibmm/refptr.h"
#include "gtkmm/builder.h"
#include "gtkmm/filefilter.h"
#include "gtkmm/window.h"
#include <iostream>


extern Glib::RefPtr<Gtk::Builder> builder;

const Glib::ustring app_title = "SpriteEd2";

myApplication::myApplication():
    Gtk::Application("org.gtkmm.examples", Gio::APPLICATION_HANDLES_OPEN),m_f_play(false)
{

}

Glib::RefPtr<myApplication> myApplication::create()
{
    return Glib::RefPtr<myApplication> (new myApplication());
}

void myApplication::on_action_quit()
{
    auto windows = get_windows();
    for(auto w : windows){
        w->hide();
    }
    quit();
}

void myApplication::resetToolbarIcons()
{
    //-------------------------------------------------
    m_SelectToolButton->set_icon_name("SelectBox");
    m_PencilToolButton->set_icon_name("Pencil");
    m_RectangleToolButton->set_icon_name("Rectangle");
    m_CircleToolButton->set_icon_name("Ellipse");
    m_FloodFillToolButton->set_icon_name("FloodFill");

}

void myApplication::on_select_mode()
{
    std::cout << "Edit mode select." << std::endl;
    m_Statusbar.push("Select tool", m_ContextId);   
    m_edit_area.SetEditMode(editArea::EM_SELECT);
    resetToolbarIcons();
    m_SelectToolButton->set_icon_name("SelectBoxD");
    m_CurrentToolButton->set_icon_name("SelectBox");

}

void myApplication::on_pencil_mode()
{
    std::cout << "Edit mode pencil." << std::endl;
    m_Statusbar.push("Pencil tool", m_ContextId);   
    m_edit_area.SetEditMode(editArea::EM_PENCIL);
    resetToolbarIcons();
    m_PencilToolButton->set_icon_name("PencilD");
    m_CurrentToolButton->set_icon_name("Pencil");

}

void myApplication::on_rectangle_mode()
{
    std::cout << "Edit mode rectangle." << std::endl;
    m_Statusbar.push("Draw Rectangle", m_ContextId);   
    m_edit_area.SetEditMode(editArea::EM_RECTANGLE);
    resetToolbarIcons();
    m_RectangleToolButton->set_icon_name("RectangleD");
    m_CurrentToolButton->set_icon_name("Rectangle");

}

void myApplication::on_ellipse_mode()
{
    std::cout << "Edit mode ellipse." << std::endl;
    m_Statusbar.push("Draw Ellipse", m_ContextId);   
    m_edit_area.SetEditMode(editArea::EM_ELLIPSE);
    resetToolbarIcons();
    m_CircleToolButton->set_icon_name("EllipseD");
    m_CurrentToolButton->set_icon_name("Ellipse");
  
}

void myApplication::on_fill_mode()
{
    std::cout << "Edit mode ellipse." << std::endl;
    m_Statusbar.push("Fill tool", m_ContextId);   
    m_edit_area.SetEditMode(editArea::EM_FILL);
    resetToolbarIcons();
    m_FloodFillToolButton->set_icon_name("FloodFillD");
    m_CurrentToolButton->set_icon_name("FloodFill");

}

void myApplication::on_play_mode()
{
    //std::cout << "Play tool" << std::endl;
    //m_Statusbar.push("Play tool", m_ContextId);

    if (!m_f_play){
        m_f_play = true;
        m_playButton->set_icon_name("PauseTool");
        m_sprite_area.PlaySequence();

    }else{
        
        m_f_play = false;
        m_playButton->set_icon_name("PlayTool");
        m_sprite_area.PauseSequence();

    }
 
}

void myApplication::on_menu_new()
{
    //-----generate-header---------------------------------------------------------
    std::cout << "on_menu_new" << std::endl;
    m_sprite_area.CreateNewSprite();

}

void myApplication::on_menu_open()
{
    //------------------------------------------------------------
    Gtk::FileChooserDialog dlg("Please choose a file", Gtk::FILE_CHOOSER_ACTION_OPEN);
    dlg.set_transient_for(*m_window);

    //-- Add response buttons
    dlg.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
    dlg.add_button("_Open", Gtk::RESPONSE_OK);

    //-- Add filters
    Glib::RefPtr<Gtk::FileFilter> filter_pixbuf = Gtk::FileFilter::create();
    filter_pixbuf->set_name("All Images");
    filter_pixbuf->add_pixbuf_formats();
    dlg.add_filter(filter_pixbuf);
    //-- Show the dialog and wait for a user response
    int result = dlg.run();

    //-- Handle the response
    switch (result)
    {
    case Gtk::RESPONSE_OK:
        {
            Glib::ustring name;
            Glib::ustring file_name;
            file_name = dlg.get_filename();
            Glib::RefPtr<Gdk::Pixbuf> pixBuf = Gdk::Pixbuf::create_from_file(file_name);
            m_sprite_area.SetSprite(pixBuf);
            m_sprite_area.SetSpriteName(file_name);
            name = "SpriteEd2 : " + Glib::path_get_basename( dlg.get_filename());
            m_window->set_title(name.c_str());
        }
        break;
    case Gtk::RESPONSE_CANCEL:
        {
        }
        break;
    }
    std::cout << "A File|Open menu item was selected." << std::endl;

}

void myApplication::on_menu_file_save()
{
    Glib::ustring file_name;
    //------------------------------------------------------------
    file_name = m_sprite_area.GetSpriteName();
    if (!file_name.empty())
    {
        Glib::RefPtr<Gdk::Pixbuf> pixBuf = m_edit_area.GetSprite();
        Glib::ustring tmpstr = file_name.uppercase();
        int l = tmpstr.length();
        if (tmpstr.find(".PNG", l - 4) == Glib::ustring::npos)
        {
            file_name += ".png";
        }
        pixBuf->save(file_name, "png");
    }
    else
    {
        on_menu_file_save_as();
    }
}

void myApplication::on_menu_file_save_as()
{
    //------------------------------------------------------------
    Gtk::FileChooserDialog dlg("Please choose a file", Gtk::FILE_CHOOSER_ACTION_SAVE);
    dlg.set_transient_for(*m_window);

    //-- Add response buttons
    dlg.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
    dlg.add_button("_Save", Gtk::RESPONSE_OK);

    //-- Add filters
    Glib::RefPtr<Gtk::FileFilter> filter_pixbuf = Gtk::FileFilter::create();
    filter_pixbuf->set_name("All Images");
    filter_pixbuf->add_pixbuf_formats();
    dlg.add_filter(filter_pixbuf);

    //-- Show the dialog and wait for a user response
    int result = dlg.run();

    //-- Handle the response
    switch (result)
    {
    case Gtk::RESPONSE_OK:
        {
            Glib::ustring name;
            Glib::ustring file_name = dlg.get_filename();
            Glib::RefPtr<Gdk::Pixbuf> pixBuf = m_sprite_area.GetSprite();
            Glib::ustring tmpstr = file_name.uppercase();
            if (tmpstr.find_last_of(".PNG") == Glib::ustring::npos)
            {
                file_name += ".png";
            }
            pixBuf->save(file_name, "png");
            //--
            name = app_title + " : " + Glib::path_get_basename(file_name);
            m_window->set_title(name);
            m_sprite_area.SetSpriteName(file_name);

        }
        break;
    case Gtk::RESPONSE_CANCEL:
        {
        }
        break;
    }

    std::cout << "A File|Open menu item was selected." << std::endl;
}

void myApplication::on_menu_about()
{
    //-----------------------------------------------
 
    m_Dialog.show();
    //Bring it to the front, in case it was already shown:
    m_Dialog.present();

}

void myApplication::on_about_dialog_response(int response_id)
{
  std::cout << response_id
    << ", close=" << Gtk::RESPONSE_CLOSE
    << ", cancel=" << Gtk::RESPONSE_CANCEL
    << ", delete_event=" << Gtk::RESPONSE_DELETE_EVENT
    << std::endl;

  switch (response_id)
  {
  case Gtk::RESPONSE_CLOSE:
  case Gtk::RESPONSE_CANCEL:
  case Gtk::RESPONSE_DELETE_EVENT:
    m_Dialog.hide();
    break;
  default:
    std::cout << "Unexpected response!" << std::endl;
    break;
  }
}

void myApplication::on_action_edit_undo()
{
    std::cout << "A Edit|Undo menu item was selected." << std::endl;
    m_edit_area.Undo();
}

void myApplication::on_action_edit_cut()
{
    std::cout << "A Edit|Cut menu item was selected." << std::endl;
    m_edit_area.CutSelect();
}

void myApplication::on_action_edit_copy()
{
    std::cout << "A Edit|Copy menu item was selected." << std::endl;
    m_edit_area.CopySelect();
}

void myApplication::on_action_edit_paste()
{
    std::cout << "A Edit|Paste menu item was selected." << std::endl;
    m_Statusbar.push("Select tool", m_ContextId);   
    m_edit_area.PasteCopy();
}

void myApplication::on_action_flip_verticaly()
{
    //------------------------------------------------------------
    std::cout << "A Image|Flip Verticaly menu item was selected." << std::endl;
    m_edit_area.flip_verticaly();
}

void myApplication::on_action_flip_horizontaly()
{
    //------------------------------------------------------------
    std::cout << "A Image|Flip Horizontaly menu item was selected." << std::endl;
    m_edit_area.flip_horizontaly();
}

void myApplication::on_action_swing_left()
{
    //------------------------------------------------------------
    std::cout << "A Image|Rotate 90 Left menu item was selected." << std::endl;
    m_edit_area.rotate_left();
}

void myApplication::on_action_swing_right()
{
    //------------------------------------------------------------
    std::cout << "A Image|Rotate 90 Right menu item was selected." << std::endl;
    m_edit_area.rotate_right();
}

void myApplication::on_set_edit_sprite(Glib::RefPtr<Gdk::Pixbuf> sprite)
{
    Glib::ustring name;
    //------------------------------------------------------------
    Glib::ustring file_name = m_sprite_area.GetSpriteName();
    if (!file_name.empty()){
        name = "SpriteEd2 : " + Glib::path_get_basename( file_name);
    }else{
        name = "SpriteEd2";
    }
    m_window->set_title(name.c_str());
    m_edit_area.SetSprite(sprite);

}

void myApplication::on_drop_drag_data_received(
        const Glib::RefPtr<Gdk::DragContext>& context, int v1, int v2,
        const Gtk::SelectionData& selection_data, guint v3, guint time)
{
    const int length = selection_data.get_length();
    if((length >= 0) && (selection_data.get_format() == 8))
    {
        auto tab = selection_data.get_uris();
        if (tab.size()){
            auto v = tab[0];
            auto filenamepath = Glib::filename_from_uri(v);
            if (Glib::str_has_suffix(filenamepath,"png")){
                Glib::RefPtr<Gdk::Pixbuf> pixBuf = Gdk::Pixbuf::create_from_file(filenamepath);
                m_sprite_area.SetSprite(pixBuf);
                m_sprite_area.SetSpriteName(filenamepath);
                //std::cout << "Received \"" << filenamepath << "\" in EditArea " << std::endl;
            }
        }

    }
    //--
    context->drag_finish(false, false, time);

}

void myApplication::on_activate()
{
    //--
    m_iconTheme = Gtk::IconTheme::get_default();
    m_iconTheme->add_resource_path("/res");

    //Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_resource("/res/gtkspriteed.glade");

    builder->get_widget("spriteedwin",m_window);
    m_window->set_default_size(640,736);
    m_window->set_position(Gtk::WIN_POS_CENTER);
    add_window(*m_window);
    m_window->present();

    m_pauseImage = Gdk::Pixbuf::create_from_resource("/res/PauseTool.png");
    m_playImage = Gdk::Pixbuf::create_from_resource("/res/PlayTool.png");
    //m_playIcon = Gdk::Pixbuf::create_from_resource("/res/PlayTool.png");

    // m_iconTheme = Gtk::IconTheme::create();
    // m_iconTheme->add_builtin_icon("SelectBoxIcon", 24, Gdk::Pixbuf::create_from_resource("/res/SelectBox.png"));
    // m_iconTheme->add_builtin_icon("SelectBoxDIcon", 24, Gdk::Pixbuf::create_from_resource("/res/SelectBoxD.png"));
    // m_iconTheme->add_builtin_icon("PencilIcon", 24, Gdk::Pixbuf::create_from_resource("/res/Pencil.png"));
    // m_iconTheme->add_builtin_icon("PencilDIcon", 24, Gdk::Pixbuf::create_from_resource("/res/PencilD.png"));
    // m_iconTheme->add_builtin_icon("RectangleIcon", 24, Gdk::Pixbuf::create_from_resource("/res/Rectangle.png"));
    // m_iconTheme->add_builtin_icon("RectangleDIcon", 24, Gdk::Pixbuf::create_from_resource("/res/RectangleD.png"));
    // m_iconTheme->add_builtin_icon("EllipseIcon", 24, Gdk::Pixbuf::create_from_resource("/res/Ellipse.png"));
    // m_iconTheme->add_builtin_icon("EllipseDIcon", 24, Gdk::Pixbuf::create_from_resource("/res/EllipseD.png"));
    // m_iconTheme->add_builtin_icon("FloodFillIcon", 24, Gdk::Pixbuf::create_from_resource("/res/FloodFill.png"));
    // m_iconTheme->add_builtin_icon("FloodFillDIcon", 24, Gdk::Pixbuf::create_from_resource("/res/FloodFillD.png"));
    // m_iconTheme->add_builtin_icon("PlayToolIcon", 24, Gdk::Pixbuf::create_from_resource("/res/PlayTool.png"));
    // m_iconTheme->add_builtin_icon("PauseToolIcon", 24, Gdk::Pixbuf::create_from_resource("/res/PauseTool.png"));


    builder->get_widget("PlayTool",m_playButton);
    builder->get_widget("SelectTool",m_SelectToolButton);
    builder->get_widget("PencilTool",m_PencilToolButton);
    builder->get_widget("RectangleTool",m_RectangleToolButton);
    builder->get_widget("CircleTool",m_CircleToolButton);
    builder->get_widget("FloodFillTool",m_FloodFillToolButton);
    builder->get_widget("CurrentTool",m_CurrentToolButton);

    Gtk::Box       *vbox;
    builder->get_widget("vbox",vbox);

 
    actionGroup = Gio::SimpleActionGroup::create();


    actionGroup->add_action("quit",sigc::mem_fun(*this,&myApplication::on_action_quit));
    actionGroup->add_action("selectmode",sigc::mem_fun(*this,&myApplication::on_select_mode));
    actionGroup->add_action("pencilmode",sigc::mem_fun(*this,&myApplication::on_pencil_mode));
    actionGroup->add_action("rectanglemode",sigc::mem_fun(*this,&myApplication::on_rectangle_mode));
    actionGroup->add_action("ellipsemode",sigc::mem_fun(*this,&myApplication::on_ellipse_mode));
    actionGroup->add_action("fillmode",sigc::mem_fun(*this,&myApplication::on_fill_mode));
    actionGroup->add_action("playmode",sigc::mem_fun(*this,&myApplication::on_play_mode));
   
    actionGroup->add_action("new",sigc::mem_fun(*this,&myApplication::on_menu_new));
    actionGroup->add_action("open",sigc::mem_fun(*this,&myApplication::on_menu_open));
    actionGroup->add_action("save",sigc::mem_fun(*this,&myApplication::on_menu_file_save));
    actionGroup->add_action("saveas",sigc::mem_fun(*this,&myApplication::on_menu_file_save_as));
    actionGroup->add_action("about",sigc::mem_fun(*this,&myApplication::on_menu_about));

    actionGroup->add_action("undo",sigc::mem_fun(*this,&myApplication::on_action_edit_undo));
    actionGroup->add_action("copy",sigc::mem_fun(*this,&myApplication::on_action_edit_copy));
    actionGroup->add_action("paste",sigc::mem_fun(*this,&myApplication::on_action_edit_paste));
    actionGroup->add_action("cut",sigc::mem_fun(*this,&myApplication::on_action_edit_cut));

    actionGroup->add_action("horizontal_symmetry",sigc::mem_fun(*this,&myApplication::on_action_flip_horizontaly));
    actionGroup->add_action("vertical_symmetry",sigc::mem_fun(*this,&myApplication::on_action_flip_verticaly));
    actionGroup->add_action("rotate_left",sigc::mem_fun(*this,&myApplication::on_action_swing_left));
    actionGroup->add_action("rotate_right",sigc::mem_fun(*this,&myApplication::on_action_swing_right));


    m_window->insert_action_group("actions", actionGroup);

    //--
    m_Dialog.set_transient_for(*m_window);
    m_Dialog.set_logo(Gdk::Pixbuf::create_from_resource("/res/FloodFill.png", -1, 40, true));
    m_Dialog.set_program_name("GtkSpriteEd");
    m_Dialog.set_version("0.9.0");
    m_Dialog.set_copyright("Raymond NGUYEN THANH");
    m_Dialog.set_comments("This is just an small Sprite Editor.");
    m_Dialog.set_license("LGPL");
    m_Dialog.signal_response().connect(
            sigc::mem_fun(*this, &myApplication::on_about_dialog_response) );


    m_hbox1.pack_start(m_edit_area,Gtk::PACK_EXPAND_WIDGET);
    m_hbox1.pack_start(m_sprite_area,Gtk::PACK_SHRINK);

    vbox->pack_start(m_hbox1,Gtk::PACK_EXPAND_WIDGET);
    vbox->pack_start(m_palette,Gtk::PACK_SHRINK);

    m_palette.signal_color_select().connect(sigc::mem_fun(m_edit_area,
            &editArea::on_palette_color_select) );

    m_ContextId = m_Statusbar.get_context_id("SpriteEd2");
    vbox->pack_start(m_Statusbar,Gtk::PACK_SHRINK);

    m_edit_area.SetForegroundColor(m_palette.m_foreGroundColor);
    m_edit_area.SetBackgroundColor(m_palette.m_backGroundColor);

    m_edit_area.signal_forecolor_pick().connect(sigc::mem_fun(m_palette,
                     &palette::on_set_foreground_color) );
    m_edit_area.signal_backcolor_pick().connect(sigc::mem_fun(m_palette,
                     &palette::on_set_background_color) );
    m_edit_area.signal_sprite_modified().connect(sigc::mem_fun(m_sprite_area,
                     &spriteArea::on_sprite_modified) );
    m_edit_area.m_edit_mode->signal_new_sprite().connect(sigc::mem_fun(m_sprite_area,
                     &spriteArea::on_new_sprite) );

    m_edit_area.SetSprite(m_sprite_area.m_liste_sprites[0]);

    m_sprite_area.signal_sprite_pick().connect(sigc::mem_fun(this,
                        &myApplication::on_set_edit_sprite) );

    m_palette.signal_pick_color().connect(sigc::mem_fun(m_edit_area,
                        &editArea::on_pick_color_mode) );


    m_window->show_all_children();
    
    // builder->get_widget("Test",m_testDlg);
    // m_testDlg->set_transient_for(*m_window);
    // auto response = m_testDlg->run();

    std::vector< Gtk::TargetEntry > targets;
    targets.push_back( Gtk::TargetEntry("text/uri-list") );
    m_edit_area.drag_dest_set( targets, Gtk::DestDefaults::DEST_DEFAULT_ALL, Gdk::ACTION_COPY|Gdk::ACTION_MOVE|Gdk::ACTION_LINK);
    m_edit_area.signal_drag_data_received().connect(sigc::mem_fun( *this, &myApplication::on_drop_drag_data_received));

    on_pencil_mode();

    //auto mainWin = new mainWindow();
    //add_window(*mainWin);
    //mainWin->present();

}

void myApplication::on_open(const Gio::Application::type_vec_files& files,
    const Glib::ustring& /* hint */)
{

}

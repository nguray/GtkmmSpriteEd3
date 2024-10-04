#include "palette.h"
#include <iostream>
#include <fstream>

extern Glib::RefPtr<Gtk::Application>  app;

guint8 palette::GetRGBAlpha(guint32 rgb)
{
    return (rgb & RGB_A_MASK);
}

guint8 palette::GetRGBRed(guint32 rgb)
{
    return (rgb & RGB_R_MASK) >> 24;
}

guint8 palette::GetRGBGreen(guint32 rgb)
{
    return (rgb & RGB_G_MASK) >> 16;
}

guint8 palette::GetRGBBlue(guint32 rgb)
{
    return (rgb & RGB_B_MASK) >> 8;
}

guint32 palette::RGBA(guint8 r,guint8 g,guint8 b,guint8 a)
{
    return (b << 8) | (g << 16) | (r << 24) | (a);
}


palette::palette():Glib::ObjectBase("myPalette"),Gtk::Widget(),m_Menu_Popup(NULL),m_f_pick_color_mode(false)
{
    //ctor
    set_has_window(true);
//    gtk_widget_class_install_style_property(GTK_WIDGET_CLASS(
//        G_OBJECT_GET_CLASS(gobj())),
//        g_param_spec_int("myPalette",
//        "GtkSpriteEdit",
//        "The scale to use when drawing. This is just a silly example.",
//        G_MININT,
//        G_MAXINT,
//        500,
//        G_PARAM_READABLE) );
//
//        m_refStyleProvider = Gtk::CssProvider::create();
//        Glib::RefPtr<Gtk::StyleContext> refStyleContext = get_style_context();
//        refStyleContext->add_provider(m_refStyleProvider,
//        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
//
//    //Glib::ustring data = "myEditArea {color: #ff00ea;font: Comic Sans MS 12;}";
//    Glib::ustring data = "myPalette {color: #ff0000;font: Comic Sans MS 12;}";
//
//    try
//    {
//    //            m_refStyleProvider->load_from_path("custom_gtk.css");
//        m_refStyleProvider->load_from_data(data);
//    }
//    catch(const Glib::Error& ex)
//    {
//        std::cerr << "Gtk::CssProvider::load_from_data() failed: " << ex.what() << std::endl;
//    }

    m_cellSize = 24;
    m_nbRows = 2;
    m_nbColumns = 20;
    int n = m_nbRows*m_nbColumns;
    m_tblColors = (guint32 *) calloc(n,sizeof(guint32));

    int     i=0;
    m_tblColors[i++]= RGBA(0xFF,0xFF,0xFF,0x00);
    m_tblColors[i++]= RGBA(0x00,0x00,0x00,0xFF);
    m_tblColors[i++]= RGBA(0xFF,0xFF,0xFF,0xFF);
    m_tblColors[i++]= RGBA(0x80,0x80,0x80,0xFF);
    m_tblColors[i++]= RGBA(0xC0,0xC0,0xC0,0xFF);
    m_tblColors[i++]= RGBA(0x80,0x00,0x00,0xFF);
    m_tblColors[i++]= RGBA(0xFF,0x00,0x00,0xFF);
    m_tblColors[i++]= RGBA(0x80,0x80,0x00,0xFF);
    m_tblColors[i++]= RGBA(0xFF,0xFF,0x00,0xFF);
    m_tblColors[i++]= RGBA(0x00,0x80,0x00,0xFF);
    m_tblColors[i++]= RGBA(0x00,0xFF,0x00,0xFF);
    m_tblColors[i++]= RGBA(0x00,0x80,0x80,0xFF);
    m_tblColors[i++]= RGBA(0x00,0xFF,0xFF,0xFF);
    m_tblColors[i++]= RGBA(0x00,0x00,0x80,0xFF);
    m_tblColors[i++]= RGBA(0x00,0x00,0xFF,0xFF);
    m_tblColors[i++]= RGBA(0x80,0x00,0x80,0xFF);
    m_tblColors[i++]= RGBA(0xFF,0x00,0xFF,0xFF);

    m_foreGroundColor = m_tblColors[1];
    m_backGroundColor = m_tblColors[0];

    //--
    Load("","Default.pal");
    
	m_pick_color_item = new Gtk::MenuItem("Pick Color", true);
	m_pick_color_item->signal_activate().connect(sigc::mem_fun(*this, &palette::on_menu_popup_pick_color) );

}

palette::~palette()
{
    //dtor
    if (m_tblColors){

        Save("","Default.pal");
        free(m_tblColors);
    }
    if (m_Menu_Popup){
        delete m_Menu_Popup;
    }

}

void palette::get_preferred_width_vfunc(int& minimum_width, int& natural_width) const
{
  minimum_width = 26*16;
  natural_width = 26*16;
}

void palette::get_preferred_height_for_width_vfunc(int /* width */,
   int& minimum_height, int& natural_height) const
{
  minimum_height = 26;
  natural_height = 26;
}

void palette::get_preferred_height_vfunc(int& minimum_height, int& natural_height) const
{
  minimum_height = 26*2;
  natural_height = 26*2;
}

void palette::get_preferred_width_for_height_vfunc(int /* height */,
   int& minimum_width, int& natural_width) const
{
  minimum_width = 26*16;
  natural_width = 26*16;
}

void palette::on_size_allocate(Gtk::Allocation& allocation)
{
  //Do something with the space that we have actually been given:
  //(We will not be given heights or widths less than we have requested, though
  //we might get more)

  //Use the offered allocation for this container:
  set_allocation(allocation);

  if(m_refGdkWindow)
  {
    m_refGdkWindow->move_resize( allocation.get_x(), allocation.get_y(),
            allocation.get_width(), allocation.get_height() );
  }

}

void palette::on_map()
{
    // Call base class:
    Gtk::Widget::on_map();
}

void palette::on_unmap()
{
    // Call base class:
    Gtk::Widget::on_unmap();
}

void palette::on_realize()
{
    // Call base class:

    set_realized();

    if (!m_refGdkWindow){

        //--Create the GdkWindow
        GdkWindowAttr attributes;
        memset(&attributes,0,sizeof(attributes));

        Gtk::Allocation allocation = get_allocation();

        //-- Set initial position and size of the Gdk::Window
        attributes.x = allocation.get_x();
        attributes.y = allocation.get_y();
        attributes.width = allocation.get_width();
        attributes.height = allocation.get_height();

        attributes.event_mask = get_events() | Gdk::EXPOSURE_MASK | Gdk::BUTTON1_MASK | Gdk::BUTTON3_MASK
                                                | Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_RELEASE_MASK
                                                | Gdk::POINTER_MOTION_HINT_MASK;
        attributes.window_type = GDK_WINDOW_CHILD;
        attributes.wclass = GDK_INPUT_OUTPUT;

        m_refGdkWindow = Gdk::Window::create(get_parent_window(),&attributes,GDK_WA_X|GDK_WA_Y);

        set_window(m_refGdkWindow);

        //--
        m_refPickColorCursor =  Gdk::Cursor::create(m_refGdkWindow->get_display(),
                                    Gdk::Pixbuf::create_from_resource("/res/PickColor.png"), 4, 28);

        //--Set colors
        //modify_bg(Gtk::STATE_NORMAL, Gdk::Color("white"));
        //modify_fg(Gtk::STATE_NORMAL, Gdk::Color("blue"));

        // make the widget recieve exposure events
        m_refGdkWindow->set_user_data(gobj());

    }

}

void palette::on_unrealize()
{
    m_refGdkWindow.clear();

    //Call base class:
    Gtk::Widget::on_unrealize();
}

void palette::DrawCellFrame(const Cairo::RefPtr<Cairo::Context>& cr,int x,int y,int cs)
{
    //-----------------------------------------------------------
    cr->set_source_rgba( 0.5f, 0.5f, 0.5f, 1.0f);
    cr->set_line_width(1.0f);
    cr->move_to( x, y);
    cr->line_to( x+cs, y+cs);
    cr->move_to( x+cs, y);
    cr->line_to( x, y+cs);
    cr->move_to( x, y);
    cr->line_to( x+cs, y);
    cr->line_to( x+cs, y+cs);
    cr->line_to( x, y+cs);
    cr->line_to( x, y);
    cr->stroke();
    
}

bool palette::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    const int width = get_allocation().get_width();
    const int height = get_allocation().get_height();

    // paint the background
    Gdk::Cairo::set_source_rgba(cr, get_style_context()->get_background_color());
    cr->paint();

    cr->set_source_rgba( 1.0, 0.0, 1.0, 1.0);

//    cr->move_to( 0, 0);
//    cr->line_to( width, height);
//    cr->line_to( width, 0);
//    cr->line_to( 0, height);
//    cr->stroke();

    int ic,x,y;
    int x1,y1,cs;

    guint8  r,g,b,a;
    guint32 color;

    //--
    r = GetRGBRed(m_backGroundColor);
    g = GetRGBGreen(m_backGroundColor);
    b = GetRGBBlue(m_backGroundColor);
    a = GetRGBAlpha(m_backGroundColor);
    if (a==0){
        DrawCellFrame( cr, 1, 1, 2*m_cellSize - 4);
    }else{
        cr->set_source_rgba( r/255.0f, g/255.0f, b/255.0f, a/255.0f);
        cr->rectangle(1,1,2*m_cellSize-2,2*m_cellSize-2);
        cr->fill();
    }
    //--
    r = GetRGBRed(m_foreGroundColor);
    g = GetRGBGreen(m_foreGroundColor);
    b = GetRGBBlue(m_foreGroundColor);
    a = GetRGBAlpha(m_foreGroundColor);
    if (a==0){
        DrawCellFrame( cr, 1, 1, 1.2*m_cellSize);
    }else{
        cr->set_source_rgba( r/255.0f, g/255.0f, b/255.0f, a/255.0f);
        cr->rectangle(1,1,1.2*m_cellSize,1.2*m_cellSize);
        cr->fill();
    }
    //--
    ic = 0;
    for (unsigned int i=0;i<m_nbRows;i++){
        y = i*m_cellSize;
        for(unsigned int j=0;j<m_nbColumns;j++){
            x = j*m_cellSize + 2*m_cellSize;
            color = m_tblColors[ic++];
            r = GetRGBRed(color);
            g = GetRGBGreen(color);
            b = GetRGBBlue(color);
            a = GetRGBAlpha(color);
            if (a==0){
                DrawCellFrame( cr, x+1, y+1, m_cellSize - 2);

            }else{
                cr->set_source_rgba( r/255.0f, g/255.0f, b/255.0f, a/255.0f);
                cr->rectangle(x,y,m_cellSize-1,m_cellSize-1);
                cr->fill();

            }
        }
    }

    return true;
}

bool palette::Coordinates2Color(int x,int y,guint32 &selColor)
{
    int     ic = 0;
    int     xLeft,xRight,yTop,yBottom;
    //-----------------------------------------------------------
    for (unsigned int i=0;i<m_nbRows;i++){
        yTop = i*m_cellSize;
        yBottom = yTop + m_cellSize;
        for(unsigned int j=0;j<m_nbColumns;j++){
            xLeft = j*m_cellSize + 2*m_cellSize;
            xRight = xLeft + m_cellSize;
            if ((x>xLeft)&&(x<xRight)&&(y>yTop)&&(y<yBottom)){
                selColor = m_tblColors[ic];
                return true;
            }
            ic++;
        }
    }
    return false;
}

int palette::Coordinates2Index(int x,int y)
{
    int     ic = 0;
    int     xLeft,xRight,yTop,yBottom;
    //-----------------------------------------------------------
    for (unsigned int i=0;i<m_nbRows;i++){
        yTop = i*m_cellSize;
        yBottom = yTop + m_cellSize;
        for(unsigned int j=0;j<m_nbColumns;j++){
            xLeft = j*m_cellSize + 2*m_cellSize;
            xRight = xLeft + m_cellSize;
            if ((x>xLeft)&&(x<xRight)&&(y>yTop)&&(y<yBottom)){
                return ic;
            }
            ic++;
        }
    }
    return -1;
}

bool palette::on_button_press_event(GdkEventButton *event)
{
    int         x,y,id;
    guint8      r,g,b,a;
    guint32     selColor;
    Gtk::Window first_window;
    //Gdk::ModifierType   state= Gdk::ModifierType(0);
    //----------------------------------------------------------
    //m_refGdkWindow->get_pointer(x,y,state);

    if (event->button==1){
    	if (event->type==GDK_2BUTTON_PRESS){
            if ((id = Coordinates2Index(event->x,event->y))>0){
                //Gtk::ColorChooserDialog     dlg;
                Gtk::ColorSelectionDialog   dlg;
                dlg.set_transient_for(*(app->get_active_window()));
                dlg.set_title("Color Selection");
                Gtk::ColorSelection *colsel = dlg.get_color_selection();
                //dlg.set_position(Gtk::WIN_POS_CENTER_ALWAYS);
                r = GetRGBRed(m_foreGroundColor);
                g = GetRGBGreen(m_foreGroundColor);
                b = GetRGBBlue(m_foreGroundColor);
                a = GetRGBAlpha(m_foreGroundColor);
                m_selColor.set_red(r/255.0f);
                m_selColor.set_green(g/255.0f);
                m_selColor.set_blue(b/255.0f);
                m_selColor.set_alpha(a/255.0f);
                colsel->set_current_rgba(m_selColor);
                const int result = dlg.run();
                switch(result){
                case Gtk::RESPONSE_OK:
                    {
                        //Store the chosen color:
                        Gtk::ColorSelection *colsel = dlg.get_color_selection();
                        m_selColor = colsel->get_current_rgba();
                        r = 255*m_selColor.get_red();
                        g = 255*m_selColor.get_green();
                        b = 255*m_selColor.get_blue();
                        a = 255*m_selColor.get_alpha();
                        m_foreGroundColor = RGBA( r, g, b, a);
                         m_tblColors[id] = m_foreGroundColor;
                        m_signal_color_select.emit(true, m_foreGroundColor);
                        //--
                        Save("","Default.pal");
                        break;
                    }
                case Gtk::RESPONSE_CANCEL:
                    {
                        std::cout << "Cancel clicked." << std::endl;
                        break;
                    }
                default:
                    {
                        std::cout << "Unexpected button clicked: " << result << std::endl;
                    }
                }
            }
        }else{
            int mx = event->x;
            int my = event->y;
            if (Coordinates2Color(mx,my,selColor)){
                if (m_f_pick_color_mode){
                    m_f_pick_color_mode = false;
                   	m_signal_pick_color.emit(-1); // Stop pick color in Editarea
                	m_refGdkWindow->set_cursor();
                    m_tblColors[m_pick_id] = selColor;
                }else{
                    m_foreGroundColor = selColor;
                    m_signal_color_select.emit(true, selColor);
                }
            }else if ((mx>0)&&(mx<2*m_cellSize)&&(my>0)&&(my<2*m_cellSize)){
                if (m_f_pick_color_mode){
                    m_f_pick_color_mode = false;
                   	m_signal_pick_color.emit(-1); // Stop pick color in Editarea
                	m_refGdkWindow->set_cursor();
                    if ((mx>0)&&(mx<m_cellSize)){
                        m_tblColors[m_pick_id] = m_foreGroundColor;
                    }else{
                        m_tblColors[m_pick_id] = m_backGroundColor;
                    }
                }else{
                    //--Swap fore and back ground color
                    guint32     dum;
                    dum = m_foreGroundColor;
                    m_foreGroundColor = m_backGroundColor;
                    m_backGroundColor = dum;
                    m_signal_color_select.emit(true, m_foreGroundColor);
                    m_signal_color_select.emit(false, m_backGroundColor);
                }
            }
        }
    }else if (event->button==3){
        // if (Coordinates2Color(event->x,event->y,selColor)){
        //     m_backGroundColor = selColor;
        //     m_signal_color_select.emit(false, selColor);
        // }
        if (m_f_pick_color_mode){
            m_f_pick_color_mode = false;
            m_signal_pick_color.emit(-1); // Stop pick color in Editarea
    	    m_refGdkWindow->set_cursor();
        }else{
            int mx = event->x;
            int my = event->y;
            m_pick_id = Coordinates2Index(mx,my);
            if (m_pick_id>0){
                if (m_Menu_Popup){
                    delete m_Menu_Popup;
                }
                m_Menu_Popup = new Gtk::Menu();
                m_Menu_Popup->append(*m_pick_color_item);
                m_Menu_Popup->show_all();
                m_Menu_Popup->popup_at_pointer((GdkEvent*) event);
            }
        }

    }
    m_refGdkWindow->invalidate(true);

    return true;
}

bool palette::on_motion_notify_event(GdkEventMotion *event)
{
    if (m_f_pick_color_mode){
    	m_refGdkWindow->set_cursor(m_refPickColorCursor);

    }else{
    	m_refGdkWindow->set_cursor();
    
    }
    return true;

}
palette::type_signal_color_select palette::signal_color_select()
{
  return m_signal_color_select;
}

void palette::on_set_foreground_color(gint id,guint32 color)
{
    if (id>0){
        m_tblColors[id] = color;
    }else if (id==-1){
        m_foreGroundColor = color;
    }
    m_f_pick_color_mode = false;
    m_refGdkWindow->invalidate(true);

}

void palette::on_set_background_color(guint32 color)
{
    m_backGroundColor = color;
    m_f_pick_color_mode = false;
    m_refGdkWindow->invalidate(true);

}

void palette::Save(std::string pathName, std::string fileName)
{
    std::string fullPathName;
    //-------------------------------------------------
    if (pathName!=""){
        fullPathName = pathName + "/" + fileName;
    }else{
        fullPathName = fileName;
    }
    std::ofstream   f(fullPathName);
    if (f.is_open()){
        //--
        f << "FOREGROUND " << m_foreGroundColor << std::endl;
        //--
        f << "BACKGROUND " << m_backGroundColor << std::endl;
        //--
        for(int i=0;i<m_nbRows*m_nbColumns;i++){
            f << m_tblColors[i] << std::endl;
        }
        f.close();
    }

}

bool palette::Load(std::string pathName, std::string fileName)
{
    guint32     color;
    std::string strline;
    std::string strWord;
    std::string fullPathName;
    //-------------------------------------------------
    if (pathName!=""){
        fullPathName = pathName + "/" + fileName;
    }else{
        fullPathName = fileName;
    }
    std::ifstream   f(fullPathName);
    if (f.is_open()){
        int  i = 0;
        //--
        while(!f.eof()){
            std::getline(f,strline);
            std::stringstream ss(strline);
            ss >> strWord;
            if (strWord=="FOREGROUND"){
                ss >> strWord;
                m_foreGroundColor = std::atoi(strWord.c_str());
            }else if (strWord=="BACKGROUND"){
                m_backGroundColor = std::atoi(strWord.c_str());
            }else{
                color = std::atoi(strWord.c_str());
                if (i<m_nbRows*m_nbColumns){
                    m_tblColors[i++] = color;
                }else{
                    break;
                }
            }
        }
        f.close();
        return true;
    }
    return false;
}

void palette::on_menu_popup_pick_color()
{
    //-------------------------------------------------
    std::cout << "Pick color" << std::endl;
    m_f_pick_color_mode = true;
	m_refGdkWindow->set_cursor(m_refPickColorCursor);
	m_signal_pick_color.emit(m_pick_id);

}

palette::type_signal_pick_color palette::signal_pick_color()
{
    //------------------------------------------------
    return m_signal_pick_color;
    
}

#include "editArea.h"
#include <iostream>
#include <gtkmm/application.h>
#include "palette.h"
#include <cairomm/context.h>
#include <giomm/resource.h>
#include <gdkmm/general.h>
#include <glibmm/fileutils.h>


editArea::editArea():Glib::ObjectBase("myEditArea"),Gtk::Widget(),m_edit_mode(NULL),m_f_pick_color_mode(false)
{
    //ctor
     set_has_window(true);

//   //Install a style so that an aspect of this widget may be themed via a CSS
//   //style sheet file:
//   gtk_widget_class_install_style_property(GTK_WIDGET_CLASS(
//         G_OBJECT_GET_CLASS(gobj())),
//         g_param_spec_int("editArea",
//         "GtkSpriteEdit",
//         "The scale to use when drawing. This is just a silly example.",
//         G_MININT,
//         G_MAXINT,
//         500,
//         G_PARAM_READABLE) );

//         m_refStyleProvider = Gtk::CssProvider::create();
//         Glib::RefPtr<Gtk::StyleContext> refStyleContext = get_style_context();
//         refStyleContext->add_provider(m_refStyleProvider, GTK_STYLE_PROVIDER_PRIORITY_USER);

//         //Glib::ustring data = "myEditArea {color: #ff00ea;font: Comic Sans MS 12;}";
//         Glib::ustring data = "myEditArea {color: #ff0000;}";

//         try
//         {
// //            m_refStyleProvider->load_from_path("custom_gtk.css");
//             m_refStyleProvider->load_from_data(data);
//         }
//         catch(const Glib::Error& ex)
//         {
//             std::cerr << "Gtk::CssProvider::load_from_data() failed: " << ex.what() << std::endl;
//         }


    m_edit_mode = &m_edit_mode_pencil;
    // m_edit_mode->m_sprite = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, TRUE, 8, 32, 32);
    // m_edit_mode->m_sprite->fill(0xffffff00); // RGBA
    // m_edit_mode->m_sprite_bak = m_edit_mode->m_sprite->copy();

}

editArea::~editArea()
{
    //dtor
}

//Discover the total amount of minimum space and natural space needed by
//this widget.
//Let's make this simple example widget always need minimum 60 by 50 and
//natural 100 by 70.
void editArea::get_preferred_width_vfunc(int &minimum_width, int &natural_width) const
{
    minimum_width = 256;
    natural_width = 256;
}

void editArea::get_preferred_height_for_width_vfunc(int /* width */,
                                                    int &minimum_height, int &natural_height) const
{
    minimum_height = 128;
    natural_height = 128;
}

void editArea::get_preferred_height_vfunc(int &minimum_height, int &natural_height) const
{
    minimum_height = 128;
    natural_height = 256;
}

void editArea::get_preferred_width_for_height_vfunc(int /* height */,
                                                    int &minimum_width, int &natural_width) const
{
    minimum_width = 128;
    natural_width = 256;
}

void editArea::on_size_allocate(Gtk::Allocation &allocation)
{
    //Do something with the space that we have actually been given:
    //(We will not be given heights or widths less than we have requested, though
    //we might get more)

    //Use the offered allocation for this container:
    set_allocation(allocation);

    if (m_refGdkWindow)
    {
        m_refGdkWindow->move_resize(allocation.get_x(), allocation.get_y(),
                                    allocation.get_width(), allocation.get_height());
    }
}

void editArea::on_map()
{
    // Call base class:
    Gtk::Widget::on_map();
}

void editArea::on_unmap()
{
    // Call base class:
    Gtk::Widget::on_unmap();
}

void editArea::on_realize()
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

        attributes.event_mask = get_events() | Gdk::EXPOSURE_MASK | Gdk::BUTTON1_MASK | Gdk::BUTTON2_MASK | Gdk::BUTTON3_MASK
                                            | Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_RELEASE_MASK
                                            | Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK
                                            | Gdk::POINTER_MOTION_HINT_MASK | Gdk::SCROLL_MASK;
        attributes.window_type = GDK_WINDOW_CHILD;
        attributes.wclass = GDK_INPUT_OUTPUT;
        
        m_refGdkWindow = Gdk::Window::create(get_parent_window(),&attributes,GDK_WA_X|GDK_WA_Y);

        editMode::m_refGdkWindow = m_refGdkWindow;

        this->set_can_focus(true);
        this->grab_focus();

        set_window(m_refGdkWindow);

        //--Set colors
        //modify_bg(Gtk::STATE_NORMAL, Gdk::Color("white"));
        //modify_fg(Gtk::STATE_NORMAL, Gdk::Color("blue"));

        // make the widget recieve exposure events
        m_refGdkWindow->set_user_data(gobj());

        //--
        m_refPickColorCursor =  Gdk::Cursor::create(m_refGdkWindow->get_display(),
                                    Gdk::Pixbuf::create_from_resource("/res/PickColor.png"), 4, 28);
        // m_edit_mode_select.m_refMoveCursor = Gdk::Cursor::create(m_refGdkWindow->get_display(),"move");
        // m_edit_mode_select.m_refNWCursor = Gdk::Cursor::create(m_refGdkWindow->get_display(),"nw-resize");
        // m_edit_mode_select.m_refNECursor = Gdk::Cursor::create(m_refGdkWindow->get_display(),"ne-resize");
        // m_edit_mode_select.m_refSWCursor = Gdk::Cursor::create(m_refGdkWindow->get_display(),"sw-resize");
        // m_edit_mode_select.m_refSECursor = Gdk::Cursor::create(m_refGdkWindow->get_display(),"se-resize");

    }

}

void editArea::on_unrealize()
{
    //--
    m_refGdkWindow.clear();

    //Call base class:
    Gtk::Widget::on_unrealize();
}

void editArea::get_pixel(Glib::RefPtr<Gdk::Pixbuf> pixbuf, int x, int y, guchar *red, guchar *green, guchar *blue, guchar *alpha)
{
    int width, height, rowstride, n_channels;
    guchar *pixels, *p;
    //-----------------------------------------------------------------------
    *red = 0x00;
    *green = 0x00;
    *blue = 0x00;
    *alpha = 0x00;
    if (!pixbuf)
        return;

    n_channels = pixbuf->get_n_channels(); // gdk_pixbuf_get_n_channels (pixbuf);

    g_assert(pixbuf->get_colorspace() == Gdk::COLORSPACE_RGB); // gdk_pixbuf_get_colorspace (pixbuf)
    g_assert(pixbuf->get_bits_per_sample() == 8);              //  gdk_pixbuf_get_bits_per_sample (pixbuf)
    g_assert(pixbuf->get_has_alpha());                         //gdk_pixbuf_get_has_alpha (pixbuf)
    g_assert(n_channels == 4);

    width = pixbuf->get_width();   // gdk_pixbuf_get_width (pixbuf);
    height = pixbuf->get_height(); // gdk_pixbuf_get_height (pixbuf);

    g_assert(x >= 0 && x < width);
    g_assert(y >= 0 && y < height);

    rowstride = pixbuf->get_rowstride(); // gdk_pixbuf_get_rowstride (pixbuf);
    pixels = pixbuf->get_pixels();       // gdk_pixbuf_get_pixels (pixbuf);

    p = pixels + y * rowstride + x * n_channels;
    *red = p[0];
    *green = p[1];
    *blue = p[2];
    *alpha = p[3];

}

void editArea::put_pixel(Glib::RefPtr<Gdk::Pixbuf> pixbuf, int x, int y, guchar red, guchar green, guchar blue, guchar alpha)
{
    int width, height, rowstride, n_channels;
    guchar *pixels, *p;
    //-----------------------------------------------------------------------
    if (!pixbuf)
        return;

    n_channels = pixbuf->get_n_channels(); // gdk_pixbuf_get_n_channels (pixbuf);

    g_assert(pixbuf->get_colorspace() == Gdk::COLORSPACE_RGB); // gdk_pixbuf_get_colorspace (pixbuf)
    g_assert(pixbuf->get_bits_per_sample() == 8);              //  gdk_pixbuf_get_bits_per_sample (pixbuf)
    g_assert(pixbuf->get_has_alpha());                         //gdk_pixbuf_get_has_alpha (pixbuf)
    g_assert(n_channels == 4);

    width = pixbuf->get_width();   // gdk_pixbuf_get_width (pixbuf);
    height = pixbuf->get_height(); // gdk_pixbuf_get_height (pixbuf);

    g_assert(x >= 0 && x < width);
    g_assert(y >= 0 && y < height);

    rowstride = pixbuf->get_rowstride(); // gdk_pixbuf_get_rowstride (pixbuf);
    pixels = pixbuf->get_pixels();       // gdk_pixbuf_get_pixels (pixbuf);

    p = pixels + y * rowstride + x * n_channels;
    p[0] = red;
    p[1] = green;
    p[2] = blue;
    p[3] = alpha;

}

bool editArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    //--------------------------------------------------
    int w = get_allocation().get_width();
    int h = get_allocation().get_height();

    //-- Calculer la taille du pixel affiché
    int o1,o2;
    o1 = (w-4) / m_edit_mode->m_nbHPixels;
    o2 = (h-4) / m_edit_mode->m_nbVPixels;
    if (o1<o2){
        m_edit_mode->m_offset = o1 * m_edit_mode->m_scale;
    }else{
        m_edit_mode->m_offset = o2 * m_edit_mode->m_scale;
    }


    // paint the background
    //Gdk::Cairo::set_source_rgba(cr, get_style_context()->get_background_color());
    cr->set_source_rgba(200,200,200,255);
    cr->paint();

  // draw the foreground
  //Gdk::Cairo::set_source_rgba(cr, get_style_context()->get_color());
    int       x,y,dx;
    guchar   red,green,blue,alpha;

    cr->translate( m_edit_mode->m_origin_x, m_edit_mode->m_origin_y);
    //cr->scale(m_edit_mode->m_scale, m_edit_mode->m_scale);

    //-- DrawGrid
    dx = m_edit_mode->m_offset - 1;
    cr->set_source_rgba(0.5, 0.5, 0.5, 1.0);
    for(int row=0;row<=m_edit_mode->m_nbVPixels;row++){
        y = row * m_edit_mode->m_offset+2;
        for(int col=0;col<=m_edit_mode->m_nbHPixels;col++){
            x = col * m_edit_mode->m_offset + 2;
            cr->move_to(x-0.5,y);
            cr->line_to(x+0.5,y);
            //cr->move_to(x,y-0.5);
            //cr->line_to(x,y+0.5);
        }
    }
    cr->stroke();

    //-- Draw Frame
    cr->set_source_rgba(0.1, 0.1, 0.1, 1.0);
    cr->set_line_width(0.2);
    int left = 1;
    int top = 1;
    int right = m_edit_mode->m_nbHPixels * m_edit_mode->m_offset + 2;
    int bottom = m_edit_mode->m_nbVPixels * m_edit_mode->m_offset + 2;
    cr->move_to(left,top);
    cr->line_to(right,top);
    cr->line_to(right,bottom);
    cr->line_to(left,bottom);
    cr->line_to(left,top);
    cr->stroke();


    //-- Draw Pixels
    for(int row=0;row<m_edit_mode->m_nbVPixels;row++){
        y = row * m_edit_mode->m_offset+2;
        for(int col=0;col<m_edit_mode->m_nbHPixels;col++){

            x = col * m_edit_mode->m_offset + 2;

            get_pixel (m_edit_mode->m_sprite, col, row, &red, &green, &blue, &alpha);
            cr->set_source_rgba(red/255.0, green/255.0, blue/255.0,alpha/255.0);
            cr->rectangle (x,y,dx,dx);
            cr->fill();
            /*
            if (!alpha){
                x1 = x;
                y1 = y;
                x2 = x1 + dx;
                y2 = y1 + dy;
                xm = (x1+x2) >> 1;
                ym = (y1+y2) >> 1;
                cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);
                cairo_set_line_width(cr, 2);
                cairo_move_to(cr, xm, ym - 2);
                cairo_line_to(cr, xm, ym + 2);
                cairo_move_to(cr, xm + 2, ym);
                cairo_line_to(cr, xm - 2, ym);
                cairo_stroke(cr);

            }
            */
         }
    }

    //-- 
    m_edit_mode->on_draw(cr);

    //-- Draw sprite
    if (m_edit_mode->m_select_pixbuf){
        Gdk::Cairo::set_source_pixbuf(cr,m_edit_mode->m_select_pixbuf, 32*m_edit_mode->m_offset+10, 0);
        cr->paint();
    }
    //-- Draw sprite backup
    // Gdk::Cairo::set_source_pixbuf(cr,m_edit_mode->m_sprite_bak, 32*m_edit_mode->m_offset+10, 64);
    // cr->paint();


/*
    cr->set_source_rgba(0.337, 0.612, 0.117, 0.9);   // green

    int     x,y,dx;
    dx = m_offset - 1;
    for(int i=0;i<m_nbVPixels;i++){
        y = i*m_offset+2;
        for(int j=0;j<m_nbHPixels;j++){
            x = j*m_offset+2;
            if (j % 2){
                cr->set_source_rgba(0.337, 0.0, 0.117, 0.9);   // green
            }else{
                cr->set_source_rgba(0.337, 0.612, 0.117, 0.9);   // green
            }
            cr->rectangle ( x, y, dx, dx);
            cr->fill();
        }
    }
*/

  return true;
}

// bool editArea::MouseToPixel(int mx,int my,int &pixelX,int &pixelY)
// {
//     //---------------------------------------------------------
//     pixelX = (mx-2) / m_edit_mode->m_offset;
//     pixelY = (my-2) / m_edit_mode->m_offset;
//     return (pixelX>=0) && (pixelX<m_edit_mode->m_nbHPixels) && (pixelY>=0) && (pixelY<m_edit_mode->m_nbVPixels);

// }

bool editArea::on_key_press_event (GdkEventKey* key_event)
{
    //----------------------------------------------------------
    if (key_event->state & GDK_SHIFT_MASK){
	    m_refGdkWindow->set_cursor(m_refPickColorCursor);
    }

    return true;

}

bool editArea::on_key_release_event (GdkEventKey* key_event)
{
    //----------------------------------------------------------
    m_refGdkWindow->set_cursor();

    return true;

}

bool editArea::on_button_press_event(GdkEventButton *event)
{
    int     pixelX,pixelY;
    guchar  red,green,blue,alpha;
    //Gdk::ModifierType   state= Gdk::ModifierType(0);
    //----------------------------------------------------------
    //m_refGdkWindow->get_pointer(x,y,state);
    if (m_f_pick_color_mode||(event->state & GDK_SHIFT_MASK)){
        if (m_edit_mode->MouseToPixel(event->x,event->y,pixelX,pixelY)){
            m_edit_mode->get_pixel(m_edit_mode->m_sprite,pixelX,pixelY,&red,&green,&blue,&alpha);
            if (m_f_pick_color_mode){
                m_edit_mode->m_foreGroundColor = palette::RGBA(red, green, blue, alpha);
                m_signal_forecolor_pick.emit( m_id_palette_color, m_edit_mode->m_foreGroundColor);
                m_f_pick_color_mode = false;
                m_refGdkWindow->set_cursor();
            }else{
                if (event->button==1){
                    m_edit_mode->m_foreGroundColor = palette::RGBA(red, green, blue, alpha);
                    m_signal_forecolor_pick.emit(-1,m_edit_mode->m_foreGroundColor);
                }else if (event->button==3){
                    m_edit_mode->m_backGroundColor = palette::RGBA(red, green, blue, alpha);
                    m_signal_backcolor_pick.emit(m_edit_mode->m_backGroundColor);
                }
            }
        }else{
            m_f_pick_color_mode = false;
            m_signal_forecolor_pick.emit(-2, 0); // Faire sortir Palette du mode Pick Color
            m_refGdkWindow->set_cursor();
        }
        m_refGdkWindow->invalidate(true);
    }else{

        if (event->button==2){
            //std::cout << "editModePencil:on_button_press_event (x,y) = (" <<  event->x << "," << event->y << ")"<< std::endl;
            m_edit_mode->m_start_x = event->x;
            m_edit_mode->m_start_y = event->y;
            m_edit_mode->m_start_origin_x = m_edit_mode->m_origin_x;
            m_edit_mode->m_start_origin_y = m_edit_mode->m_origin_y;

        }else{
            if (m_edit_mode->on_button_press_event(event)){
                m_refGdkWindow->invalidate(true);

            }
        }
    }
    return true;
}

bool editArea::on_button_release_event(GdkEventButton *event)
{
   //---------------------------------------------------------
    m_signal_sprite_modified.emit();
    if (m_edit_mode->on_button_release_event(event)){
        m_refGdkWindow->invalidate(true);
    }
    return true;
}

bool editArea::on_motion_notify_event(GdkEventMotion *event)
{
    //-------------------------------------------------------
    if (m_f_pick_color_mode||(event->state & GDK_SHIFT_MASK)){
	    m_refGdkWindow->set_cursor(m_refPickColorCursor);
    }else{
        m_refGdkWindow->set_cursor();
        if (event->state & Gdk::BUTTON2_MASK){
            int dx = event->x - m_edit_mode->m_start_x;
            int dy = event->y - m_edit_mode->m_start_y;
            m_edit_mode->m_origin_x = m_edit_mode->m_start_origin_x + dx;
            m_edit_mode->m_origin_y = m_edit_mode->m_start_origin_y + dy;
            m_refGdkWindow->invalidate(true);
            
        }else{

            if (m_edit_mode->on_motion_notify_event(event)){
                m_signal_sprite_modified.emit();
                m_refGdkWindow->invalidate(true);
            }

        }

    }
    return true;
}

bool editArea::on_scroll_event(GdkEventScroll *event)
{
    //-------------------------------------------------------
    if (event->direction==GDK_SCROLL_UP){
        if (m_edit_mode->m_scale<5.0f){
            m_edit_mode->m_scale += 0.05f;

        }
    }else if (event->direction==GDK_SCROLL_DOWN){
        if (m_edit_mode->m_scale>0.5f){
            m_edit_mode->m_scale -= 0.05f;
        }
    }

    m_refGdkWindow->invalidate(true);

    std::cout << "editModePencil:on_mouse_scroll" << m_edit_mode->m_offset << std::endl;
    return true;
}

void editArea::on_palette_color_select(bool f, guint32 selColor)
{
    //-------------------------------------------------------
    if (f){
        m_edit_mode->m_foreGroundColor = selColor;
    }else{
        m_edit_mode->m_backGroundColor = selColor;
    }

}

void editArea::SetSprite(Glib::RefPtr<Gdk::Pixbuf> sprite)
{
    //-----------------------------------------------------
    if (sprite){
        if (!sprite->get_has_alpha()){
            m_edit_mode->m_sprite = sprite->add_alpha(true,0,0,0);
        }else{
            m_edit_mode->m_sprite = sprite;
        }
        m_edit_mode->m_sprite_bak = m_edit_mode->m_sprite->copy();
        m_edit_mode->m_nbHPixels = sprite->get_width();
        m_edit_mode->m_nbVPixels = sprite->get_height();
        if (m_refGdkWindow) m_refGdkWindow->invalidate(true);

    }
    
}

Glib::RefPtr<Gdk::Pixbuf> editArea::GetSprite()
{
    //-----------------------------------------------------
    return m_edit_mode->m_sprite;

}

void editArea::SetForegroundColor(guint32 newColor)
{
    m_edit_mode->m_foreGroundColor = newColor;

}

void editArea::SetBackgroundColor(guint32 newColor)
{
    m_edit_mode->m_backGroundColor = newColor;

}

void editArea::SetEditMode(int imode)
{
    //-----------------------------------------------------
    switch(imode){
    case EM_SELECT:
        m_edit_mode = &m_edit_mode_select;
        break;
    case EM_PENCIL:
        m_edit_mode = &m_edit_mode_pencil;
        break;    
    case EM_RECTANGLE:
        m_edit_mode = &m_edit_mode_rect;
        break;
    case EM_ELLIPSE:
        m_edit_mode = &m_edit_mode_ellipse;
        break;
    case EM_FILL:
        m_edit_mode = &m_edit_mode_fill;
        break;
    }
    m_mode = imode;
    m_edit_mode->init_mode();
    if (m_refGdkWindow) m_refGdkWindow->invalidate(true);

}

void editArea::CopySelect()
{
    //------------------------------------------------
    if (m_mode == EM_SELECT)
    {
        if (!m_edit_mode->m_rect_select_pix.IsNULL())
        {
            if (!m_edit_mode->m_select_pixbuf)
            {
                m_edit_mode->m_select_pixbuf = m_edit_mode->m_sprite->copy();
            }
            m_edit_mode->FillPixBuf(m_edit_mode->m_select_pixbuf,m_edit_mode->m_rect_select_pix,m_edit_mode->m_backGroundColor);
            m_edit_mode->BlitPixBuf(m_edit_mode->m_select_pixbuf, 0, 0, m_edit_mode->m_sprite, m_edit_mode->m_rect_select_pix);
            m_edit_mode->BackupSprite();
            m_edit_mode->m_rect_copy_pix = m_edit_mode->m_rect_select_pix;
            m_edit_mode->m_copy_pixbuf = m_edit_mode->m_select_pixbuf->copy();
            m_edit_mode->init_mode();
            m_refGdkWindow->invalidate(true);

            int w = m_edit_mode->m_rect_copy_pix.right - m_edit_mode->m_rect_copy_pix.left;
            int h = m_edit_mode->m_rect_copy_pix.bottom - m_edit_mode->m_rect_copy_pix.top;

            auto tmpImg = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, TRUE, 8, w, h);
            if (tmpImg){
                m_edit_mode->BlitPixBuf(tmpImg, 0, 0, m_edit_mode->m_sprite, m_edit_mode->m_rect_copy_pix);
                Glib::RefPtr<Gtk::Clipboard> clip = Gtk::Clipboard::get(GDK_SELECTION_CLIPBOARD);
                clip->set_image(tmpImg);

            }


        }
    }
}

void editArea::on_image_received(const Glib::RefPtr<Gdk::Pixbuf>& pixbuf)
{
    //------------------------------------------------

    m_edit_mode->BackupSprite();

    int w,h;
    int wi = pixbuf->get_width();
    int hi = pixbuf->get_height();
    if (wi<=m_edit_mode->m_nbHPixels){
        w = wi;
    }else{
        w = m_edit_mode->m_nbHPixels;
    }
    if (hi<=m_edit_mode->m_nbVPixels){
        h = hi;
    }else{
        h = m_edit_mode->m_nbVPixels;
    }
    m_edit_mode->m_rect_copy_pix.left = 0;
    m_edit_mode->m_rect_copy_pix.top = 0;
    m_edit_mode->m_rect_copy_pix.right = w;
    m_edit_mode->m_rect_copy_pix.bottom = h;
    m_edit_mode->m_copy_pixbuf = m_edit_mode->m_sprite->copy();
    m_edit_mode->BlitPixBuf(m_edit_mode->m_copy_pixbuf, 0, 0, pixbuf, m_edit_mode->m_rect_copy_pix);

    m_edit_mode->m_rect_select_pix = m_edit_mode->m_rect_copy_pix;
    m_edit_mode->m_select_pixbuf = m_edit_mode->m_copy_pixbuf->copy();
    
    //--
    m_edit_mode->BlitPixBuf(m_edit_mode->m_sprite,
                m_edit_mode->m_rect_select_pix.left,
                m_edit_mode->m_rect_select_pix.top,
                m_edit_mode->m_select_pixbuf,
                RRect(0,0,m_edit_mode->m_rect_select_pix.Width(),m_edit_mode->m_rect_select_pix.Height()));
    m_edit_mode->m_select_state = 3;

}

void editArea::PasteCopy()
{
    //------------------------------------------------
    Glib::RefPtr<Gtk::Clipboard> clip = Gtk::Clipboard::get(GDK_SELECTION_CLIPBOARD);
    if (clip->wait_is_image_available()){
        SetEditMode(editArea::EM_SELECT);
        clip->request_image(sigc::mem_fun(*this,&editArea::on_image_received));
        //--
        m_signal_sprite_modified.emit();
        m_refGdkWindow->invalidate(true);

    }

}

void editArea::CutSelect()
{

   //------------------------------------------------
   if (m_mode==EM_SELECT){
        if (!m_edit_mode->m_rect_select_pix.IsNULL()){

            if (!m_edit_mode->m_select_pixbuf)
            {
                m_edit_mode->m_select_pixbuf = m_edit_mode->m_sprite->copy();
            }
            m_edit_mode->FillPixBuf(m_edit_mode->m_select_pixbuf,m_edit_mode->m_rect_select_pix,m_edit_mode->m_backGroundColor);
            m_edit_mode->BlitPixBuf(m_edit_mode->m_select_pixbuf, 0, 0, m_edit_mode->m_sprite, m_edit_mode->m_rect_select_pix);
            m_edit_mode->BackupSprite();
            m_edit_mode->m_rect_copy_pix = m_edit_mode->m_rect_select_pix;
            m_edit_mode->m_copy_pixbuf = m_edit_mode->m_select_pixbuf->copy();
            m_refGdkWindow->invalidate(true);

            int w = m_edit_mode->m_rect_copy_pix.right - m_edit_mode->m_rect_copy_pix.left;
            int h = m_edit_mode->m_rect_copy_pix.bottom - m_edit_mode->m_rect_copy_pix.top;

            auto tmpImg = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, TRUE, 8, w, h);
            if (tmpImg){
                m_edit_mode->BlitPixBuf(tmpImg, 0, 0, m_edit_mode->m_sprite, m_edit_mode->m_rect_copy_pix);
                Glib::RefPtr<Gtk::Clipboard> clip = Gtk::Clipboard::get(GDK_SELECTION_CLIPBOARD);
                clip->set_image(tmpImg);

            }

            //--
            m_edit_mode->FillPixBuf(m_edit_mode->m_sprite,m_edit_mode->m_rect_select_pix,m_edit_mode->m_backGroundColor);
            m_edit_mode->init_mode();
            m_refGdkWindow->invalidate(true);
        }
   }
}

void editArea::Undo()
{
    //------------------------------------------------
    m_edit_mode->init_mode();
    m_edit_mode->UndoSprite();
    m_refGdkWindow->invalidate(true);

}

void editArea::Redo()
{
    //------------------------------------------------
    m_edit_mode->init_mode();
    m_edit_mode->UndoSprite();
    m_refGdkWindow->invalidate(true);

}

editArea::type_signal_forecolor_pick editArea::signal_forecolor_pick()
{
    //------------------------------------------------
    return m_signal_forecolor_pick;
    
}

editArea::type_signal_backcolor_pick editArea::signal_backcolor_pick()
{
    //------------------------------------------------
    return m_signal_backcolor_pick;

}

editArea::type_signal_sprite_modified editArea::signal_sprite_modified()
{
    //------------------------------------------------
    return m_signal_sprite_modified;

}

void editArea::on_pick_color_mode(int id_color)
{
    //------------------------------------------------
    if (id_color<0){
        m_id_palette_color = -1;
        m_f_pick_color_mode = false;
    }else{
        m_id_palette_color = id_color;
        m_f_pick_color_mode = true;
    }
}

void editArea::flip_horizontaly()
{
    //----------------------------------------
 
    //--
    m_edit_mode->init_mode();

    m_edit_mode->BackupSprite();
    //priv->undo_mode = UNDO_FLIP_HORIZONTALY;

    m_edit_mode->flip_horizontaly(m_edit_mode->m_sprite_bak,m_edit_mode->m_sprite);

    //--
    m_refGdkWindow->invalidate(true);
    m_signal_sprite_modified.emit();


}

void editArea::flip_verticaly()
{
    //----------------------------------------

    //--
    m_edit_mode->init_mode();

    m_edit_mode->BackupSprite();
    //priv->undo_mode = UNDO_FLIP_VERTICALY;

    m_edit_mode->flip_verticaly(m_edit_mode->m_sprite_bak,m_edit_mode->m_sprite);

    //--
    m_refGdkWindow->invalidate(true);
    m_signal_sprite_modified.emit();


}

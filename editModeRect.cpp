#include "editArea.h"
#include "editModeRect.h"
#include <iostream>

editModeRect::editModeRect():editMode(),
        m_start_pt(NULL),m_end_pt(NULL),
        m_select_handle(NULL)
{
    //ctor
    for (int i=0;i<4;i++){
        m_handles[i] = new RHandle(0,0,0,0);
    }

    m_handles[RHandle::TOP_LEFT]->m_x = &m_x1;
    m_handles[RHandle::TOP_LEFT]->m_y = &m_y1;

    m_handles[RHandle::BOTTOM_LEFT]->m_x = &m_x1;
    m_handles[RHandle::BOTTOM_LEFT]->m_y = &m_y2;

    m_handles[RHandle::TOP_RIGHT]->m_x = &m_x2;
    m_handles[RHandle::TOP_RIGHT]->m_y = &m_y1;

    m_handles[RHandle::BOTTOM_RIGHT]->m_x = &m_x2;
    m_handles[RHandle::BOTTOM_RIGHT]->m_y = &m_y2;

    m_handle_color = palette::RGBA(0, 0, 156, 255);

}

editModeRect::~editModeRect()
{
    //dtor
    for (int i=0;i<4;i++){
        if (m_handles[i]) delete (m_handles[i]);
    }

}

bool editModeRect::on_button_press_event(Gtk::Widget *w, GdkEventButton *event)
{
    int         pixelX,pixelY;
    //----------------------------------------------------

    int tmx = event->x - m_origin_x;
    int tmy = event->y - m_origin_y;

    if (event->button==1)
    {

        if (m_select_handle = HitHandle(tmx, tmy))
        {
            std::cout << "editModeRect:Hit Handle" << std::endl;
            return false;
        }
        else
        {
            if (MouseToPixel(tmx, tmy, pixelX, pixelY))
            {
                if (m_rect_current_pix.PtInRect(pixelX, pixelY))
                {
                    m_select_move_flag = true;
                    m_rect_current_pix_sav = m_rect_current_pix;
                    m_start_pix_x = pixelY;
                    m_start_pix_y = pixelY;
                    return false;
                }

                if (m_rect_current_pix.IsNULL()){
                    if (!m_start_pt){
                        m_start_pt = new Gdk::Point(pixelX, pixelY);
                        //-- Sauvegarder l'image d'origine
                        SaveStartState();
                        static_cast<editArea*>(w)->signal_save_image_state().emit();
                    }
                }else{
                    InitHandles();
                    m_select_move_flag = false;
                    m_rect_current_pix.SetNULL();
                    return true;
                }

            }

        }

     }
    return true;
}

bool editModeRect::on_button_release_event(Gtk::Widget *w, GdkEventButton *event)
{
    int         pixelX,pixelY;
   //---------------------------------------------------------
    std::cout << "editModePencil:on_button_release_event" << std::endl;
    if (m_start_pt){
        delete m_start_pt;
        m_start_pt = NULL;
    }
    if (m_end_pt){
        delete m_end_pt;
        m_end_pt = NULL;
    }

    //-- Normalize le rectangle de sélection
    if (!m_rect_current_pix.IsNULL()){
        if (m_x1>m_x2){
            int dum = m_x1;
            m_x1 = m_x2;
            m_x2 = dum;
        }
        if (m_y1>m_y2){
            int dum = m_y1;
            m_y1 = m_y2;
            m_y2 = dum;
        }
        int startX, startY;
        int endX, endY;
        MouseToPixel(m_x1, m_y1, startX, startY);
        MouseToPixel(m_x2, m_y2, endX, endY);
        m_rect_current_pix.left = startX;
        m_rect_current_pix.top = startY;
        m_rect_current_pix.right = endX+1;
        m_rect_current_pix.bottom = endY+1;

    }

    m_select_handle = NULL;

    return false;
}

void editModeRect::DrawRectangle(Glib::RefPtr<Gdk::Pixbuf> pixbuf, int startX,int startY, int endX, int endY, guint32 col)
{
	int dum;
    //---------------------------------------------------------
    if (endX<startX){
        dum = startX;
        startX = endX;
        endX = dum;
    }
    if (endY<startY){
        dum = startY;
        startY = endY;
        endY = dum;
    }
    if ((endX!=startX)||(endY!=startY)){
        //-- Tracer le rectangle
        line(pixbuf, startX, startY,endX,startY,col);
        line(pixbuf, endX, startY,endX,endY,col);
        line(pixbuf, endX, endY,startX,endY,col);
        line(pixbuf, startX, endY,startX,startY,col);
    }

}

void editModeRect::FillRectangle(Glib::RefPtr<Gdk::Pixbuf> pixbuf, int startX,int startY, int endX, int endY, guint32 col)
{
	int     dum;
    int     width, height, rowstride, n_channels;
    guchar *pixels, *p;
    int     yOffset;
    guchar  red,green,blue,alpha;
    //-----------------------------------------------------------------------
    if (!pixbuf)
        return;

    if (endX<startX){
        dum = startX;
        startX = endX;
        endX = dum;
    }
    if (endY<startY){
        dum = startY;
        startY = endY;
        endY = dum;
    }

    if (startX<0){
        startX = 0;
    }

    if ((endX!=startX)||(endY!=startY)){

        n_channels = pixbuf->get_n_channels(); // gdk_pixbuf_get_n_channels (pixbuf);

        g_assert(pixbuf->get_colorspace() == Gdk::COLORSPACE_RGB); // gdk_pixbuf_get_colorspace (pixbuf)
        g_assert(pixbuf->get_bits_per_sample() == 8);              //  gdk_pixbuf_get_bits_per_sample (pixbuf)
        g_assert(pixbuf->get_has_alpha());                         //gdk_pixbuf_get_has_alpha (pixbuf)
        g_assert(n_channels == 4);

        width = pixbuf->get_width();   // gdk_pixbuf_get_width (pixbuf);
        height = pixbuf->get_height(); // gdk_pixbuf_get_height (pixbuf);

        //g_assert(x >= 0 && x < width);
        //g_assert(y >= 0 && y < height);

        red   = palette::GetRGBRed(col);
        green = palette::GetRGBGreen(col);
        blue  = palette::GetRGBBlue(col);
        alpha = palette::GetRGBAlpha(col);

        rowstride = pixbuf->get_rowstride(); // gdk_pixbuf_get_rowstride (pixbuf);
        pixels = pixbuf->get_pixels();       // gdk_pixbuf_get_pixels (pixbuf);

        for(int y=startY;y<=endY;y++){
            yOffset = y * rowstride;
            for(int x=startX;x<=endX;x++){
                p = pixels + yOffset + x * n_channels;
                p[0] = red;
                p[1] = green;
                p[2] = blue;
                p[3] = alpha;
            }
        }
    }
}

RHandle *editModeRect::HitHandle(int mx,int my)
{
    //---------------------------------------------------------
    // Cairo::Matrix m;
    // m = Cairo::identity_matrix();
    // m.scale(m_scale,m_scale);
    // m.translate(m_origin_x,m_origin_y);
    // m.invert();
    // double dx = mx;
    // double dy = my;
    // m.transform_point(dx,dy);

    for (int i=0;i<4;i++){
        if (m_handles[i]->PtInRect(mx,my)){
            return m_handles[i];
        }
    }
    return NULL;
}

bool editModeRect::RectInEditArea(RRect &rect)
{
    //----------------------------------------------------------
    if ((rect.left<0)||(rect.left>m_nbHPixels)){
        return false;
    }
    if ((rect.right<0)||(rect.right>m_nbHPixels)){
        return false;
    }
    if ((rect.top<0)||(rect.top>m_nbVPixels)){
        return false;
    }
    if ((rect.bottom<0)||(rect.bottom>m_nbVPixels)){
        return false;
    }
    return true;
}

bool editModeRect::on_motion_notify_event(Gtk::Widget *w, GdkEventMotion *event)
{
    guint8 r, g, b, a;
    int pixelX, pixelY;
    bool fButton = false;
    guint32 color;
    //---------------------------------------------------------
    //std::cout << "editModePencil:on_motion_notify_event" << std::endl;

    if (m_select_handle)
    {
        int left,top;
        int tmx = event->x - m_origin_x;
        int tmy = event->y - m_origin_y;
        if (MouseToPixel(tmx, tmy, pixelX, pixelY)){
            PixelToMouse(pixelX, pixelY, left, top);
            *(m_select_handle->m_x) = left;
            *(m_select_handle->m_y) = top;


            int startX, startY;
            int endX, endY;
            MouseToPixel(m_x1, m_y1, startX, startY);
            MouseToPixel(m_x2, m_y2, endX, endY);

            m_rect_current_pix.left = startX;
            m_rect_current_pix.top = startY;
            m_rect_current_pix.right = endX+1;
            m_rect_current_pix.bottom = endY+1;
             
            RestoreStartState();

            if ((startX != endX) || (startY != endY))
            {
                if (m_current_fill_mode)
                {
                    FillRectangle(m_sprite, startX, startY, endX, endY, m_current_color);
                }
                else
                {
                    DrawRectangle(m_sprite, startX, startY, endX, endY, m_current_color);
                }
            }
            return true;
        }
    }
    else if (m_start_pt)
    {
        if (event->state & Gdk::BUTTON1_MASK)
        {
            m_current_color = m_foreGroundColor;
            fButton = true;
        }
        else if (event->state & Gdk::BUTTON3_MASK)
        {
            m_current_color = m_backGroundColor;
            fButton = true;
        }

        if (fButton)
        {
            int tmx = event->x - m_origin_x;
            int tmy = event->y - m_origin_y;

            MouseToPixel(tmx, tmy, pixelX, pixelY);
            int maxH = m_nbHPixels - 1;
            if (pixelX > maxH)
            {
                pixelX = maxH;
            }
            int maxV = m_nbVPixels - 1;
            if (pixelY > maxV)
            {
                pixelY = maxV;
            }

            RestoreStartState();
            
            if (!m_end_pt)
            {
                m_end_pt = new Gdk::Point(pixelX, pixelY);
            }
            else
            {
                m_end_pt->set_x(pixelX);
                m_end_pt->set_y(pixelY);
            }
            int startX = m_start_pt->get_x();
            int startY = m_start_pt->get_y();
            int endX = m_end_pt->get_x();
            int endY = m_end_pt->get_y();

            if (startX>endX){
                int dum = startX;
                startX = endX;
                endX = dum;
            }
            if (startY>endY){
                int dum = startY;
                startY = endY;
                endY = dum;
            }

            //--
            int left, top;
            int right, bottom;
            PixelToMouse(startX, startY, left, top);
            PixelToMouse(endX, endY, right, bottom);

            m_x1 = left;
            m_y1 = top;
            m_x2 = right;
            m_y2 = bottom;

            m_rect_current_pix.left = startX;
            m_rect_current_pix.top = startY;
            m_rect_current_pix.right = endX+1;
            m_rect_current_pix.bottom = endY+1;

            if ((startX != endX) || (startY != endY))
            {

                if (event->state & GDK_CONTROL_MASK)
                {
                    m_current_fill_mode = true;
                    FillRectangle(m_sprite, startX, startY, endX, endY, m_current_color);
                }
                else
                {
                    m_current_fill_mode = false;
                    DrawRectangle(m_sprite, startX, startY, endX, endY, m_current_color);
                }
            }
            return true;
        }
    }
    else if (m_select_move_flag)
    {
        int     x, y;
        int     vx=0,vy=0;

        if (event->state & Gdk::BUTTON1_MASK)
        {
            int tmx = event->x - m_origin_x;
            int tmy = event->y - m_origin_y;
            if (PtInEditArea(tmx, tmy))
            {
                if (MouseToPixel(tmx,tmy,x,y) &&
                        ((x!=m_start_pix_x)||(y!=m_start_pix_y))){     
                    RRect rect;
                    vx = x - m_start_pix_x;
                    vy = y - m_start_pix_y;
                    rect = m_rect_current_pix_sav;
                    rect.Offset(vx,vy);

                    if (!RectInEditArea(rect)){
                        m_rect_current_pix_sav = m_rect_current_pix;
                        m_start_pix_x = x;
                        m_start_pix_y = y;
                    }else{
                        m_rect_current_pix = rect;
                    }

                    int startX, startY;
                    int endX, endY;
                    startX = m_rect_current_pix.left;
                    startY = m_rect_current_pix.top;
                    endX = m_rect_current_pix.right-1;
                    endY = m_rect_current_pix.bottom-1;

                    int left, top;
                    int right, bottom;
                    PixelToMouse(startX, startY, left, top);
                    PixelToMouse(endX, endY, right, bottom);

                    m_x1 = left;
                    m_y1 = top;
                    m_x2 = right;
                    m_y2 = bottom;

                    RestoreStartState();

                    if ((startX != endX) || (startY != endY))
                    {
                        if (m_current_fill_mode)
                        {
                            FillRectangle(m_sprite, startX, startY, endX, endY, m_current_color);
                        }
                        else
                        {
                            DrawRectangle(m_sprite, startX, startY, endX, endY, m_current_color);
                        }

                    }
                    return true;

                }
            }
        }

    }
    return true;

}

bool editModeRect::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    RRect   rect;
    guchar  red,green,blue,alpha;
    //---------------------------------------------------------

    red   = palette::GetRGBRed(m_handle_color);
    green = palette::GetRGBGreen(m_handle_color);
    blue  = palette::GetRGBBlue(m_handle_color);
    alpha  = palette::GetRGBAlpha(m_handle_color);

    //-- Update Handles
    if (!m_rect_current_pix.IsNULL()) {
        PixelToMouse(m_rect_current_pix.left, m_rect_current_pix.top, m_x1, m_y1);
        PixelToMouse(m_rect_current_pix.right - 1, m_rect_current_pix.bottom - 1,
                        m_x2, m_y2);
        m_handles[RHandle::TOP_LEFT]->UpdateRect(m_offset);
        m_handles[RHandle::BOTTOM_LEFT]->UpdateRect(m_offset);
        m_handles[RHandle::TOP_RIGHT]->UpdateRect(m_offset);
        m_handles[RHandle::BOTTOM_RIGHT]->UpdateRect(m_offset);
    }

    for (int i=0;i<4;i++){
        if (!m_handles[i]->IsNULL()){
            cr->set_source_rgba(red/255.0f, green/255.0f, blue/255.0f, alpha/255.0f);
            rect.left = m_handles[i]->m_left;
            rect.top = m_handles[i]->m_top;
            rect.right = m_handles[i]->m_right;
            rect.bottom = m_handles[i]->m_bottom;
            rect.Deflate(2,2,3,3);
            cr->rectangle (rect.left,rect.top,rect.Width(),rect.Height());
            cr->fill();
            cr->set_source_rgba(200/255.0f, 200/255.0f, 200/255.0f, alpha/255.0f);
            rect.Deflate(1,1,1,1);
            cr->rectangle (rect.left,rect.top,rect.Width(),rect.Height());
            cr->fill();            
        }
    }

    //-- Draw Sélection frame
    if (!m_rect_current_pix.IsNULL()) {
        int scrLeft, scrTop, scrRight, scrBottom;
        int left, top, right, bottom;

        //-- Make a Clipping
        if (m_rect_current_pix.left < 0) {
            left = 0;
        } else {
            left = m_rect_current_pix.left;
        }
        if (m_rect_current_pix.top < 0) {
            top = 0;
        } else {
            top = m_rect_current_pix.top;
        }
        if (m_rect_current_pix.right >= m_nbHPixels) {
            right = m_nbHPixels;
        } else {
            right = m_rect_current_pix.right;
        }
        if (m_rect_current_pix.bottom >= m_nbHPixels) {
            bottom = m_nbHPixels;
        } else {
            bottom = m_rect_current_pix.bottom;
        }

        PixelToMouse(left, top, scrLeft, scrTop);
        PixelToMouse(right, bottom, scrRight, scrBottom);
        cr->set_source_rgba(red / 255.0f, green / 255.0f, blue / 255.0f, 0.1f);
        cr->rectangle(scrLeft, scrTop, scrRight - scrLeft, scrBottom - scrTop);
        cr->fill();

    }

    return true;
}

void editModeRect::InitHandles()
{
    //---------------------------------------------------------
    for (int i=0;i<4;i++){
        m_handles[i]->SetNULL();
    }
    m_select_handle = NULL;

}

void editModeRect::init_mode()
{
    //---------------------------------------------------------
    InitHandles();
    m_rect_current_pix.SetNULL();
    m_rect_current_pix_sav.SetNULL();
}


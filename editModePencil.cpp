#include "editModePencil.h"
#include <iostream>
#include <gdkmm/color.h>

editModePencil::editModePencil():editMode(),m_lastPixelX(0),m_lastPixelY(0)
{
    //ctor

}

editModePencil::~editModePencil()
{
    //dtor
    
}

bool editModePencil::on_button_press_event(GdkEventButton *event)
{
    int         x,y;
    guint8      r,g,b,a;
    bool        fButton=false;
    guint32     color= m_foreGroundColor;
    //----------------------------------------------------
    std::cout << "editModePencil:on_button_press_event" << std::endl;

    int tmx = event->x - m_origin_x;
    int tmy = event->y - m_origin_y;

    if (event->button==1){
        color = m_foreGroundColor;
    }else if (event->button==3){
        color = m_backGroundColor;
    }

    if (MouseToPixel(tmx,tmy,m_pixelX,m_pixelY)&&PtInEditArea(tmx, tmy)){
        SaveState();
        BackupSprite();
        if (event->state & GDK_CONTROL_MASK){
            line( m_sprite, m_lastPixelX, m_lastPixelY, m_pixelX, m_pixelY, color);
            return true;
        }else{
            r = palette::GetRGBRed(color);
            g = palette::GetRGBGreen(color);
            b = palette::GetRGBBlue(color);
            a = palette::GetRGBAlpha(color);
            put_pixel (m_sprite , m_pixelX, m_pixelY, r, g, b, a);
            std::cout << "editModePencil:on_button_press_event a=" << std::to_string(a) << std::endl;
            return true;
        }
    }
    
    return false;
}

bool editModePencil::on_button_release_event(GdkEventButton *event)
{
    int         pixelX,pixelY;
   //---------------------------------------------------------
    std::cout << "editModePencil:on_button_release_event" << std::endl;

    int tmx = event->x - m_origin_x;
    int tmy = event->y - m_origin_y;

    if (MouseToPixel( tmx, tmy, pixelX, pixelY)){
        m_lastPixelX = pixelX;
        m_lastPixelY = pixelY;
        return true;
    }
    m_start_x = -1;
    m_start_y = -1;
    
    return false;
}

bool editModePencil::on_motion_notify_event(GdkEventMotion *event)
{
    guint8  r,g,b,a;
    int     pixelX,pixelY;
    bool    fButton=false;
    guint32 color;
    //---------------------------------------------------------
    //std::cout << "editModePencil:on_motion_notify_event" << std::endl;

    if (event->state & Gdk::BUTTON1_MASK){
        color =m_foreGroundColor;
        fButton = true;
    }else if (event->state & Gdk::BUTTON3_MASK){
        color =m_backGroundColor;
        fButton = true;
    }

    int tmx = event->x - m_origin_x;        
    int tmy = event->y - m_origin_y;        
    if ((fButton)&&(MouseToPixel(tmx,tmy,pixelX,pixelY))&&(PtInEditArea(tmx, tmy))){
        if ((pixelX!=m_pixelX)||(pixelY!=m_pixelY)){
            m_pixelX = pixelX;
            m_pixelY = pixelY;
            if (event->state & GDK_CONTROL_MASK){
                RestoreSprite();
                line( m_sprite, m_lastPixelX, m_lastPixelY, pixelX, pixelY, color);
                return true;
            }else{
                r = palette::GetRGBRed(color);
                g = palette::GetRGBGreen(color);
                b = palette::GetRGBBlue(color);
                a = palette::GetRGBAlpha(color);
                put_pixel (m_sprite , pixelX, pixelY, r, g, b, a);
                return true;
            }
        }
    }

    return false;
    
}

bool editModePencil::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    //---------------------------------------------------------

    return false;
}

void editModePencil::init_mode()
{

}

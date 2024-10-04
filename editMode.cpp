#include "editMode.h"
#include <iostream>
#include "palette.h"

int editMode::m_offset = 22;
int editMode::m_nbHPixels = 32;
int editMode::m_nbVPixels = 32;

int editMode::m_origin_x = 0;
int editMode::m_origin_y = 0;
int editMode::m_start_origin_x = 0;
int editMode::m_start_origin_y = 0;
int editMode::m_start_x = -1;
int editMode::m_start_y = -1;
double editMode::m_scale = 1.0f;

guint32 editMode::m_foreGroundColor = 0;
guint32 editMode::m_backGroundColor = 0;

Glib::RefPtr<Gdk::Pixbuf>   editMode::m_sprite(NULL);
Glib::RefPtr<Gdk::Pixbuf>   editMode::m_sprite_bak(NULL);

int                         editMode::m_select_state = 0;
RRect                       editMode::m_rect_select_pix(0,0,0,0);

Glib::RefPtr<Gdk::Pixbuf>   editMode::m_select_pixbuf(NULL);
RRect                       editMode::m_rect_copy_pix(0,0,0,0);
Glib::RefPtr<Gdk::Pixbuf>   editMode::m_copy_pixbuf(NULL);
RRect                       editMode::m_rect_select_pix_sav(0,0,0,0);
bool                        editMode::m_select_move_flag = false;
Glib::RefPtr<Gdk::Window>   editMode::m_refGdkWindow(NULL);

editMode::editMode()
{
    //ctor

}

editMode::~editMode()
{
    //dtor
    
}

bool editMode::PtInEditArea(int x, int y)
{
    //----------------------------------------------------------
  int limLeft, limRight, limTop, limBottom;
  PixelToMouse(0, 0, limLeft, limTop);
  PixelToMouse(m_nbHPixels + 1, m_nbVPixels + 1, limRight, limBottom);
  return (x >= limLeft) && (x <= limRight) && (y >= limTop) && (y < limBottom);
}


bool editMode::MouseToPixel(int mx,int my,int &pixelX,int &pixelY)
{
    //---------------------------------------------------------
    //Cairo::Matrix m;
    //m = Cairo::identity_matrix();
    //m.translate(m_origin_x,m_origin_y);
    //m.scale(m_scale,m_scale);
    //m.invert();
//    double dx = mx - m_origin_x - 2;
//    double dy = my - m_origin_y - 2;
    //m.transform_point(dx,dy);
    pixelX = (mx - 2) / m_offset;
    pixelY = (my - 2) / m_offset;
    return (pixelX>=0) && (pixelX<m_nbHPixels) && (pixelY>=0) && (pixelY<m_nbVPixels);

}

bool editMode::PixelToMouse(int pixelX,int pixelY,int &mouseX,int &mouseY)
/*----------------------------------------------------------------------------*\
    Description :



    Date de création : 24-04-2014                       Raymond NGUYEN THANH
\*----------------------------------------------------------------------------*/
{
    //---------------------------------------------------------
    mouseX = pixelX*m_offset + 2;
    mouseY = pixelY*m_offset + 2;

    // Cairo::Matrix m;
    // m = Cairo::identity_matrix();
    // m.translate(m_origin_x,m_origin_y);
    // m.scale(m_scale,m_scale);
    // //m.invert();
    // double dmx = mouseX;
    // double dmy = mouseY;
    // m.transform_point(dmx,dmy);
    // mouseX = dmx + 2;
    // mouseY = dmy + 2;
    return true;

}

void editMode::put_pixel(Glib::RefPtr<Gdk::Pixbuf> pixbuf, int x, int y, guchar red, guchar green, guchar blue, guchar alpha)
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

void editMode::get_pixel(Glib::RefPtr<Gdk::Pixbuf> pixbuf, int x, int y,
                         guchar *red, guchar *green, guchar *blue, guchar *alpha)
/*----------------------------------------------------------------------------*\
    Description :



    Date de création : 24-04-2014                       Raymond NGUYEN THANH
\*----------------------------------------------------------------------------*/
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

void editMode::CopyPixBuf(Glib::RefPtr<Gdk::Pixbuf> destPixbuf,Glib::RefPtr<Gdk::Pixbuf> srcPixbuf)
/*----------------------------------------------------------------------------*\
    Description :



    Date de création : 24-04-2014                       Raymond NGUYEN THANH
\*----------------------------------------------------------------------------*/
{
    int         i;
    int         rowstride, n_channels;
    guchar      *pixelsDest,*pDest;
    guchar      *pixelsSrc,*pSrc;
     //----------------------------------------------------------------------
    if ((destPixbuf)||(srcPixbuf)) return;

    n_channels = destPixbuf->get_n_channels();
    rowstride = destPixbuf->get_rowstride();// gdk_pixbuf_get_rowstride (pixbuf);
    if ((n_channels!=srcPixbuf->get_n_channels())||(rowstride!=srcPixbuf->get_rowstride())) return;

    pixelsDest = destPixbuf->get_pixels();// gdk_pixbuf_get_pixels (pixbuf);
    pixelsSrc = srcPixbuf->get_pixels();// gdk_pixbuf_get_pixels (pixbuf);

    gint h = srcPixbuf->get_height();
    gint w = srcPixbuf->get_width();

    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            i = (y * rowstride + x * n_channels);
            pixelsDest[i] = pixelsSrc[i++];
            pixelsDest[i] = pixelsSrc[i++];
            pixelsDest[i] = pixelsSrc[i++];
            pixelsDest[i] = pixelsSrc[i];
        }
    }

}
void editMode::BlitPixBuf(Glib::RefPtr<Gdk::Pixbuf> destPixbuf,int destX,int destY,
                            Glib::RefPtr<Gdk::Pixbuf> srcPixbuf,RRect srcRect)
{
    int         i,id;
    int         x,y;
    int         xd,yd;
    int         dest_rowstride, dest_n_channels;
    int         src_rowstride, src_n_channels;
    guchar      *pixelsDest;
    guchar      *pixelsSrc;
    //----------------------------------------------------------------------
    if ((destPixbuf)&&(srcPixbuf)){

        dest_n_channels = destPixbuf->get_n_channels();
        dest_rowstride = destPixbuf->get_rowstride();// gdk_pixbuf_get_rowstride (pixbuf);
        src_n_channels = srcPixbuf->get_n_channels();
        src_rowstride = srcPixbuf->get_rowstride();// gdk_pixbuf_get_rowstride (pixbuf);

        pixelsDest = destPixbuf->get_pixels();// gdk_pixbuf_get_pixels (pixbuf);
        pixelsSrc = srcPixbuf->get_pixels();// gdk_pixbuf_get_pixels (pixbuf);

        gint h = destPixbuf->get_height();
        gint w = destPixbuf->get_width();

        yd = destY;
        for(y=srcRect.top;y<srcRect.bottom;y++){
            if (yd>=0){
                if (yd<h){
                    xd = destX;
                    for(x=srcRect.left;x<srcRect.right;x++){
                        if (xd>=0){
                            if (xd<w){
                                i = (y * src_rowstride + x * src_n_channels);
                                id = (yd * dest_rowstride + xd * dest_n_channels);
                                pixelsDest[id++] = pixelsSrc[i++];
                                pixelsDest[id++] = pixelsSrc[i++];
                                pixelsDest[id++] = pixelsSrc[i++];
                                pixelsDest[id]   = pixelsSrc[i];
                            }else{
                                break;
                            }
                        }
                        //--
                        xd++;
                    }
                }else{
                    break;
                }
            }
            //--
            yd++;
        }
    }

}

void editMode::FillPixBuf(Glib::RefPtr<Gdk::Pixbuf> destPixbuf,RRect destRect,guint32 fillColor)
{
    int         i;
    int         x,y;
    int         xd,yd;
    int         rowstride, n_channels;
    guchar      *pixelsDest;
    guint8      r,g,b,a;
    //----------------------------------------------------------------------
    if (destPixbuf){
        n_channels = destPixbuf->get_n_channels();
        rowstride = destPixbuf->get_rowstride();// gdk_pixbuf_get_rowstride (pixbuf);

        pixelsDest = destPixbuf->get_pixels();// gdk_pixbuf_get_pixels (pixbuf);

        gint h = destPixbuf->get_height();
        gint w = destPixbuf->get_width();

        r = palette::GetRGBRed(fillColor);
        g = palette::GetRGBGreen(fillColor);
        b = palette::GetRGBBlue(fillColor);
        a = palette::GetRGBAlpha(fillColor);

        xd = destRect.left;
        if (xd<0) xd = 0;

        yd = destRect.top;
        if (yd<0) yd=0;

        for(y=yd;y<destRect.bottom;y++){
            if (y>=h){
                break;
            }else{
                for(x=xd;x<destRect.right;x++){
                    if (x>=w){
                        break;
                    }else{
                        i = (y * rowstride + x * n_channels);
                        pixelsDest[i++] = r;
                        pixelsDest[i++] = g;
                        pixelsDest[i++] = b;
                        pixelsDest[i]   = a;
                    }
                }
            }
        }
    }

}

void editMode::flip_horizontaly(Glib::RefPtr<Gdk::Pixbuf> pixbufSrc,Glib::RefPtr<Gdk::Pixbuf> pixbufDes)
/*----------------------------------------------------------------------------*\
    Description :


    Date de création : 24-04-2014                       Raymond NGUYEN THANH
\*----------------------------------------------------------------------------*/
{
    int width, height, rowstride, n_channels;
    guchar *pixelsSrc, *pSrc;
    guchar *pixelsDes, *pDes;
    guchar red, green, blue, alpha;
    //-----------------------------------------------------------------------
    if ((!pixbufSrc)||(!pixbufDes)) return;

    n_channels = pixbufSrc->get_n_channels();

    g_assert(pixbufSrc->get_colorspace() == Gdk::COLORSPACE_RGB);// GDK_COLORSPACE_RGB)

    g_assert(pixbufSrc->get_bits_per_sample() == 8);              //  
    g_assert(pixbufSrc->get_has_alpha());                         //
    g_assert(n_channels == 4);

    width = pixbufSrc->get_width();   // 
    height = pixbufSrc->get_height(); // 

    //g_assert(x >= 0 && x < width);
    //g_assert(y >= 0 && y < height);

    rowstride = pixbufSrc->get_rowstride();//

    pixelsSrc = pixbufSrc->get_pixels();      // 
    pixelsDes = pixbufDes->get_pixels();      // 

    int w = width-1;
    for (int y=0;y<height;y++){
        int yOffset = y * rowstride;
        for (int x=0;x<width;x++){
            pSrc = pixelsSrc + yOffset + x *n_channels;
            pDes = pixelsDes + yOffset + (w - x) * n_channels;
            pDes[0] = pSrc[0];
            pDes[1] = pSrc[1];
            pDes[2] = pSrc[2];
            pDes[3] = pSrc[3];
        }
    }

}

void editMode::flip_verticaly(Glib::RefPtr<Gdk::Pixbuf> pixbufSrc,Glib::RefPtr<Gdk::Pixbuf> pixbufDes)
/*----------------------------------------------------------------------------*\
    Description :


    Date de création : 24-04-2014                       Raymond NGUYEN THANH
\*----------------------------------------------------------------------------*/
{
    int width, height, rowstride, n_channels;
    guchar *pixelsSrc, *pSrc;
    guchar *pixelsDes, *pDes;
    guchar red, green, blue, alpha;
    //-----------------------------------------------------------------------
    if ((!pixbufSrc)||(!pixbufDes)) return;

    n_channels = pixbufSrc->get_n_channels();

    g_assert(pixbufSrc->get_colorspace() == Gdk::COLORSPACE_RGB); // 

    g_assert(pixbufSrc->get_bits_per_sample() == 8);              //  
    g_assert(pixbufSrc->get_has_alpha());                         //
    g_assert(n_channels == 4);

    width = pixbufSrc->get_width();   // 
    height = pixbufSrc->get_height(); // 

    //g_assert(x >= 0 && x < width);
    //g_assert(y >= 0 && y < height);

    rowstride = pixbufSrc->get_rowstride();

    pixelsSrc = pixbufSrc->get_pixels();
    pixelsDes = pixbufDes->get_pixels();

    int h = height-1;
    for (int y=0;y<height;y++){
        int ySrcOffset = y * rowstride;
        int yDesOffset = (h-y) * rowstride;
        for (int x=0;x<width;x++){
            pSrc = pixelsSrc + ySrcOffset + x *n_channels;
            pDes = pixelsDes + yDesOffset + x * n_channels;
            pDes[0] = pSrc[0];
            pDes[1] = pSrc[1];
            pDes[2] = pSrc[2];
            pDes[3] = pSrc[3];
        }
    }

}

void editMode::BackupSprite()
{
    int     w,h;
    //-------------------------------------------------------
    h = m_sprite->get_height();
    w = m_sprite->get_width();
    m_sprite->copy_area( 0, 0, w, h, m_sprite_bak, 0, 0);

}

void editMode::RestoreSprite()
{
    int     w,h;
    //-------------------------------------------------------
    h = m_sprite_bak->get_height();
    w = m_sprite_bak->get_width();
    m_sprite_bak->copy_area( 0, 0, w, h, m_sprite, 0, 0);

}

void editMode::UndoSprite()
{
    int     w,h;
    Glib::RefPtr<Gdk::Pixbuf>   sprite_redo;
    //-------------------------------------------------------
    h = m_sprite_bak->get_height();
    w = m_sprite_bak->get_width();
    sprite_redo = m_sprite->copy();
    m_sprite_bak->copy_area( 0, 0, w, h, m_sprite, 0, 0);
    sprite_redo->copy_area( 0, 0, w, h, m_sprite_bak, 0, 0);

}

void editMode::line(Glib::RefPtr<Gdk::Pixbuf> pixbuf, gint x0, gint y0,gint x1,gint y1,guint32 c)
/*----------------------------------------------------------------------------*\
    Description :



    Date de création : 24-04-2014                       Raymond NGUYEN THANH
\*----------------------------------------------------------------------------*/
{
    int       width, height, n_channels;
    gboolean  steep;
    gint      deltax,deltay;
    gint      error;
    gint      x,y,ystep;
    gint      dum;
    guchar    red,green,blue,alpha;
    //-----------------------------------------------------------------------

    if (!pixbuf) return;

    n_channels = pixbuf->get_n_channels();
    g_assert (pixbuf->get_colorspace() == Gdk::COLORSPACE_RGB); // gdk_pixbuf_get_colorspace (pixbuf)
    g_assert (pixbuf->get_bits_per_sample()== 8); //  gdk_pixbuf_get_bits_per_sample (pixbuf)
    g_assert (pixbuf->get_has_alpha()); //gdk_pixbuf_get_has_alpha (pixbuf)
    g_assert (n_channels == 4);

    width = pixbuf->get_width();// gdk_pixbuf_get_width (pixbuf);
    height = pixbuf->get_height(); // gdk_pixbuf_get_height (pixbuf);


    if ( ((x0 >= 0 && x0 < width) && (y0 >= 0 && y0 < height)) &&
          ((x1 >= 0 && x1 < width) && (y1 >= 0 && y1 < height)) ){

        red   = palette::GetRGBRed(c);
        green = palette::GetRGBGreen(c);
        blue  = palette::GetRGBBlue(c);
        alpha = palette::GetRGBAlpha(c);

        steep = (abs(y1-y0) > abs(x1-x0));
        if (steep){
            dum = x0;
            x0  = y0;
            y0  = dum;
            dum = x1;
            x1  = y1;
            y1  = dum;
        }
        if (x0>x1){
            dum = x0;
            x0  = x1;
            x1  = dum;
            dum = y0;
            y0  = y1;
            y1  = dum;
        }

        deltax = x1 - x0;
        deltay = abs(y1-y0);
        error  = deltax / 2;

        y  = y0;
        if (y0 < y1){
            ystep = 1;
        }else{
            ystep = -1;
        }

        for(x=x0;x<=x1;x++){
            if (steep){
                put_pixel( pixbuf , y, x, red, green, blue, alpha);
            }else{
                put_pixel( pixbuf , x, y, red, green, blue, alpha);
            }
            error = error - deltay;
            if (error<0){
                y = y + ystep;
                error = error + deltax;
            }
        }
    }

}


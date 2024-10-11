#include "editModeFill.h"
#include <iostream>

editModeFill::editModeFill():editMode()
{
    //ctor

}

editModeFill::~editModeFill()
{
    //dtor

}

bool editModeFill::on_button_press_event(GdkEventButton *event)
{
    int         pixelX,pixelY;
    guint32     color= m_foreGroundColor;
   //----------------------------------------------------
    std::cout << "editModePencil:on_button_press_event" << std::endl;
    int tmx = event->x - m_origin_x;
    int tmy = event->y - m_origin_y;

    if (MouseToPixel(tmx,tmy,pixelX,pixelY)){
        //-- Sauvegarder l'image d'origine
        SaveState();

        if (event->button==1){
            color = m_foreGroundColor;
        }else if (event->button==3){
            color = m_backGroundColor;
        }

        //--
        FloodFill(m_sprite, pixelX, pixelY,color);

        return true;

    }

    return false;
}

bool editModeFill::on_button_release_event(GdkEventButton *event)
{
    int         pixelX,pixelY;
   //---------------------------------------------------------
    std::cout << "editModePencil:on_button_release_event" << std::endl;
    
    return false;
}

gboolean editModeFill::floodFillGetColor(Glib::RefPtr<Gdk::Pixbuf> pixbuf, gint x, gint y,guint32 *col)
{
  int       width, height,
            rowstride, n_channels;
  guchar    *pixels,*p;
  //-----------------------------------------------------------------------

  width = pixbuf->get_width();// gdk_pixbuf_get_width (pixbuf);
  height = pixbuf->get_height(); // gdk_pixbuf_get_height (pixbuf);

  if ((x >= 0 && x < width) && (y >= 0 && y < height)){

	  n_channels = pixbuf->get_n_channels();
	  rowstride = pixbuf->get_rowstride();// gdk_pixbuf_get_rowstride (pixbuf);
	  pixels = pixbuf->get_pixels();// gdk_pixbuf_get_pixels (pixbuf);

      p = pixels + y * rowstride + x * n_channels;

      *col  = palette::RGBA(p[0],p[1],p[2],p[3]);

      return TRUE;
  }

  return FALSE;

}

void editModeFill::plot_pixel(Glib::RefPtr<Gdk::Pixbuf> pixbuf,gint x,gint y, guchar red, guchar green, guchar blue, guchar alpha)
/*----------------------------------------------------------------------------*\
    Description :



    Date de création : 24-04-2014                       Raymond NGUYEN THANH
\*----------------------------------------------------------------------------*/
{
    gint        rowstride, n_channels;
    guchar      *pixels,*p;
    //----------------------------------------------------------------------

    n_channels = pixbuf->get_n_channels();
    rowstride = pixbuf->get_rowstride();// gdk_pixbuf_get_rowstride (pixbuf);
    pixels = pixbuf->get_pixels();// gdk_pixbuf_get_pixels (pixbuf);

    p = pixels + y * rowstride + x * n_channels;
    p[0] = red;
    p[1] = green;
    p[2] = blue;
    p[3] = alpha;

}

void editModeFill::FloodFill(Glib::RefPtr<Gdk::Pixbuf> pixbuf, gint fillX, gint fillY,guint32 fillColor)
{
    int         width, height,n_channels;
    guint32     targetColor,curColor;
    RPoint	    *pPt;
    GQueue      *stk;
    gint		x,y;
    gint		startX,startY;
    gint		xEndLine,xStartLine;
    gboolean	fStartNord,fNord,
                fStartSud,fSud;
    guchar      red;
    guchar      green;
    guchar      blue;
    guchar      alpha;
    //-----------------------------------------------------------------------

    n_channels = pixbuf->get_n_channels();
    g_assert (pixbuf->get_colorspace() == Gdk::COLORSPACE_RGB); // gdk_pixbuf_get_colorspace (pixbuf)
    g_assert (pixbuf->get_bits_per_sample()== 8); //  gdk_pixbuf_get_bits_per_sample (pixbuf)
    g_assert (pixbuf->get_has_alpha()); //gdk_pixbuf_get_has_alpha (pixbuf)
    g_assert (n_channels == 4);


    if (!floodFillGetColor(pixbuf, fillX, fillY,&targetColor)||(targetColor==fillColor)) return;

    width = pixbuf->get_width();// gdk_pixbuf_get_width (pixbuf);
    height = pixbuf->get_height(); // gdk_pixbuf_get_height (pixbuf);



    red   = palette::GetRGBRed(fillColor);
    green = palette::GetRGBGreen(fillColor);
    blue  = palette::GetRGBBlue(fillColor);
    alpha = palette::GetRGBAlpha(fillColor);

    //-- Créer la pile
    stk = g_queue_new();

    pPt = new RPoint(fillX,fillY);
    g_queue_push_head( stk, pPt);

    while (g_queue_get_length(stk)){

        pPt = (RPoint *) g_queue_pop_head(stk);
        startX = pPt->x;
        startY = pPt->y;
        delete pPt;

        //-- Vérifier au Nord
        fStartNord = FALSE;

        if (startY>0){
            if (floodFillGetColor(pixbuf,startX,startY-1,&curColor)){
                if (curColor==targetColor){
                    pPt = new RPoint(startX,startY-1);
                    g_queue_push_head( stk, pPt);
                    fStartNord = TRUE;
                }
            }
        }

        //-- Vérifier au sud
        fStartSud = FALSE;
        if (startY<(height-1)){
            if (floodFillGetColor(pixbuf,startX,startY+1,&curColor)){
                if (curColor==targetColor){
                    pPt = new RPoint(startX,startY+1);
                    g_queue_push_head( stk, pPt);
                    fStartSud = TRUE;
                }
            }
        }

        y = startY;
        //-- Aller vers l'est
        xEndLine = startX;
        fNord = fStartNord;
        fSud = fStartSud;
        if (xEndLine<(width-1)){

            x = xEndLine + 1;
            do{
                if (!floodFillGetColor(pixbuf,x,y,&curColor)) break;
                if (curColor!=targetColor){
                    break;
                }

                //-- Vérifier au Nord
                if ((y>0)&&(floodFillGetColor(pixbuf,x,y-1,&curColor))){
                    if (curColor==targetColor){
                        if (!fNord){
                            pPt = new RPoint(x,y-1);
                            g_queue_push_head( stk, pPt);
                            fNord = TRUE;
                        }
                    }else{
                        fNord = FALSE;
                    }
                }else{
                    fNord = FALSE;
                }

                //-- Vérifier au sud
                if ((y<(height-1))&&(floodFillGetColor(pixbuf,x,y+1,&curColor))){
                    if (curColor==targetColor){
                        if (!fSud){
                            pPt = new RPoint(x,y+1);
                            g_queue_push_head( stk, pPt);
                            fSud = TRUE;

                        }
                    }else{
                        fSud = FALSE;
                    }
                }else{
                    fSud = FALSE;
                }
                xEndLine = x;
                x++;
            }while(x<width);

        }else{
            xEndLine = width-1;
        }

        //-- Aller vers l'ouest
        xStartLine = startX;
        fNord = fStartNord;
        fSud = fStartSud;
        if (xStartLine>0){
            x = xStartLine - 1;
            do{
                if (!floodFillGetColor(pixbuf,x,y,&curColor)) break;
                if (curColor!=targetColor) break;

                //-- Vérifier au Nord
                if ((y>0)&&(floodFillGetColor(pixbuf,x,y-1,&curColor))){
                    if (curColor==targetColor){
                        if (!fNord){
                            pPt = new RPoint(x,y-1);
                            g_queue_push_head( stk, pPt);
                            fNord = TRUE;
                        }
                    }else{
                        fNord = FALSE;
                    }
                }else{
                    fNord = FALSE;
                }

                //-- Vérifier au sud
                if ((y<(height-1))&&(floodFillGetColor(pixbuf,x,y+1,&curColor))){
                    if (curColor==targetColor){
                        if (!fSud){
                            pPt = new RPoint(x,y+1);
                            g_queue_push_head( stk, pPt);
                            fSud = TRUE;
                        }
                    }else{
                        fSud = FALSE;
                    }
                }else{
                    fSud = FALSE;
                }
                xStartLine = x;
                x--;
            }while(x>=0);
        }else{
            xStartLine = 0;
        }
        //-- Tracer la line
        for(x=xStartLine;x<=xEndLine;x++){
            plot_pixel( pixbuf, x, y, red, green, blue, alpha);

        }

    }

    //-- Libérer la pile
    g_queue_free(stk);


}

bool editModeFill::on_motion_notify_event(GdkEventMotion *event)
{
    //---------------------------------------------------------
    
    return false;

}

bool editModeFill::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    //---------------------------------------------------------

    return false;
}

void editModeFill::init_mode()
{

}

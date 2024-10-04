#include "editModeEllipse.h"
#include <iostream>

editModeEllipse::editModeEllipse()
    : editMode(), m_start_pt(NULL), m_end_pt(NULL), m_select_handle(NULL) {
    // ctor
    for (int i = 0; i < 4; i++) {
        m_handles[i] = new RHandle(0, 0, 0, 0);
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

editModeEllipse::~editModeEllipse() {
    // dtor
    for (int i = 0; i < 4; i++) {
        if (m_handles[i]) delete (m_handles[i]);
    }
}

bool editModeEllipse::on_button_press_event(GdkEventButton *event) {
    int pixelX, pixelY;
    //----------------------------------------------------
    int tmx = event->x - m_origin_x;
    int tmy = event->y - m_origin_y;
    if (event->button == 1) {

        if (m_select_handle = HitHandle(tmx, tmy)) {
            std::cout << "editModeRect:Hit Handle" << std::endl;
            return false;
        } else {

            int x, y;
            if (MouseToPixel(tmx, tmy, x, y)) {
                if (m_rect_current_pix.PtInRect(x, y)) {
                    m_select_move_flag = true;
                    m_rect_current_pix_sav = m_rect_current_pix;
                    m_start_pix_x = x;
                    m_start_pix_y = y;
                    return false;
                }
            }
        }
    }

    InitHandles();
    m_select_move_flag = false;
    m_rect_current_pix.SetNULL();
    if (MouseToPixel(tmx, tmy, pixelX, pixelY)) {
        if (!m_start_pt) {
            m_start_pt = new Gdk::Point(pixelX, pixelY);
            //-- Sauvegarder l'image d'origine
            // m_sprite_backup = m_sprite->copy();
            BackupSprite();
        }
    }

    return true;
}

bool editModeEllipse::on_button_release_event(GdkEventButton *event) {
    int pixelX, pixelY;
    //---------------------------------------------------------
    std::cout << "editModePencil:on_button_release_event" << std::endl;
    if (m_start_pt) {
        delete m_start_pt;
        m_start_pt = NULL;
    }
    if (m_end_pt) {
        delete m_end_pt;
        m_end_pt = NULL;
    }

    //-- Normalize le rectangle de sélection
    if (!m_rect_current_pix.IsNULL()) {
        if (m_x1 > m_x2) {
            int dum = m_x1;
            m_x1 = m_x2;
            m_x2 = dum;
        }
        if (m_y1 > m_y2) {
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
        m_rect_current_pix.right = endX + 1;
        m_rect_current_pix.bottom = endY + 1;
    }

    m_select_handle = NULL;

    return false;
}

void editModeEllipse::DrawHorizontalLine(Glib::RefPtr<Gdk::Pixbuf> pixbuf,
                                         int xLeft, int xRight, int y,
                                         guint32 col) {
    int rowstride, n_channels;
    guchar *pixels, *p;
    guchar red, green, blue, alpha;
    //---------------------------------------------

    n_channels =
        pixbuf->get_n_channels();        // gdk_pixbuf_get_n_channels (pixbuf);
    rowstride = pixbuf->get_rowstride(); // gdk_pixbuf_get_rowstride (pixbuf);
    pixels = pixbuf->get_pixels();       // gdk_pixbuf_get_pixels (pixbuf);

    red = palette::GetRGBRed(col);
    green = palette::GetRGBGreen(col);
    blue = palette::GetRGBBlue(col);
    alpha = palette::GetRGBAlpha(col);

    for (int x = xLeft; x <= xRight; x++) {
        p = pixels + y * rowstride + x * n_channels;
        p[0] = red;
        p[1] = green;
        p[2] = blue;
        p[3] = alpha;
    }
}

void editModeEllipse::FillEllipse(Glib::RefPtr<Gdk::Pixbuf> pixbuf, int left,
                                  int top, int right, int bottom, guint32 col) {
    //---------------------------------------------
    int a = (right - left) / 2;
    int b = (bottom - top) / 2;

    int x = 0;
    int y = b;

    int a2 = a * a;
    int b2 = b * b;
    int a2b2 = a2 + b2;
    int a2sqr = a2 + a2;
    int b2sqr = b2 + b2;
    int a4sqr = a2sqr + a2sqr;
    int b4sqr = b2sqr + b2sqr;
    int a8sqr = a4sqr + a4sqr;
    int b8sqr = b4sqr + b4sqr;
    int a4sqr_b4sqr = a4sqr + b4sqr;

    int fn = a8sqr + a4sqr;
    int fnn = a8sqr;
    int fnnw = a8sqr;
    int fnw = a8sqr + a4sqr - b8sqr * a + b8sqr;
    int fnwn = a8sqr;
    int fnwnw = a8sqr + b8sqr;
    int fnww = b8sqr;
    int fwnw = b8sqr;
    int fww = b8sqr;
    int d1 = b2 - b4sqr * a + a4sqr;

    while ((fnw < a2b2) || (d1 < 0) || (((fnw - fn) > b2) && (y > 0))) {

        DrawHorizontalLine(pixbuf, left + x, right - x, top + y, col);
        DrawHorizontalLine(pixbuf, left + x, right - x, bottom - y, col);

        y -= 1;
        if ((d1 < 0) || ((fnw - fn) > b2)) {
            d1 += fn;
            fn += fnn;
            fnw += fnwn;
        } else {
            x += 1;
            d1 += fnw;
            fn += fnnw;
            fnw += fnwnw;
        }
    }

    int fw = fnw - fn + b4sqr;
    int d2 = d1 + (fw + fw - fn - fn + a4sqr_b4sqr + a8sqr) / 4;
    fnw += b4sqr - a4sqr;

    int old_y = y + 1;

    while (x <= a) {
        if (y != old_y) {
            DrawHorizontalLine(pixbuf, left + x, right - x, top + y, col);
            DrawHorizontalLine(pixbuf, left + x, right - x, bottom - y, col);
        }
        old_y = y;
        x += 1;
        if (d2 < 0) {
            y -= 1;
            d2 += fnw;
            fw += fwnw;
            fnw += fnwnw;
        } else {
            d2 += fw;
            fw += fww;
            fnw += fnww;
        }
    }
}

void editModeEllipse::DrawFillEllipse(Glib::RefPtr<Gdk::Pixbuf> pixbuf,
                                      int startX, int startY, int endX,
                                      int endY, guint32 col) {
    int dum;
    //----------------------------------------------------------------------
    if (endX < startX) {
        dum = startX;
        startX = endX;
        endX = dum;
    }
    if (endY < startY) {
        dum = startY;
        startY = endY;
        endY = dum;
    }
    if ((endX != startX) || (endY != startY)) {
        FillEllipse(pixbuf, startX, startY, endX, endY, col);
    }
}

void editModeEllipse::BorderEllipse(Glib::RefPtr<Gdk::Pixbuf> pixbuf, int left,
                                  int top, int right, int bottom, guint32 col) {
    //---------------------------------------------
    int a = (right - left) / 2;
    int b = (bottom - top) / 2;

    int x = 0;
    int y = b;

    int a2 = a * a;
    int b2 = b * b;
    int a2b2 = a2 + b2;
    int a2sqr = a2 + a2;
    int b2sqr = b2 + b2;
    int a4sqr = a2sqr + a2sqr;
    int b4sqr = b2sqr + b2sqr;
    int a8sqr = a4sqr + a4sqr;
    int b8sqr = b4sqr + b4sqr;
    int a4sqr_b4sqr = a4sqr + b4sqr;

    int fn = a8sqr + a4sqr;
    int fnn = a8sqr;
    int fnnw = a8sqr;
    int fnw = a8sqr + a4sqr - b8sqr * a + b8sqr;
    int fnwn = a8sqr;
    int fnwnw = a8sqr + b8sqr;
    int fnww = b8sqr;
    int fwnw = b8sqr;
    int fww = b8sqr;
    int d1 = b2 - b4sqr * a + a4sqr;

    guchar red = palette::GetRGBRed(col);
    guchar green = palette::GetRGBGreen(col);
    guchar blue = palette::GetRGBBlue(col);
    guchar alpha = palette::GetRGBAlpha(col);


    while ((fnw < a2b2) || (d1 < 0) || (((fnw - fn) > b2) && (y > 0))) {

        //DrawHorizontalLine(pixbuf, left + x, right - x, top + y, col);
        put_pixel(pixbuf, left + x, top + y, red, green, blue, alpha);
        put_pixel(pixbuf, right - x, top + y, red, green, blue, alpha);
        //DrawHorizontalLine(pixbuf, left + x, right - x, bottom - y, col);
        put_pixel(pixbuf, left + x, bottom - y, red, green, blue, alpha);
        put_pixel(pixbuf, right - x, bottom - y, red, green, blue, alpha);


        y -= 1;
        if ((d1 < 0) || ((fnw - fn) > b2)) {
            d1 += fn;
            fn += fnn;
            fnw += fnwn;
        } else {
            x += 1;
            d1 += fnw;
            fn += fnnw;
            fnw += fnwnw;
        }
    }

    int fw = fnw - fn + b4sqr;
    int d2 = d1 + (fw + fw - fn - fn + a4sqr_b4sqr + a8sqr) / 4;
    fnw += b4sqr - a4sqr;

    //int old_y = y + 1;

    while (x <= a) {
        //if (y != old_y) {
            //DrawHorizontalLine(pixbuf, left + x, right - x, top + y, col);
            put_pixel(pixbuf, left + x, top + y, red, green, blue, alpha);
            put_pixel(pixbuf, right - x, top + y, red, green, blue, alpha);
            //DrawHorizontalLine(pixbuf, left + x, right - x, bottom - y, col);
            put_pixel(pixbuf, left + x, bottom - y, red, green, blue, alpha);
            put_pixel(pixbuf, right - x, bottom - y, red, green, blue, alpha);
        //}
        //old_y = y;
        x += 1;
        if (d2 < 0) {
            y -= 1;
            d2 += fnw;
            fw += fwnw;
            fnw += fnwnw;
        } else {
            d2 += fw;
            fw += fww;
            fnw += fnww;
        }
    }
}


void editModeEllipse::DrawEllipse(Glib::RefPtr<Gdk::Pixbuf> pixbuf,
                                      int startX, int startY, int endX,
                                      int endY, guint32 col) {
    int dum;
    //----------------------------------------------------------------------
    if (endX < startX) {
        dum = startX;
        startX = endX;
        endX = dum;
    }
    if (endY < startY) {
        dum = startY;
        startY = endY;
        endY = dum;
    }
    if ((endX != startX) || (endY != startY)) {
        BorderEllipse(pixbuf, startX, startY, endX, endY, col);
    }
}


RHandle *editModeEllipse::HitHandle(int mx, int my) {
    //---------------------------------------------------------
    for (int i = 0; i < 4; i++) {
        if (m_handles[i]->PtInRect(mx, my)) {
            return m_handles[i];
        }
    }
    return NULL;
}

bool editModeEllipse::RectInEditArea(RRect &rect) {
    //----------------------------------------------------------
    if ((rect.left < 0) || (rect.left > m_nbHPixels)) {
        return false;
    }
    if ((rect.right < 0) || (rect.right > m_nbHPixels)) {
        return false;
    }
    if ((rect.top < 0) || (rect.top > m_nbVPixels)) {
        return false;
    }
    if ((rect.bottom < 0) || (rect.bottom > m_nbVPixels)) {
        return false;
    }
    return true;
}

bool editModeEllipse::on_motion_notify_event(GdkEventMotion *event) {
    guint8 r, g, b, a;
    int pixelX, pixelY;
    bool fButton = false;
    //---------------------------------------------------------
    // std::cout << "editModePencil:on_motion_notify_event" << std::endl;

    if (m_select_handle) {

        int left, top;
        int tmx = event->x - m_origin_x;
        int tmy = event->y - m_origin_y;
        if (MouseToPixel(tmx, tmy, pixelX, pixelY)) {
            PixelToMouse(pixelX, pixelY, left, top);
            *(m_select_handle->m_x) = left;
            *(m_select_handle->m_y) = top;

            int startX, startY;
            int endX, endY;
            MouseToPixel(m_x1, m_y1, startX, startY);
            MouseToPixel(m_x2, m_y2, endX, endY);

            m_rect_current_pix.left = startX;
            m_rect_current_pix.top = startY;
            m_rect_current_pix.right = endX + 1;
            m_rect_current_pix.bottom = endY + 1;

            RestoreSprite();

            if ((startX != endX) || (startY != endY)) {
                if (m_current_fill_mode) {
                    DrawFillEllipse(m_sprite, startX, startY, endX, endY,
                                    m_current_color);
                } else {
                    DrawEllipse(m_sprite, startX, startY, endX, endY,
                                m_current_color);
                }
            }
            return true;
        }

    } else if (m_start_pt) {
        if (event->state & Gdk::BUTTON1_MASK) {
            m_current_color = m_foreGroundColor;
            fButton = true;
        } else if (event->state & Gdk::BUTTON3_MASK) {
            m_current_color = m_backGroundColor;
            fButton = true;
        }

        if (fButton) {
            int tmx = event->x - m_origin_x;
            int tmy = event->y - m_origin_y;

            MouseToPixel(tmx, tmy, pixelX, pixelY);
            int maxH = m_nbHPixels - 1;
            if (pixelX > maxH) {
                pixelX = maxH;
            }
            int maxV = m_nbVPixels - 1;
            if (pixelY > maxV) {
                pixelY = maxV;
            }
            RestoreSprite();
            if (!m_end_pt) {
                m_end_pt = new Gdk::Point(pixelX, pixelY);
            } else {
                m_end_pt->set_x(pixelX);
                m_end_pt->set_y(pixelY);
            }
            int startX = m_start_pt->get_x();
            int startY = m_start_pt->get_y();
            int endX = m_end_pt->get_x();
            int endY = m_end_pt->get_y();

            if (startX > endX) {
                int dum = startX;
                startX = endX;
                endX = dum;
            }
            if (startY > endY) {
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
            m_rect_current_pix.right = endX + 1;
            m_rect_current_pix.bottom = endY + 1;

            if ((startX != endX) || (startY != endY)) {

                if (event->state & GDK_CONTROL_MASK) {
                    m_current_fill_mode = true;
                    DrawFillEllipse(m_sprite, startX, startY, endX, endY,
                                    m_current_color);
                } else {
                    m_current_fill_mode = false;
                    DrawEllipse(m_sprite, startX, startY, endX, endY,
                                m_current_color);
                }
            }
            return true;
        }

    } else if (m_select_move_flag) {
        int x, y;
        int vx = 0, vy = 0;

        if (event->state & Gdk::BUTTON1_MASK) {
            int tmx = event->x - m_origin_x;
            int tmy = event->y - m_origin_y;
            if (PtInEditArea(tmx, tmy)) {
                if (MouseToPixel(tmx, tmy, x, y) &&
                    ((x != m_start_pix_x) || (y != m_start_pix_y))) {
                    RRect rect;
                    vx = x - m_start_pix_x;
                    vy = y - m_start_pix_y;
                    rect = m_rect_current_pix_sav;
                    rect.Offset(vx, vy);

                    if (!RectInEditArea(rect)) {
                        m_rect_current_pix_sav = m_rect_current_pix;
                        m_start_pix_x = x;
                        m_start_pix_y = y;
                    } else {
                        m_rect_current_pix = rect;
                    }

                    int startX, startY;
                    int endX, endY;
                    startX = m_rect_current_pix.left;
                    startY = m_rect_current_pix.top;
                    endX = m_rect_current_pix.right - 1;
                    endY = m_rect_current_pix.bottom - 1;

                    int left, top;
                    int right, bottom;
                    PixelToMouse(startX, startY, left, top);
                    PixelToMouse(endX, endY, right, bottom);

                    m_x1 = left;
                    m_y1 = top;
                    m_x2 = right;
                    m_y2 = bottom;

                    RestoreSprite();
                    if ((startX != endX) || (startY != endY)) {
                        if (event->state & GDK_CONTROL_MASK) {
                            m_current_fill_mode = true;
                            DrawFillEllipse(m_sprite, startX, startY, endX,
                                            endY, m_current_color);
                        } else {
                            m_current_fill_mode = false;
                            DrawEllipse(m_sprite, startX, startY, endX, endY,
                                        m_current_color);
                        }
                    }
                    return true;
                }
            }
        }
    }
    return false;
}

bool editModeEllipse::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
    RRect rect;
    guchar red, green, blue, alpha;
    //---------------------------------------------------------

    red = palette::GetRGBRed(m_handle_color);
    green = palette::GetRGBGreen(m_handle_color);
    blue = palette::GetRGBBlue(m_handle_color);
    alpha = palette::GetRGBAlpha(m_handle_color);

    //-- Update Handles
    if (!m_rect_current_pix.IsNULL()) {
        PixelToMouse(m_rect_current_pix.left, m_rect_current_pix.top, m_x1,
                     m_y1);
        PixelToMouse(m_rect_current_pix.right - 1,
                     m_rect_current_pix.bottom - 1, m_x2, m_y2);
        m_handles[RHandle::TOP_LEFT]->UpdateRect(m_offset);
        m_handles[RHandle::BOTTOM_LEFT]->UpdateRect(m_offset);
        m_handles[RHandle::TOP_RIGHT]->UpdateRect(m_offset);
        m_handles[RHandle::BOTTOM_RIGHT]->UpdateRect(m_offset);
    }

    for (int i = 0; i < 4; i++) {
        if (!m_handles[i]->IsNULL()) {
            cr->set_source_rgba(red / 255.0f, green / 255.0f, blue / 255.0f,
                                alpha / 255.0f);
            rect.left = m_handles[i]->m_left;
            rect.top = m_handles[i]->m_top;
            rect.right = m_handles[i]->m_right;
            rect.bottom = m_handles[i]->m_bottom;
            rect.Deflate(2, 2, 3, 3);
            cr->rectangle(rect.left, rect.top, rect.Width(), rect.Height());
            cr->fill();
            cr->set_source_rgba(200 / 255.0f, 200 / 255.0f, 200 / 255.0f,
                                alpha / 255.0f);
            rect.Deflate(1, 1, 1, 1);
            cr->rectangle(rect.left, rect.top, rect.Width(), rect.Height());
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

void editModeEllipse::InitHandles() {
    //---------------------------------------------------------
    for (int i = 0; i < 4; i++) {
        m_handles[i]->SetNULL();
    }
    m_select_handle = NULL;
}

void editModeEllipse::init_mode() {
    InitHandles();
}

#include "editModeSelect.h"
#include <iostream>

editModeSelect::editModeSelect()
    : editMode(), m_start_pt(NULL), m_end_pt(NULL), m_select_handle(NULL),
      m_f_draw_handles(false) {
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

editModeSelect::~editModeSelect() {
  // dtor
  for (int i = 0; i < 4; i++) {
    if (m_handles[i])
      delete (m_handles[i]);
  }
}

bool editModeSelect::on_button_press_event(GdkEventButton *event) {
  int pixelX, pixelY;
  int x, y;
  //---------------------------------------------------------
  m_select_move_flag = false;

  int tmx = event->x - m_origin_x;
  int tmy = event->y - m_origin_y;

  if ((m_select_handle = HitHandle(tmx, tmy))!=NULL) {
    std::cout << "editModeRect:Hit Handle" << std::endl;
  } else {

    MouseToPixel(tmx, tmy, x, y);
    if (m_rect_select_pix.PtInRect(x, y)) {
      m_select_move_flag = true;
      m_rect_select_pix_sav = m_rect_select_pix;
      m_start_pix_x = x;
      m_start_pix_y = y;
    } else {
      InitHandles();
      m_rect_select_pix.SetNULL();
      m_select_pixbuf = (Glib::RefPtr<Gdk::Pixbuf>)NULL;
      m_f_draw_handles = false;
      if (MouseToPixel(tmx, tmy, pixelX, pixelY)) {
        if (!m_start_pt) {
          m_f_draw_handles = true;
          m_start_pt = new Gdk::Point(pixelX, pixelY);
        }
      }
    }
  }

  return true;
}

bool editModeSelect::on_button_release_event(GdkEventButton *event) {
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

  //-- Normalize le rectangle de séléction
  if (!m_rect_select_pix.IsNULL()) {
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
    m_rect_select_pix.left = startX;
    m_rect_select_pix.top = startY;
    m_rect_select_pix.right = endX + 1;
    m_rect_select_pix.bottom = endY + 1;
  }

  m_select_handle = NULL;
  m_select_move_flag = false;

  return true;
}

RHandle *editModeSelect::HitHandle(int mx, int my) {
  //---------------------------------------------------------
  // Cairo::Matrix m;
  // m = Cairo::identity_matrix();
  // m.translate(m_origin_x,m_origin_y);
  // m.scale(m_scale,m_scale);
  // m.invert();
  // double dx = mx;
  // double dy = my;
  // m.transform_point(dx,dy);

  for (int i = 0; i < 4; i++) {
    if (m_handles[i]->PtInRect(mx, my)) {
      return m_handles[i];
    }
  }
  return NULL;
}


bool editModeSelect::on_motion_notify_event(GdkEventMotion *event) {
  int pixelX, pixelY;
  guint32 color;
  //---------------------------------------------------------
  if (m_select_handle) {
    if (event->state & Gdk::BUTTON1_MASK) {
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
        int left, top;
        int right, bottom;
        PixelToMouse(startX, startY, left, top);
        PixelToMouse(endX, endY, right, bottom);
        m_x1 = left;
        m_y1 = top;
        m_x2 = right;
        m_y2 = bottom;

        int x1 = m_x1;
        int x2 = m_x2;
        int y1 = m_y1;
        int y2 = m_y2;
        if (x1 > x2) {
          int dum = x1;
          x1 = x2;
          x2 = dum;
        }
        if (y1 > y2) {
          int dum = y1;
          y1 = y2;
          y2 = dum;
        }
        MouseToPixel(x1, y1, startX, startY);
        MouseToPixel(x2, y2, endX, endY);
        m_rect_select_pix.left = startX;
        m_rect_select_pix.top = startY;
        m_rect_select_pix.right = endX + 1;
        m_rect_select_pix.bottom = endY + 1;

        return true;
      }
    }
  } else if (m_start_pt) {
    if (event->state & Gdk::BUTTON1_MASK) {
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
      // RestoreSprite();
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

      //--
      int left, top;
      int right, bottom;
      PixelToMouse(startX, startY, left, top);
      PixelToMouse(endX, endY, right, bottom);

      m_x1 = left;
      m_y1 = top;
      m_x2 = right;
      m_y2 = bottom;

      int x1 = m_x1;
      int x2 = m_x2;
      int y1 = m_y1;
      int y2 = m_y2;
      if (x1 > x2) {
        int dum = x1;
        x1 = x2;
        x2 = dum;
      }
      if (y1 > y2) {
        int dum = y1;
        y1 = y2;
        y2 = dum;
      }
      MouseToPixel(x1, y1, startX, startY);
      MouseToPixel(x2, y2, endX, endY);
      m_rect_select_pix.left = startX;
      m_rect_select_pix.top = startY;
      m_rect_select_pix.right = endX + 1;
      m_rect_select_pix.bottom = endY + 1;

      return true;
    }
  } else if (m_select_move_flag) {
    int x, y;
    int vx, vy;

    if (event->state & Gdk::BUTTON1_MASK) {

      // Cairo::Matrix m;
      // m = Cairo::identity_matrix();
      // m.translate(m_origin_x,m_origin_y);
      // m.scale(m_scale,m_scale);
      // m.invert();
      // double dmx = event->x;
      // double dmy = event->y;
      // m.transform_point(dmx,dmy);
      int tmx = event->x - m_origin_x;
      int tmy = event->y - m_origin_y;

      if (PtInEditArea(tmx, tmy)) {
        MouseToPixel(tmx, tmy, x, y);
        vx = x - m_start_pix_x;
        vy = y - m_start_pix_y;
        m_rect_select_pix = m_rect_select_pix_sav;
        m_rect_select_pix.Offset(vx, vy);

        int startX, startY;
        int endX, endY;
        startX = m_rect_select_pix.left;
        startY = m_rect_select_pix.top;
        endX = m_rect_select_pix.right - 1;
        endY = m_rect_select_pix.bottom - 1;
        int left, top;
        int right, bottom;
        PixelToMouse(startX, startY, left, top);
        PixelToMouse(endX, endY, right, bottom);

        m_x1 = left;
        m_y1 = top;
        m_x2 = right;
        m_y2 = bottom;

        if ((startX != endX) || (startY != endY)) {
          if (m_select_pixbuf) {
            //-- restore backup
            RestoreStartState();
            //--
            BlitPixBuf(m_sprite, m_rect_select_pix.left, m_rect_select_pix.top,
                       m_select_pixbuf,
                       RRect(0, 0, m_rect_select_pix.Width(),
                             m_rect_select_pix.Height()));
            // m_select_pixbuf->copy_area(0,0,m_rect_select_pix.Width(),m_rect_select_pix.Height(),
            //                    m_sprite,m_rect_select_pix.left,m_rect_select_pix.top);
          }
        }
        return true;
      }
    }
  }

  return false;
}

bool editModeSelect::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
  bool fRefresh(false);
  RRect rect;
  guchar red, green, blue, alpha;
  //---------------------------------------------------------

  red = palette::GetRGBRed(m_handle_color);
  green = palette::GetRGBGreen(m_handle_color);
  blue = palette::GetRGBBlue(m_handle_color);
  alpha = palette::GetRGBAlpha(m_handle_color);

  //-- Update Handles
  if (!m_rect_select_pix.IsNULL()) {
    PixelToMouse(m_rect_select_pix.left, m_rect_select_pix.top, m_x1, m_y1);
    PixelToMouse(m_rect_select_pix.right - 1, m_rect_select_pix.bottom - 1,
                 m_x2, m_y2);
    m_handles[RHandle::TOP_LEFT]->UpdateRect(m_offset);
    m_handles[RHandle::BOTTOM_LEFT]->UpdateRect(m_offset);
    m_handles[RHandle::TOP_RIGHT]->UpdateRect(m_offset);
    m_handles[RHandle::BOTTOM_RIGHT]->UpdateRect(m_offset);
  }

  if (m_f_draw_handles) {
    //-- Draw Handles
    for (int i = 0; i < 4; i++) {
      if (!m_handles[i]->IsNULL()) {
        rect.left = m_handles[i]->m_left;
        rect.top = m_handles[i]->m_top;
        rect.right = m_handles[i]->m_right;
        rect.bottom = m_handles[i]->m_bottom;
        if (PtInEditArea(rect.left, rect.top) &&
            PtInEditArea(rect.right, rect.bottom)) {
          cr->set_source_rgba(red / 255.0f, green / 255.0f, blue / 255.0f,
                              alpha / 255.0f);
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
    }
    fRefresh = true;
  }
  //-- Draw Sélection frame
  if (!m_rect_select_pix.IsNULL()) {
    int scrLeft, scrTop, scrRight, scrBottom;
    int left, top, right, bottom;

    //-- Make a Clipping
    if (m_rect_select_pix.left < 0) {
      left = 0;
    } else {
      left = m_rect_select_pix.left;
    }
    if (m_rect_select_pix.top < 0) {
      top = 0;
    } else {
      top = m_rect_select_pix.top;
    }
    if (m_rect_select_pix.right >= m_nbHPixels) {
      right = m_nbHPixels;
    } else {
      right = m_rect_select_pix.right;
    }
    if (m_rect_select_pix.bottom >= m_nbHPixels) {
      bottom = m_nbHPixels;
    } else {
      bottom = m_rect_select_pix.bottom;
    }

    PixelToMouse(left, top, scrLeft, scrTop);
    PixelToMouse(right, bottom, scrRight, scrBottom);
    cr->set_source_rgba(red / 255.0f, green / 255.0f, blue / 255.0f, 0.1f);
    cr->rectangle(scrLeft, scrTop, scrRight - scrLeft, scrBottom - scrTop);
    cr->fill();

    // cr->set_source_rgba(red / 255.0f, green / 255.0f, blue / 255.0f, alpha /
    // 255.0f); cr->set_line_width(0.8);
    // cr->move_to(scrLeft, scrTop);
    // cr->line_to(scrRight, scrTop);
    // cr->line_to(scrRight, scrBottom);
    // cr->line_to(scrLeft, scrBottom);
    // cr->line_to(scrLeft, scrTop);
    // cr->stroke();
    fRefresh = true;
  }

  return fRefresh;
}

void editModeSelect::InitHandles() {
  //---------------------------------------------------------
  for (int i = 0; i < 4; i++) {
    m_handles[i]->SetNULL();
  }
  m_select_handle = NULL;
  m_f_draw_handles = false;
}

void editModeSelect::init_mode() {
  //---------------------------------------------------------
  m_rect_select_pix.SetNULL();
  InitHandles();
}
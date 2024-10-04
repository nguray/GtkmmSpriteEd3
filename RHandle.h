#pragma once

class RHandle
{
private:
    /* data */
public:

    enum{
        TOP_LEFT=0,
        TOP_RIGHT,
        BOTTOM_RIGHT,
        BOTTOM_LEFT
    };

    RHandle(int left,int top,int right,int bottom);
    ~RHandle();

    bool    PtInRect(int x,int y);
    bool    IsNULL();
    void    SetNULL();
    int     Width();
    int     Height();
    void    UpdateRect(int offSet);

    int     m_left;
    int     m_top;
    int     m_right;
    int     m_bottom;

    int     *m_x;
    int     *m_y;

};


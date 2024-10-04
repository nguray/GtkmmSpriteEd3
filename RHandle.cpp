#include <cstddef>
#include "RHandle.h"

RHandle::RHandle(int left,int top,int right,int bottom):
    m_left(left), m_top(top), m_right(right), m_bottom(bottom),
    m_x(NULL), m_y(NULL)
{
}

RHandle::~RHandle()
{

}

bool RHandle::PtInRect(int x,int y)
{
    return (!IsNULL()) && (x>=m_left) && (x<=m_right) && (y>=m_top) && (y<=m_bottom);

}

bool RHandle::IsNULL()
{
    return (!(m_left || m_top || m_right || m_bottom));
}

void RHandle::SetNULL()
{
    m_left = m_top = m_right = m_bottom = 0;
}

int RHandle::Width()
{
    return m_right-m_left;
}

int RHandle::Height()
{
    return m_bottom-m_top;
}

void RHandle::UpdateRect(int offSet)
{
    m_left = *m_x;
    m_top = *m_y;
    m_right = *m_x + offSet;
    m_bottom = *m_y + offSet;

}
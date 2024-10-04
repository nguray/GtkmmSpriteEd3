/*
 * RRect.cpp
 *
 *  Created on: 29 juin 2015
 *      Author: nguray
 */

#include "RRect.h"

RRect::RRect():left(0),top(0),right(0),bottom(0)
{
    //ctor
}

RRect::RRect(int l,int t,int r,int b):left(l),top(t),right(r),bottom(b)
{
    //ctor
}

RRect::~RRect()
{
    //dtor
}

int RRect::Width()
{
    return right-left;
}

int RRect::Height()
{
    return bottom-top;
}

void RRect::Normalize()
{
    int     dum;
    //------------------------------------------------------------------
    if (left>right){
        dum = left;
        left = right;
        right = dum;
    }
    if (top>bottom){
        dum = top;
        top = bottom;
        bottom = dum;
    }

}

void RRect::Offset(int dx,int dy)
{
    //------------------------------------------------------------------
    left    += dx;
    right   += dx;
    top     += dy;
    bottom  += dy;
}

bool RRect::PtInRect(int x,int y)
{
    return (!IsNULL()) && (x>=left) && (x<=right) && (y>=top) && (y<=bottom);

}

void RRect::SetNULL()
{
    left = top = right = bottom = 0;
}

bool RRect::IsNULL()
{
    return (!(left | top | right | bottom));
}

void RRect::Deflate(int l,int t,int r,int b)
{
    left    += l;
    right   -= r;
    top     += t;
    bottom  -= b;
}

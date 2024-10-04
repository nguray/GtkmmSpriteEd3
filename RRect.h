/*
 * RRect.h
 *
 *  Created on: 29 juin 2015
 *      Author: nguray
 */

#pragma once

class RRect {
public:
    RRect();
    RRect(int l,int t,int r,int b);
    virtual ~RRect();
    int     Width();
    int     Height();
    void    Normalize();
    void    Offset(int dx,int dy);
    bool    PtInRect(int x,int y);
    void    SetNULL();
    bool    IsNULL();
    void    Deflate(int l,int t,int r,int b);

    int left,top,right,bottom;

protected:
private:
};


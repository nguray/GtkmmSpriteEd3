/*
 * RPoint.h
 *
 *  Created on: 29 juin 2015
 *      Author: nguray
 */
#pragma once

class RPoint {
public:
	RPoint();
	virtual ~RPoint();
	RPoint(int x=0,int y=0);
	int		x;
	int		y;
};


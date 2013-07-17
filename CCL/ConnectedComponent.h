/*
 *  ConnectedComponent.h
 *  ConnectedComponentsLabelling
 *
 *  Created by Alessio Aboudan on 4/12/12.
 *  Copyright 2012 IASF BO. All rights reserved.
 *
 */

#ifndef __CONNECTED_COMPONENT_H__
#define __CONNECTED_COMPONENT_H__

#include <vector>
#include <string>
#include <climits>
#include <cfloat>

class ConnectedComponent {

public:
	
	ConnectedComponent();
	ConnectedComponent(const ConnectedComponent &c);
	
	void add(unsigned int col, unsigned int row, unsigned int plane);
	void update();
	
	unsigned int getCompSz() { return x.size(); };
	float getBaricentreCol() { return cx; };
	float getBaricentreRow() { return cy; };
	float getBaricentrePlane() { return cz; };
    float getBaricentreRadius() { return r; };
	
    void getBaricentre(float &col, float &row, float &plane) {
		col		= cx;
		row		= cy;
		plane	= cz;
	}
	
    void getBaricentre(float &col, float &row, float &plane, float &radius) {
        col	   = cx;
        row	   = cy;
        plane  = cz;
        radius = r;
    }
		
protected:
	
	// Points of the CC
	std::vector<float> x;
	std::vector<float> y;
	std::vector<float> z;
	
	// Baricentre
	float cx, cy, cz;
	
	// Bounds
	float minx, maxx, miny, maxy, minz, maxz;
	
    // Radius
    float r;
    
};

class ConnectedComponentProcessor {

public:
	
	ConnectedComponentProcessor(unsigned int colSz, unsigned int rowSz, unsigned int planeSz, int* labels);

	unsigned int getComponentSz() { return cc.size(); };
	ConnectedComponent getComponentAt(unsigned int i) { return cc.at(i); };
    
    unsigned int filterCompSz(unsigned int minSz, unsigned int maxSz);
    unsigned int filterRadius(float minRadius, float maxRadius);
    // unsigned int filterRoI(unsigned int minRow, unsigned int maxRow, unsigned int minCol, unsigned int maxCol);
    
	
	void print();
    void printForDS9(const std::string& fname, float constRadius = 5);
	
protected:
	
	std::vector<ConnectedComponent> cc;

};

#endif // __CONNECTED_COMPONENT_H__
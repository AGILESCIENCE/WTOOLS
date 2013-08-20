/*
 *  ConnectedComponent.cpp
 *  ConnectedComponentsLabelling
 *
 *  Created by Alessio Aboudan on 4/12/12.
 *  Copyright 2012 IASF BO. All rights reserved.
 *
 */

#include "ConnectedComponent.h"

#include <stdio.h>
#include <math.h>

using namespace std;

ConnectedComponent::ConnectedComponent() : cx(0), cy(0), cz(0), minx(FLT_MAX), maxx(0), miny(FLT_MAX), maxy(0), minz(FLT_MAX), maxz(0), r(0) {

}

ConnectedComponent::ConnectedComponent(const ConnectedComponent &c) {
	
	x  = c.x;
	y  = c.y;
	z  = c.z;
	
	cx = c.cx;
	cy = c.cy;
	cz = c.cz;
	
	minx = c.minx;
	maxx = c.maxx;
	
	miny = c.miny;
	maxy = c.maxy;
	
	minz = c.minz;
	maxz = c.maxz;
    
    r = c.r;

}

void ConnectedComponent::add(unsigned int col, unsigned int row, unsigned int plane) {

	x.push_back((float)col);
	y.push_back((float)row);
	z.push_back((float)plane);

}

void ConnectedComponent::update() {

	unsigned int n = x.size();
	
	cx		= 0;
	cy		= 0;
	cz		= 0;
	minx	= FLT_MAX;
	maxx	= 0;
	miny	= FLT_MAX ;
	maxy	= 0;
	minz	= FLT_MAX ;
	maxz	= 0;
	
    for (unsigned int i = 0; i < n; i++) {
		
		cx += x.at(i);
		cy += y.at(i);
		cz += z.at(i);
		
		if (x.at(i) < minx)
			minx = x.at(i);
		if (x.at(i) > maxx)
			maxx = x.at(i);
			
		if (y.at(i) < miny)
			miny = y.at(i);
		if (y.at(i) > maxy)
			maxy = y.at(i);
		
		if (z.at(i) < minz)
			minz = z.at(i);
		if (z.at(i) > maxz)
			maxz = z.at(i);
		
	}
	
	float d = (float)n;
	cx /= d;
	cy /= d;
	cz /= d;
    
    // Compute approximate radius from bounds
	float dx1 = fabs(cx - minx);
	float dx2 = fabs(cx - maxx);
	float dy1 = fabs(cy - miny);
	float dy2 = fabs(cy - maxy);
	
	r = dx1;
	r = dx2 > r ? dx2 : r;
	r = dy1 > r ? dy1 : r;
	r = dy2 > r ? dy2 : r;
	
}

ConnectedComponentProcessor::ConnectedComponentProcessor(unsigned int colSz, unsigned int rowSz, unsigned int planeSz, int *labels) {

	// Count connected components (max value of label index)
	int *pl = labels;
	unsigned int labelSz = 0;
	for (unsigned int p = 0; p < planeSz; p++)
		for (unsigned int r = 0; r < rowSz; r++)
			for (unsigned int c = 0; c < colSz; c++) {
				if (*pl > labelSz)
					labelSz = *pl;
				pl++;
			}
	
    // printf("labelSz %d\n", labelSz);
    
	// Allocate structures
	for (unsigned int i = 0; i < labelSz; i++)
		cc.push_back(ConnectedComponent());
	
	// Build connected components
	pl = labels;
	for (unsigned int p = 0; p < planeSz; p++)
		for (unsigned int r = 0; r < rowSz; r++)
			for (unsigned int c = 0; c < colSz; c++) {
				if (*pl)
					cc.at(*pl-1).add(c, r, p);
				pl++;
			}
	
	// Compute baricentres and bounds
	for (unsigned int i = 0; i < labelSz; i++)
		cc.at(i).update();
	
}

unsigned int ConnectedComponentProcessor::filterCompSz(unsigned int minSz, unsigned int maxSz) {
    
    std::vector<ConnectedComponent> ccNew;
    
    for (unsigned int i = 0; i < cc.size(); i++) {
    
        unsigned int curSz = cc.at(i).getCompSz();
        
        if (curSz >= minSz && curSz <= maxSz)
            ccNew.push_back(cc.at(i));
        
    }
    
    cc = ccNew;
    
    return cc.size();
    
}

unsigned int ConnectedComponentProcessor::filterRadius(float minRadius, float maxRadius) {

    std::vector<ConnectedComponent> ccNew;
    
    for (unsigned int i = 0; i < cc.size(); i++) {
        
        float curRadius = cc.at(i).getBaricentreRadius();
        
        if (curRadius >= minRadius && curRadius <= maxRadius)
            ccNew.push_back(cc.at(i));
        
    }
    
    cc = ccNew;
    
    return cc.size();

}

/*
unsigned int ConnectedComponentProcessor::filterRoI(unsigned int minRow, unsigned int maxRow, unsigned int minCol, unsigned int maxCol) {
    
    std::vector<unsigned int> removeId;
    
    for (unsigned int i = 0; i < cc.size(); i++) {
        
        float curRadius = cc.at(i).getBaricentreRadius();
        
        if (curRadius < minRadius || curRadius > maxRadius)
            removeId.push_back(i);
        
    }
    
    for (unsigned int i = 0; i < removeId.size(); i++)
        cc.erase(cc.begin() + removeId.at(i));
    
    return cc.size();

}
*/

void ConnectedComponentProcessor::print() {

	printf("Connected Components\n");
	
	if (cc.size()) {
		
		printf("\tnumber %u\n", cc.size());
        
        printf("\tcomp.  column   row      plane    radius   size\n");
		
		for (unsigned int i = 0; i < cc.size(); i++) {
			float c, r, p, radius;
			cc.at(i).getBaricentre(c, r, p, radius);
			printf("\t[%04d] %8.2f %8.2f %8.2f %8.2f %5u\n", i, c, r, p, radius, cc.at(i).getCompSz());
		}
		
		
	} else
		printf("\tempty\n");	

}

void ConnectedComponentProcessor::printForDS9(const string& fname, float constRadius) {
  
    FILE *pFile = fopen(fname.c_str(), "w");
    
    if (pFile) {
        
        // Write header
        fprintf(pFile, "%s\n", "global color=green dashlist=8 3 width=1 font=\"helvetica 10 normal\" select=1 highlite=1 dash=0 fixed=0 edit=1 move=1 delete=1 include=1 source=1");
        fprintf(pFile, "%s\n", "image");
        
        // Write points
        if (constRadius <= 0)
            for (unsigned int i = 0; i < cc.size(); i++) {
                float c, r, p, radius;
                cc.at(i).getBaricentre(c, r, p, radius);
                fprintf(pFile, "circle(%7.2f,%7.2f,%7.2f)\n", c+1, r+1, radius); // DS9 raster coordinates are 1-based
            }
        else
            for (unsigned int i = 0; i < cc.size(); i++) {
                float c, r, p;
                cc.at(i).getBaricentre(c, r, p);
                fprintf(pFile, "circle(%7.2f,%7.2f,%7.2f)\n", c+1, r+1, constRadius); // DS9 raster coordinates are 1-based
            }
    
    }
    
}
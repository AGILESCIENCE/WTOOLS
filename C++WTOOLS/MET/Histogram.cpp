/*
 *  Histogram.cpp
 *  TestHistogram
 *
 *  Created by Alessio Aboudan on 3/12/12.
 *  Copyright 2012 IASF BO. All rights reserved.
 *
 */

#include "Histogram.h"

#include <stdio.h>
#include <limits.h>

#include <gsl/gsl_math.h>

using namespace std;

Histogram::Histogram(double step) : s(step), x(0) {

}

Histogram::Histogram(const gsl_vector *v, double step) : s(step), x(0) {
	update(v);
}

Histogram::Histogram(const gsl_matrix *m, double step) : s(step), x(0) {
	update(m);
}

Histogram::Histogram(const gsl_vector *v, unsigned int size) : s(0), x(0) {
	create(v, size);
}

Histogram::Histogram(const gsl_matrix *m, unsigned int size) : s(0), x(0) {
	create(m, size);
}

int Histogram::setBinStep(double step) {
	if(s == 0) {
		s = step;
		return 0;
	} else
		return -1;
}

double Histogram::getMaxBin() {
	return x + s*(y.size());
}

int Histogram::getBin(gsl_vector *b) {
	if(y.empty())
		return -1;
	if(b->size < y.size())
		return -1;
	double *p = b->data;
	for(unsigned int i = 0; i < y.size(); i++)
		*p++ = x + i*s;
	return 0;
}

unsigned int Histogram::getMinCount() {
	unsigned int m = UINT_MAX;
	deque<unsigned int>::iterator it;
	it = y.begin();
	while(it != y.end()) {
		if(*it < m)
			m = *it;
		it++;
	}
	return m;
}

unsigned int Histogram::getMaxCount() {
	unsigned int m = 0;
	deque<unsigned int>::iterator it;
	it = y.begin();
	while(it != y.end()) {
		if(*it > m)
			m = *it;
		it++;
	}
	return m;
}

int Histogram::getCount(gsl_vector *c) {
	if(y.empty())
		return -1;
	if(c->size < y.size())
		return -1;
	double *p = c->data;
	deque<unsigned int>::iterator it = y.begin();
	while(it != y.end())
		*p++ = *it++;
	return 0;
}

void Histogram::create(const gsl_vector *v, double step) {
    
    // Set step
	s = step;
	
	// Fill the histogram
	update(v);
	
}


void Histogram::create(const gsl_vector *v, unsigned int size) {

	// Get step from size
	double minVal, maxVal;
	gsl_vector_minmax(v, &minVal, &maxVal);
	double delta = maxVal - minVal;
	s = delta/size;
	
	// Fill the histogram
	update(v);
	
}

void Histogram::create(const gsl_matrix *m, double step) {
    
	// Set step
	s = step;
	
	// Fill the histogram
	update(m);
	
}

void Histogram::create(const gsl_matrix *m, unsigned int size) {

	// Get step from size
	double minVal, maxVal;
	gsl_matrix_minmax(m, &minVal, &maxVal);
	double delta = maxVal - minVal;
	s = delta/size;
	
	// Fill the histogram
	update(m);
	
}

int Histogram::update(double d) {
	if(s != 0) {
		runUpdate(d);
		return 0;
	} else
		return -1;
}

int Histogram::update(const gsl_vector *v) {
	if(s != 0) {

		double *p = v->data;
		for(unsigned int i = 0; i < v->size; i++)
			runUpdate(*p++);

		return 0;
	} else
		return -1;
}

int Histogram::update(const gsl_matrix *m) {
	if(s != 0) {

		double *p = m->data;
		for(unsigned int i = 0; i < m->size1; i++) {
			for(unsigned int j = 0; j < m->size2; j++)
				runUpdate(*p++);
			p += m->tda - m->size2;
		}

		return 0;

	} else
		return -1;
}

int Histogram::getPDF(gsl_vector *p) {
	
	if (y.size() == p->size) {
		
		// Sum counts
		double sum = 0;
		for(unsigned int i = 0; i < p->size; i++)
			sum += (double)y[i];
		
		// Compute total area
		sum *= s;
		
		// Normalize
		double *d = p->data;
		for(unsigned int i = 0; i < p->size; i++) {
			*d = y[i]/sum;
			d++;
		}
		
		return 0;
		
	} else
		return -1;
	
}

double Histogram::getMET() {

	// Compute PDF and bins 
	gsl_vector *p0 = gsl_vector_alloc(y.size());
	getPDF(p0);
	gsl_vector *b0 = gsl_vector_alloc(y.size());
	getBin(b0);
	
	
	// Remove zero probs
	unsigned int size = countNonZeros(p0);
	gsl_vector *p = gsl_vector_alloc(size);
	gsl_vector *b = gsl_vector_alloc(size);
	removeZeros(p0, p, b0, b);
	
	gsl_vector_free(b0);
	gsl_vector_free(p0);
	
	// Compute inter-class entropy
	gsl_vector *h = gsl_vector_alloc(size);
	
	// Init first class
	double p1 = p->data[0];
	
	// Init second class
	double p2 = 0;
	for(unsigned int i = 1; i < size; i++) {
		p2 += p->data[i];
	}
	
	// Inter-class entropy
	static const double il2 = 1/log(2);
	double *ph = h->data;
	for(unsigned int i = 1; i < size; i++) {
		
		double *pp = p->data;
		
		double h1 = 0;
		for (unsigned int j = 0; j < i; j++) {
			double tmp = *pp++/p1;
			tmp *= log(tmp);
			tmp *=il2;
			h1 -= tmp;
		}
		
		double h2 = 0;
		for (unsigned int j = i; j < size; j++) {
			double tmp = *pp++/p2;
			tmp *= log(tmp);
			tmp *=il2;
			h2 -= tmp;
		}
		
		*ph++ = h1 + h2;
		
		// Update classes
		p1 += p->data[i];
		p2 -= p->data[i];
	
	}
	
	// Get the max value of inter-class entropy
	unsigned int id = gsl_vector_max_index(h);
	double th = b->data[id];
	
	gsl_vector_free(h);
	gsl_vector_free(b);
	gsl_vector_free(p);
	
	return th;
}

void Histogram::print() {
	printf("Histogram state\n");
	printf("\ts = %10.4f x = %10.4f\n", s, x);
	if(y.empty())
		printf("\tempty\n");
	else
		for(unsigned int i = 0; i < y.size(); i++)
			printf("\t%3d %10.4f %10d\n", i, x + s*i, y.at(i));
	printf("\n");
}

inline void Histogram::runUpdate(double d) {

	double off    	= d - x;
	double floorOff = floor(off/s);

	if(y.empty()) {							// Init histogram
		x = floorOff*s;
		y.push_back(1);
	} else 									// Update histogram
		if(floorOff < 0) {					// Push front and change min bin value
			x += floorOff*s;
			unsigned int n = (unsigned int)(-floorOff) - 1;
			for(unsigned int i = 0; i < n; i++)
				y.push_front(0);
			y.push_front(1);
		} else if(floorOff >= y.size()){	// Push back
			unsigned int n = (unsigned int)floorOff - y.size();
			for(unsigned int i = 0; i < n; i++)
				y.push_back(0);
			y.push_back(1);
		} else {							// Increase existing
			unsigned int n = (unsigned int)floorOff;
			y.at(n)++;
		}

}

unsigned int countNonZeros(const gsl_vector *v) {
	unsigned int n = 0;
	double *p = v->data;
	for (unsigned int i = 0; i < v->size; i++)
		if(*p++ != 0)
			n++;
	return n;
}

void removeZeros(const gsl_vector *v, gsl_vector *u) {
	double *pv = v->data;
	double *pu = u->data;
	for (unsigned int i = 0; i < v->size; i++) {
		if(*pv != 0) {
			*pu = *pv;
			pu++;
		}
		pv++;
	}
}

void removeZeros(const gsl_vector *v, gsl_vector *u, const gsl_vector *v1, gsl_vector *u1) {
	double *pv  = v->data;
	double *pu  = u->data;
	double *pv1 = v1->data;
	double *pu1 = u1->data;
	for (unsigned int i = 0; i < v->size; i++) {
		if(*pv != 0) {
			*pu  = *pv;
			*pu1 = *pv1;
			pu++;
			pu1++;
		}
		pv++;
		pv1++;
	}
}



/*
 *  Histogram.h
 *  TestHistogram
 *
 *  Created by Alessio Aboudan on 3/12/12.
 *  Copyright 2012 IASF BO. All rights reserved.
 *
 */

#ifndef __HISTOGRAM_H__
#define __HISTOGRAM_H__

#include <deque>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

unsigned int countNonZeros(const gsl_vector *v);
void removeZeros(const gsl_vector *v, gsl_vector *u);
void removeZeros(const gsl_vector *v, gsl_vector *u, const gsl_vector *v1, gsl_vector *u1);

class Histogram {
public:

	Histogram(double step = 0);
	Histogram(const gsl_vector *v, double step);
	Histogram(const gsl_matrix *m, double step);
	Histogram(const gsl_vector *v, unsigned int size);
	Histogram(const gsl_matrix *m, unsigned int size);

	double getBinStep() { return s; };
	int setBinStep(double step);

	double getMinBin() { return x; };
	double getMaxBin();
	int getBin(gsl_vector *b);

	unsigned int getMinCount();
	unsigned int getMaxCount();
	int getCount(gsl_vector *c);

	unsigned int getSize() { return y.size(); };

	// Create from scratch and compute the step to have size bins
    void create(const gsl_vector *v, double step);
	void create(const gsl_matrix *m, double step);
	void create(const gsl_vector *v, unsigned int size);
	void create(const gsl_matrix *m, unsigned int size);
	
	// Dynamically update the bins and the histogram
	int update(double d);
	int update(const gsl_vector *v);
	int update(const gsl_matrix *m);

	// Get probability density function
	int getPDF(gsl_vector *p);

	// Get maximum entropy threshold
	double getMET();

	// Print
	void print();
	// void save(FILE *p);
	// void load(FILE *p);

protected:

	double s;
	double x;
	std::deque<unsigned int> y;

	void runUpdate(double d);

};

#endif // __HISTOGRAM_H__

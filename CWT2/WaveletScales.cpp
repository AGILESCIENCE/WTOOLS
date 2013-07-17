/*
 * Scales.cpp
 *
 *  Created on: Jan 16, 2012
 *      Author: alessio
 */

#include "WaveletScales.h"

#include <math.h>
#include <stdio.h>
#include <gsl/gsl_math.h>

WaveletScales::WaveletScales(unsigned int size) : n(size), s(0) {
	if(n > 0)
        s = new double[n];
}

WaveletScales::~WaveletScales() {
	if(s)
        delete []s;
}

double WaveletScales::operator[](unsigned int i) {
	if(s) {
		if(i < n)
            return s[i];
		else
			return GSL_NAN;
	} else
		return GSL_NAN;
}

double WaveletScales::at(unsigned int i) {
    if(s) {
		if(i < n)
            return s[i];
		else
			return GSL_NAN;
	} else
		return GSL_NAN;
}

LinearWaveletScales::LinearWaveletScales(unsigned int size, double minVal, double maxVal) : WaveletScales(size) {

	double step = (maxVal - minVal)/(size - 1);

	for(unsigned int i = 0; i < size; i++)
        s[i] = minVal + i*step;

}

void LinearWaveletScales::printInfo() {
	printf("Linear wavelet scales\n");
	printf("\tsize %d\n", n);
	printf("\tvals min %10.4e max %10.4e\n", s[0], s[n-1]);
	printf("\tscales:\n");
	for (int i = 0; i < n; i++) {
		printf("\t[%03d] %8.3ef\n", i+1, s[i]);
	}
}


ExpWaveletScales::ExpWaveletScales(unsigned int size, double minVal, double maxVal, double base) : WaveletScales(size), b(base) {

	minVal = log(minVal)/log(base);
	maxVal = log(maxVal)/log(base);
	
	double step = (maxVal - minVal)/(size-1);
	
	for(unsigned int i = 0; i < size; i++)
        s[i] = pow(base, minVal + i*step);
		
}

void ExpWaveletScales::printInfo() {
	printf("Exponential wavelet scales\n");
	printf("\tsize %d\n", n);
	printf("\tvals min %10.4e max %10.4e\n", s[0], s[n-1]);
	printf("\tbase %f\n", b);
	printf("\tscales:\n");
	for (int i = 0; i < n; i++) {
		printf("\t[%03d] %8.3e\n", i+1, s[i]);
	}
}







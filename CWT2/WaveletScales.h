/*
 * Scales.h
 *
 *  Created on: Jan 16, 2012
 *      Author: alessio
 */

#ifndef __SCALES_H__
#define __SCALES_H__

class WaveletScales {
public:

	WaveletScales(unsigned int size);
	virtual ~WaveletScales();

	unsigned int getSize() { return n; };
	double operator[](unsigned int i);
    double at(unsigned int i);

	virtual void printInfo() = 0;
	
protected:

	unsigned int n;
	double* s;

};

class LinearWaveletScales : public WaveletScales {
public:

	LinearWaveletScales(unsigned int size, double minVal, double maxVal);
	
	void printInfo();

};


class ExpWaveletScales : public WaveletScales {
public:

	ExpWaveletScales(unsigned int size, double minVal, double maxVal, double base = 2);
	
	void printInfo();
	
protected:
	
	double b;

};

#endif // __SCALES_H__

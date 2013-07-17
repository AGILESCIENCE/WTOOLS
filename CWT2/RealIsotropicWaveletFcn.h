/*
 * WaveletFcn.h
 *
 *  Created on: Jan 16, 2012
 *      Author: alessio
 */

#ifndef __REAL_WAVELET_FCN_H__
#define __REAL_WAVELET_FCN_H__

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

class RealIsotropicWaveletFcn2 {
public:

	// Function value
	virtual void fcn(double s, const gsl_matrix *x, const gsl_matrix *y, gsl_matrix *f) = 0;

	// Function FFT value
	virtual void fft(double s, const gsl_matrix *wx, const gsl_matrix *wy, gsl_matrix *fhat) = 0;
    virtual void fft(double s, const gsl_vector *wx, const gsl_vector *wy, gsl_matrix *fhat) = 0;

	// Wavelet admissibility constant
	virtual double constant() = 0;
	
	// Print informations
	virtual void printInfo() = 0;

};

class LaplacianOfGaussianWaveletFcn2 : public RealIsotropicWaveletFcn2 {
public:

	LaplacianOfGaussianWaveletFcn2();

	void fcn(double s, const gsl_matrix *x, const gsl_matrix *y, gsl_matrix *f);

	void fft(double s, const gsl_matrix *wx, const gsl_matrix *wy, gsl_matrix *fhat);
    void fft(double s, const gsl_vector *wx, const gsl_vector *wy, gsl_matrix *fhat);

	double constant();
	
	void printInfo();

protected:

	double sx;
	double sy;

	double kx;
	double ky;

	void initConstants();

	double computeFcn(double s, double x, double y);
	double computeFFT(double s, double wx, double wy);

};



#endif // __REAL_WAVELET_FCN_H__

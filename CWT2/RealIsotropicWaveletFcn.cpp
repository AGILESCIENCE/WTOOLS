/*
 * WaveletFcn.cpp
 *
 *  Created on: Jan 16, 2012
 *      Author: alessio
 */

#include "RealIsotropicWaveletFcn.h"

#include <stdio.h>
#include <math.h>
#include <gsl/gsl_math.h>

LaplacianOfGaussianWaveletFcn2::LaplacianOfGaussianWaveletFcn2() : sx(1), sy(1) {
	initConstants();
}

void LaplacianOfGaussianWaveletFcn2::fcn(double s, const gsl_matrix *x, const gsl_matrix *y, gsl_matrix *f) {
	double *px = x->data;
	double *py = y->data;
	double *pf = f->data;
	for(unsigned int i = 0; i < x->size1; i++) {
		for(unsigned int j = 0; j < x->size2; j++) {

			// Get function
			*pf++ = computeFcn(s, *px++, *py++);

			// Update pointers
			px += x->tda - x->size2;
			py += y->tda - y->size2;
			pf += f->tda - f->size2;

		}
	}
}

void LaplacianOfGaussianWaveletFcn2::fft(double s, const gsl_matrix *wx, const gsl_matrix *wy, gsl_matrix *fhat) {
	double *pwx = wx->data;
	double *pwy = wy->data;
	double *pfhat = fhat->data;
	for(unsigned int i = 0; i < wx->size1; i++) {
		for(unsigned int j = 0; j < wy->size2; j++) {

			// Get FFT
			*pfhat++ = computeFFT(s, *pwx++, *pwy++);

			// Update pointers
			pwx 	+= wx->tda - wx->size2;
			pwy 	+= wy->tda - wy->size2;
			pfhat 	+= fhat->tda - fhat->size2;

		}
	}
}

void LaplacianOfGaussianWaveletFcn2::fft(double s, const gsl_vector *wx, const gsl_vector *wy, gsl_matrix *fhat) {
	double *pwx = wx->data;
	double *pfhat = fhat->data;
	for(unsigned int i = 0; i < wx->size; i++) {
        
        double *pwy = wy->data;
		
        for(unsigned int j = 0; j < wy->size; j++) {
            
			// Get FFT
			*pfhat++ = computeFFT(s, *pwx, *pwy++);
            
			// Update pointers
			pfhat += fhat->tda - fhat->size2;
            
		}
        
        pwx++;
        
	}
}


double LaplacianOfGaussianWaveletFcn2::constant() {
	return 1;
}

void LaplacianOfGaussianWaveletFcn2::printInfo() {
	printf("Laplacian of Gaussian (LoG) wavelet\n");
	printf("\tsigma X %10.3f Y %10.3f\n", sx, sy);
}

void LaplacianOfGaussianWaveletFcn2::initConstants() {
	kx = -sx*sx/2;
	ky = -sy*sy/2;
}

inline double LaplacianOfGaussianWaveletFcn2::computeFcn(double s, double x, double y) {
	return GSL_NAN;
}

inline double LaplacianOfGaussianWaveletFcn2::computeFFT(double s, double wx, double wy) {
	wx *= s;
	wy *= s;
	double wx2 = wx*wx;
	double wy2 = wy*wy;
	double arg1 = kx*wx2;
	double arg2 = ky*wy2;
	arg1 += arg2;
	arg2 = exp(arg1);
	wx2 += wy2;
	wx2 *= arg2;
	wx2 *= s;
	return wx2;
}

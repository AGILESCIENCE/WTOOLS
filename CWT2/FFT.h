/*
 *  FFT.h
 *  ContinuousWaveletTransform2
 *
 *	2D FFT of real functions
 *
 *  Created by Alessio Aboudan on 1/13/12.
 *  Copyright 2012 IASF BO. All rights reserved.
 *
 */

#ifndef __FFT_H__
#define __FFT_H__

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_complex.h>

void realComplexMatrixMulElements(const gsl_matrix *a, gsl_matrix_complex *b);
void realComplexMatrixMulElements(const gsl_matrix *a, const gsl_matrix_complex *b, gsl_matrix_complex *c);

void copyRealPart(const gsl_matrix *a, gsl_matrix_complex *b);
void copyRealPart(const gsl_matrix_complex *a, gsl_matrix *b);
void copyImagPart(const gsl_matrix *a, gsl_matrix_complex *b);
void copyImagPart(const gsl_matrix_complex *a, gsl_matrix *b);

void fft2(gsl_matrix_complex *m);
void fft2(const gsl_matrix_complex *src, gsl_matrix_complex *dst);
void ifft2(gsl_matrix_complex *m);
void ifft2(const gsl_matrix_complex *src, gsl_matrix_complex *dst);

// Old
void rfft2(const gsl_matrix *src, gsl_matrix_complex *dst);
void irfft2(gsl_matrix_complex *m);
void irfft2(const gsl_matrix_complex *src, gsl_matrix_complex *dst);

#endif // __RFFT_H__
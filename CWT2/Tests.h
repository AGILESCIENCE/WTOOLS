/*
 *  tests.h
 *  ContinuousWaveletTransform2
 *
 *  Created by Alessio Aboudan on 1/18/12.
 *  Copyright 2012 IASF BO. All rights reserved.
 *
 */

#ifndef __TESTS_H__
#define __TESTS_H__

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_complex.h>

// Helper functions
void printMatrix(const gsl_matrix *m);
void printMatrix(const gsl_matrix_complex *m);

// GSL functions tests
void testFFT();
void testComplexRealMatrixMulElements();

// Classes tests
void testMatrixIO();
void testMatrixFITSIO();
void testCTSLoadAsMTX();
void testCTSLoadAsFITS();
void testRealWaveletFcn();
void testWaveletScales();

// Math
void testMath();

// Test CWT
void testCWT();

#endif // __TESTS_H__
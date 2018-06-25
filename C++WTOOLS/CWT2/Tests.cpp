/*
 *  tests.cpp
 *  ContinuousWaveletTransform2
 *
 *  Created by Alessio Aboudan on 1/18/12.
 *  Copyright 2012 **. All rights reserved.
 *
 */

#include "Tests.h"

#include <gsl/gsl_math.h>

#include "FFT.h"
#include "MatrixIO.h"
#include "RealIsotropicWaveletFcn.h"
#include "WaveletScales.h"
#include "RICWT.h"

using namespace std;

//-----------------------------------------------------------------------------
//
// Print matrix on stdout
//
//-----------------------------------------------------------------------------

void printMatrix(const gsl_matrix *m) {
	
	// printf("Size r %4d c %4d pad %d\n", m->size1, m->size2, m->tda);
	
	double *p = m->data;
	for(unsigned int i = 0; i < m->size1; i++) {
		for(unsigned int j = 0; j < m->size2; j++)
			printf("%12.4f ", *p++);
		printf("\n");
		p += m->tda - m->size2;
	}
	
}

void printMatrix(const gsl_matrix_complex *m) {
	
	// printf("Size r %4d c %4d pad %d\n", m->size1, m->size2, m->tda);
	
	double *p = m->data;
	for(unsigned int i = 0; i < m->size1; i++) {
		for(unsigned int j = 0; j < m->size2; j++) {
			printf("[%12.4f ", *p++);
			printf("%12.4f] ", *p++);
		}
		printf("\n");
		p += m->tda - m->size2;
	}
	
}

//-----------------------------------------------------------------------------
//
// Forward and inverse real FFT 2D test
//
//-----------------------------------------------------------------------------


void test_fft() {

	// Create a real matrix
	gsl_matrix *m = gsl_matrix_alloc(2, 5);
	gsl_matrix_set_zero(m);
	
	// Fill the matrix
	for(unsigned int i = 0; i < m->size1; i++) {
		for(unsigned int j = 0; j < m->size2; j++) {
			double v = (double)(10*i + j);
			gsl_matrix_set(m, i, j, v);
		}
	}
	
	printf("Real matrix\n");
	printMatrix(m);
	
	// FFT 2D
	gsl_matrix_complex *mHat = gsl_matrix_complex_alloc(2, 5);
	gsl_matrix_complex_set_zero(mHat);
	
	rfft2(m, mHat);
	
	printf("FFT\n");
	printMatrix(mHat);
	
	// Inverse FFT 2D
	gsl_matrix_complex *n = gsl_matrix_complex_alloc(2, 5);
	gsl_matrix_complex_set_zero(n);
	
	irfft2(mHat, n);
	
	printf("Inverse FFT\n");
	printMatrix(n);
	
	// Free memory
	gsl_matrix_complex_free(n);
	gsl_matrix_complex_free(mHat);
	gsl_matrix_free(m);

}

//-----------------------------------------------------------------------------
//
// Matrix I/O test
//
//-----------------------------------------------------------------------------

void testMatrixIO() {
	
	// Create a matrix
	
	size_t r = 100;
	size_t c = 100;
	size_t p = 10;
	
	gsl_matrix *a = gsl_matrix_alloc(r, c);
	for(size_t i = 0; i < r; i++)
		for(size_t j = 0; j < c; j++)
			gsl_matrix_set(a, i, j, i*1000 + j);
	
	// Save it
	{
		
		printf("Save file : %4d %4d %4d\n", r, c, p);
		
		MatrixFileIO outFile("test.mtx", r, c, p, MatrixFileIO::Double);
		for(int k = 0; k < p; k++)
			if(!outFile.write(a))
				printf("\tWrite error at p %4d\n", k);
		
	}
	
	// Reload and compare
	{
		
		MatrixFileIO inFile("test.mtx");
		
		printf("Load file : %4d %4d %4d\n", inFile.getRowSz(), inFile.getColSz(), inFile.getPlaneSz());
		
		gsl_matrix *b = gsl_matrix_alloc(inFile.getRowSz(), inFile.getColSz());
		
		bool hasErrors = false;
		for(int k = 0; k < inFile.getPlaneSz(); k++) {
		
			if(!inFile.read(b))
				printf("\tRead error at p %4d\n", k);
			
			double *pa = a->data;
			double *pb = b->data;
			
			for(size_t i = 0; i < r; i++)
				for(size_t j = 0; j < c; j++)
					if(*pa++ != *pb++) {
						hasErrors = true;
						printf("\tData error at %4d %4d %4d\n", i, j, k);
					}
			
		}
		
		if(!hasErrors) 
			printf("Check ok\n");
		
		gsl_matrix_free(b);
		
	}
	
	gsl_matrix_free(a);
	 
}

void testCTSLoadAsMTX() {
	
	// static const char *fname = "/Users/alessio/Works/AGILE/Matlab/Ver1.5/Wavelet/ContinuousTransform2D/CYGX3_BIN005_POINTING.cts.mtx";
	static const char *fname = "/Users/alessio/Works/AGILE/Matlab/Ver1.5/Wavelet/ContinuousTransform2DCppTest/Test2/data_d.mtx";	
	
	MatrixFileIO inFile(fname);
	
	printf("Size %5d %5d %5d\n", inFile.getRowSz(), inFile.getColSz(), inFile.getPlaneSz());
	
	gsl_matrix *a = gsl_matrix_alloc(inFile.getRowSz(), inFile.getColSz());
	
	inFile.read(a);
	
	double *pa = a->data;
	for (size_t i = 0; i < 100; i++)
		printf("%10.4f (%10.4f)\n", a->data[i], *pa++);
	
	gsl_matrix_free(a);
	
}

void testCTSLoadAsFITS() {
	
	static const char *fitsFname = "/Users/alessio/Works/AGILE/Data/CYGX3_BIN005/CYGX3_BIN005_POINTING.cts";
	// static const char *fitsFname = "/Users/alessio/Works/AGILE/Matlab/Ver1.5/Wavelet/ContinuousTransform2DCppTest/Test2/data_f.mtx";
	
	MatrixFITSIO fitsInFile(fitsFname);
	// MatrixFileIO fitsInFile(fitsFname);
	
	gsl_matrix *a = gsl_matrix_alloc(fitsInFile.getRowSz(), fitsInFile.getColSz());
	if(!fitsInFile.read(a))
		printf("ERROR : read a matrix\n");
	
	static const char *mtxFname = "/Users/alessio/Works/AGILE/Matlab/Ver1.5/Wavelet/ContinuousTransform2DCppTest/Test2/data_f.mtx";
		
	MatrixFileIO mtxInFile(mtxFname);
		
	gsl_matrix *b = gsl_matrix_alloc(mtxInFile.getRowSz(), mtxInFile.getColSz());
	if(!mtxInFile.read(b))
		printf("ERROR : read b matrix\n");
	
	printf("Sample a matrix ----------------------------\n");
	
	double *pa = a->data;
	for (size_t i = 0; i < 25; i++)
		printf("a %10.4f (%10.4f)\n", a->data[i], *pa++);
		
	printf("Sample b matrix ----------------------------\n");
	
	double *pb = b->data;
	for (size_t i = 0; i < 25; i++)
		printf("b %10.4f (%10.4f)\n", b->data[i], *pb++);
	
	printf("Error --------------------------------------\n");
	
	pa = a->data;
	pb = b->data;
	
	int errCount = 0;
	for(size_t i = 0; i < a->size1; i++) {
		for(size_t j = 0; j < a->size2; j++) {
			
			if(*pa != *pb) {
				printf("\tData error %6.2f != %6.2f @ %4d %4d\n", *pa, *pb, i, j);
				errCount++;
			}
			
			pa++;
			pb++;
			
			if(errCount > 100) {
				printf("break\n");
				break;
			}

		}
		
		if(errCount > 100)
			break;
		
	}
	
	gsl_matrix_free(b);
	gsl_matrix_free(a);
	
}


//-----------------------------------------------------------------------------
//
// LoG wavelet test
//
//-----------------------------------------------------------------------------

void testRealWaveletFcn() {

	gsl_matrix *wx = gsl_matrix_alloc(2,2);
	
	gsl_matrix_set(wx, 0, 0, 0);
	gsl_matrix_set(wx, 0, 1, -M_PI);
	gsl_matrix_set(wx, 1, 0, 0);
	gsl_matrix_set(wx, 1, 1, -M_PI);
	
	printf("wx\n");
	printMatrix(wx);
	
	gsl_matrix *wy = gsl_matrix_alloc(2,2);
	
	gsl_matrix_set(wy, 0, 0, 0);
	gsl_matrix_set(wy, 0, 1, 0);
	gsl_matrix_set(wy, 1, 0, -M_PI);
	gsl_matrix_set(wy, 1, 1, -M_PI);
	
	printf("wy\n");
	printMatrix(wy);
	
	gsl_matrix *csiHat = gsl_matrix_alloc(2,2);
	gsl_matrix_set_zero(csiHat);
	
	LaplacianOfGaussianWaveletFcn2 csi;
	
	csi.fft(1, wx, wy, csiHat);
	
	printf("FFT\n");
	printMatrix(csiHat);
	
	gsl_matrix_free(csiHat);
	gsl_matrix_free(wy);
	gsl_matrix_free(wx);
	
}

//-----------------------------------------------------------------------------
//
// Scales test
//
//-----------------------------------------------------------------------------


void testWaveletScales() {

	printf("Linear scales\n");
	
	LinearWaveletScales	s0(10, 0.5, 10);
	
	for(unsigned int i = 0; i < s0.getSize(); i++)
		printf("\t%3d %12.6f\n", i, s0[i]);
	
	printf("Diadic scales\n");
	
	ExpWaveletScales s1(46, 0.031250, 1024);
	
	for(unsigned int i = 0; i < s1.getSize(); i++)
		printf("\t%3d %12.6f\n", i, s1[i]);

	
}

//-----------------------------------------------------------------------------
//
// Math functions test
//
//-----------------------------------------------------------------------------

void testMath() {

	double x = pow(2, 1.5);
	
	printf("%10.4f", x);
	
	x = log(122.44);

	printf("%10.4f", x);
	
}

//-----------------------------------------------------------------------------
//
// Math functions test
//
//-----------------------------------------------------------------------------

void testComplexRealMatrixMulElements() {

	// Complex matrix
	
	// Create a real matrix
	gsl_matrix *m = gsl_matrix_alloc(2, 5);
	gsl_matrix_set_zero(m);
	
	// Fill the matrix
	for(unsigned int i = 0; i < m->size1; i++) {
		for(unsigned int j = 0; j < m->size2; j++) {
			double v = (double)(10*i + j);
			gsl_matrix_set(m, i, j, v);
		}
	}
	
	printf("Real matrix\n");
	printMatrix(m);
	
	// FFT 2D
	gsl_matrix_complex *mHat = gsl_matrix_complex_alloc(2, 5);
	gsl_matrix_complex_set_zero(mHat);
	
	rfft2(m, mHat);
	
	printf("FFT\n");
	printMatrix(mHat);
	
	gsl_matrix_complex *n = gsl_matrix_complex_alloc(2, 5);
	gsl_matrix_complex_set_zero(n);
	
	// Multiply
	realComplexMatrixMulElements(m, mHat, n);
	
	printf("Mul. result\n");
	printMatrix(n);
	
	// Multiply
	realComplexMatrixMulElements(m, mHat);
	
	printf("Mul. in-place result\n");
	printMatrix(mHat);
	
	gsl_matrix_complex_free(n);
	gsl_matrix_complex_free(mHat);
	gsl_matrix_free(m);


}

//-----------------------------------------------------------------------------
//
// Math functions test
//
//-----------------------------------------------------------------------------

/*
void testCWT() {

	static const char *inFname = "/Users/alessio/Works/AGILE/Matlab/Ver1.5/Wavelet/ContinuousTransform2DCppImplementation/data.mtx";
	static const char *outFname = "/Users/alessio/Works/AGILE/Matlab/Ver1.5/Wavelet/ContinuousTransform2DCppImplementation/cwt_cpp.mtx";

	// Wavelet function
	LaplacianOfGaussianWaveletFcn2 csi;
	
	// Scales
	ExpWaveletScales scales(4, 0.25, 2);
	
	// Input/Output
	MatrixFileIO inFile(inFname);
	MatrixFileIO outFile(outFname, inFile.getRowSz(), inFile.getColSz(), scales.getSize());
	
	// Computes CWT
	ricwt2(csi, scales, inFile, outFile);
	
}
*/

void testCWT() {
	
	static const char *inFname = "/Users/alessio/Works/AGILE/Matlab/Ver1.5/Wavelet/ContinuousTransform2D/CYGX3_BIN005_POINTING.cts.mtx";
	static const char *outFname = "/Users/alessio/Works/AGILE/Matlab/Ver1.5/Wavelet/ContinuousTransform2D/CYGX3_BIN005_POINTING.cwt.mtx";
	
	// Wavelet function
	LaplacianOfGaussianWaveletFcn2 csi;
	
	// Scales
	ExpWaveletScales scales(46, 0.0312, 1024);
	
	// Input/Output
	MatrixFileIO inFile(inFname);
	MatrixFileIO outFile(outFname, inFile.getRowSz(), inFile.getColSz(), scales.getSize(), MatrixFileIO::Double);
	
	// Computes CWT
	ricwt2(csi, scales, inFile, outFile);
	
}





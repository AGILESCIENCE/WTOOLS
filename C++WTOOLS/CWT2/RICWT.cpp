/*
 *  RCWT.cpp
 *  ContinuousWaveletTransform2
 *
 *  Created by Alessio Aboudan on 1/19/12.
 *  Copyright 2012 **. All rights reserved.
 *
 */

#include "RICWT.h"

#include "FFT.h"
#include "Pulse.h"

#include "Tests.h"

using namespace std;

// #define USE_PULSATION_PLANE

#ifdef USE_PULSATION_PLANE

void ricwt2(RealIsotropicWaveletFcn2 &csi,  WaveletScales &scales, MatrixIO &data, MatrixIO &dataWT) {

	// Check for valid data
	if(!data.isValid() || !dataWT.isValid())
		return;
	if(data.getRowSz() != dataWT.getRowSz() || data.getColSz() != dataWT.getColSz())
		return;
	if(dataWT.getPlaneSz() != scales.getSize())
		return;
	
	// Get data size
	size_t rowSz	= data.getRowSz();
	size_t colSz	= data.getColSz();
	size_t planeSz	= scales.getSize();
	
	// Read input data
	gsl_matrix *f = gsl_matrix_alloc(rowSz, colSz);
	data.read(f);
	
	// printMatrix(f);
	// double maxVal = gsl_matrix_max(f);
	// double minVal = gsl_matrix_min(f);
	
	// Precompute input data FFT
	gsl_matrix_complex *fHat = gsl_matrix_complex_alloc(rowSz, colSz);
	rfft2(f, fHat);
	
	// Build pulsation plane
	gsl_matrix *wx = gsl_matrix_alloc(rowSz, colSz);
	gsl_matrix *wy = gsl_matrix_alloc(rowSz, colSz);
	pulse2(wx, wy);
	
	// Allocate wavelet function FFT buffer
	gsl_matrix *csiHat = gsl_matrix_alloc(rowSz, colSz);
	
	// Allocate wavelet transform coeffs buffer
	gsl_matrix_complex *wfHat = gsl_matrix_complex_alloc(rowSz, colSz);
	gsl_matrix *wf = gsl_matrix_alloc(rowSz, colSz);
	
	// Compute CWT coeffs for each scale
	for (size_t i = 0; i < planeSz; i++) {
		
		// Compute scaled wavelet function FFT
		csi.fft(scales[i], wx, wy, csiHat);
		
		// Multiply (note: conj is not needed for real wavelet)
		realComplexMatrixMulElements(csiHat, fHat, wfHat);
		
		// Get inverse FFT (computed in-place)
		irfft2(wfHat);
		
		// Copy into the buffer
		copyRealPart(wfHat, wf);
		
		// Save
		dataWT.write(wf);
		
	}
	
	// Free local memory
	gsl_matrix_free(wf);
	gsl_matrix_complex_free(wfHat);
	gsl_matrix_free(csiHat);
	gsl_matrix_free(wy);
	gsl_matrix_free(wx);
	gsl_matrix_complex_free(fHat);
	gsl_matrix_free(f);
	
}

void ricwt2(RealIsotropicWaveletFcn2 *csi,  WaveletScales *scales, MatrixIO *data, MatrixIO *dataWT) {
    
	// Check for valid data
	if(!data->isValid() || !dataWT->isValid())
		return;
	if(data->getRowSz() != dataWT->getRowSz() || data->getColSz() != dataWT->getColSz())
		return;
	if(dataWT->getPlaneSz() != scales->getSize())
		return;
	
	// Get data size
	size_t rowSz	= data->getRowSz();
	size_t colSz	= data->getColSz();
	size_t planeSz	= scales->getSize();
	
	// Read input data
	gsl_matrix *f = gsl_matrix_alloc(rowSz, colSz);
	data->read(f);
	
	// printMatrix(f);
	// double maxVal = gsl_matrix_max(f);
	// double minVal = gsl_matrix_min(f);
	
	// Precompute input data FFT
	gsl_matrix_complex *fHat = gsl_matrix_complex_alloc(rowSz, colSz);
	rfft2(f, fHat);
	
	// Build pulsation plane
	gsl_matrix *wx = gsl_matrix_alloc(rowSz, colSz);
	gsl_matrix *wy = gsl_matrix_alloc(rowSz, colSz);
	pulse2(wx, wy);
	
	// Allocate wavelet function FFT buffer
	gsl_matrix *csiHat = gsl_matrix_alloc(rowSz, colSz);
	
	// Allocate wavelet transform coeffs buffer
	gsl_matrix_complex *wfHat = gsl_matrix_complex_alloc(rowSz, colSz);
	gsl_matrix *wf = gsl_matrix_alloc(rowSz, colSz);
	
	// Compute CWT coeffs for each scale
	for (size_t i = 0; i < planeSz; i++) {
		
		// Compute scaled wavelet function FFT
		csi->fft(scales[i], wx, wy, csiHat);
		
		// Multiply (note: conj is not needed for real wavelet)
		realComplexMatrixMulElements(csiHat, fHat, wfHat);
		
		// Get inverse FFT (computed in-place)
		irfft2(wfHat);
		
		// Copy into the buffer
		copyRealPart(wfHat, wf);
		
		// Save
		dataWT->write(wf);
		
	}
	
	// Free local memory
	gsl_matrix_free(wf);
	gsl_matrix_complex_free(wfHat);
	gsl_matrix_free(csiHat);
	gsl_matrix_free(wy);
	gsl_matrix_free(wx);
	gsl_matrix_complex_free(fHat);
	gsl_matrix_free(f);
	
}

#else

void ricwt2(RealIsotropicWaveletFcn2 &csi,  WaveletScales &scales, MatrixIO &data, MatrixIO &dataWT) {
    
	// Check for valid data
	if(!data.isValid() || !dataWT.isValid())
		return;
	if(data.getRowSz() != dataWT.getRowSz() || data.getColSz() != dataWT.getColSz())
		return;
	if(dataWT.getPlaneSz() != scales.getSize())
		return;
	
	// Get data size
	size_t rowSz	= data.getRowSz();
	size_t colSz	= data.getColSz();
	size_t planeSz	= scales.getSize();
	
	// Read input data
	gsl_matrix *f = gsl_matrix_alloc(rowSz, colSz);
	data.read(f);
	
	// printMatrix(f);
	// double maxVal = gsl_matrix_max(f);
	// double minVal = gsl_matrix_min(f);
	
	// Precompute input data FFT
	gsl_matrix_complex *fHat = gsl_matrix_complex_alloc(rowSz, colSz);
	rfft2(f, fHat);
	
	// Build pulsation vectors
	gsl_vector *wx = gsl_vector_alloc(rowSz);
    pulse(wx);
	gsl_vector *wy = gsl_vector_alloc(colSz);
	pulse(wy);
	
	// Allocate wavelet function FFT buffer
	gsl_matrix *csiHat = gsl_matrix_alloc(rowSz, colSz);
	
	// Allocate wavelet transform coeffs buffer
	gsl_matrix_complex *wfHat = gsl_matrix_complex_alloc(rowSz, colSz);
	gsl_matrix *wf = gsl_matrix_alloc(rowSz, colSz);
	
	// Compute CWT coeffs for each scale
	for (size_t i = 0; i < planeSz; i++) {
		
		// Compute scaled wavelet function FFT
		csi.fft(scales[i], wx, wy, csiHat);
		
		// Multiply (note: conj is not needed for real wavelet)
		realComplexMatrixMulElements(csiHat, fHat, wfHat);
		
		// Get inverse FFT (computed in-place)
		irfft2(wfHat);
		
		// Copy into the buffer
		copyRealPart(wfHat, wf);
		
		// Save
		dataWT.write(wf);
		
	}
	
	// Free local memory
	gsl_matrix_free(wf);
	gsl_matrix_complex_free(wfHat);
	gsl_matrix_free(csiHat);
	gsl_vector_free(wy);
	gsl_vector_free(wx);
	gsl_matrix_complex_free(fHat);
	gsl_matrix_free(f);
	
}

void ricwt2(RealIsotropicWaveletFcn2 *csi,  WaveletScales *scales, MatrixIO *data, MatrixIO *dataWT) {
    
	// Check for valid data
	if(!data->isValid() || !dataWT->isValid())
		return;
	if(data->getRowSz() != dataWT->getRowSz() || data->getColSz() != dataWT->getColSz())
		return;
	if(dataWT->getPlaneSz() != scales->getSize())
		return;
	
	// Get data size
	size_t rowSz	= data->getRowSz();
	size_t colSz	= data->getColSz();
	size_t planeSz	= scales->getSize();
	
	// Read input data
	gsl_matrix *f = gsl_matrix_alloc(rowSz, colSz);
	data->read(f);
	
	// printMatrix(f);
	// double maxVal = gsl_matrix_max(f);
	// double minVal = gsl_matrix_min(f);
	
	// Precompute input data FFT
	gsl_matrix_complex *fHat = gsl_matrix_complex_alloc(rowSz, colSz);
	rfft2(f, fHat);
	
	// Build pulsation vectors
	gsl_vector *wx = gsl_vector_alloc(rowSz);
    pulse(wx);
	gsl_vector *wy = gsl_vector_alloc(colSz);
	pulse(wy);
	
	// Allocate wavelet function FFT buffer
	gsl_matrix *csiHat = gsl_matrix_alloc(rowSz, colSz);
	
	// Allocate wavelet transform coeffs buffer
	gsl_matrix_complex *wfHat = gsl_matrix_complex_alloc(rowSz, colSz);
	gsl_matrix *wf = gsl_matrix_alloc(rowSz, colSz);
	
	// Compute CWT coeffs for each scale
	for (size_t i = 0; i < planeSz; i++) {
		
		// Compute scaled wavelet function FFT
		csi->fft(scales->at(i), wx, wy, csiHat);
		
		// Multiply (note: conj is not needed for real wavelet)
		realComplexMatrixMulElements(csiHat, fHat, wfHat);
		
		// Get inverse FFT (computed in-place)
		irfft2(wfHat);
		
		// Copy into the buffer
		copyRealPart(wfHat, wf);
		
		// Save
		dataWT->write(wf);
		
	}
	
	// Free local memory
	gsl_matrix_free(wf);
	gsl_matrix_complex_free(wfHat);
	gsl_matrix_free(csiHat);
	gsl_vector_free(wy);
	gsl_vector_free(wx);
	gsl_matrix_complex_free(fHat);
	gsl_matrix_free(f);
	
}

#endif // USE_PULSATION_PLANE

void iricwt2(const RealIsotropicWaveletFcn2& csi,  const WaveletScales& scales, const MatrixIO& dataCWT, MatrixIO& data) {

}


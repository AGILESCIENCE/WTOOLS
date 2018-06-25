/*
 *  RICWT.h
 *  ContinuousWaveletTransform2
 *
 *  Created by Alessio Aboudan on 1/19/12.
 *  Copyright 2012 IASF BO. All rights reserved.
 *
 */

#ifndef __RICWT_H__
#define __RICWT_H__

#include "MatrixIO.h"
#include "RealIsotropicWaveletFcn.h"
#include "WaveletScales.h"

// Real isotropic continuous wavelet transform
void ricwt2(RealIsotropicWaveletFcn2 &csi, WaveletScales &scales, MatrixIO &data, MatrixIO &wdata);
void ricwt2(RealIsotropicWaveletFcn2 *csi, WaveletScales *scales, MatrixIO *data, MatrixIO *wdata);
void iricwt2(RealIsotropicWaveletFcn2& csi,  WaveletScales& scales, MatrixIO& wdata, MatrixIO& data);

#endif // __RCWT_H__

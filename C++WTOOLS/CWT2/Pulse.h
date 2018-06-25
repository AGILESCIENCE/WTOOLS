/*
 *  pulse2.h
 *  ContinuousWaveletTransform2
 *
 *	Creates the pulsation plane for closed form FFT computation.
 *
 *  Created by Alessio Aboudan on 1/13/12.
 *  Copyright IASF BO. All rights reserved.
 *
 */

#ifndef __PULSE_H__
#define __PULSE_H__

#include <gsl/gsl_matrix.h>

void pulse(gsl_vector *w);
void pulse2(gsl_matrix *wx, gsl_matrix *wy);

#endif // __PULSE_H__

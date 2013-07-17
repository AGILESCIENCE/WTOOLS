
#include "Pulse.h"

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_math.h>

// Transforms the domain specified by vectors vx and vy into matrices gx and gy, which can be used to evaluate functions of two variables.
// Note: vx corresponds to the gx/gy columns and vy to the rows.
void meshgrid(const gsl_vector *vx, const gsl_vector *vy, gsl_matrix *gx, gsl_matrix *gy) {
	
	double *p = gx->data;
	for (size_t i = 0; i < gx->size1; i++)
		for (size_t j = 0; j < gx->size2; j++)
			*p++ = vx->data[j];
	
	p = gy->data;
	for (size_t i = 0; i < gy->size1; i++)
		for (size_t j = 0; j < gy->size2; j++)
			*p++ = vy->data[i];
	
}

// Build the pulsation vector 1D
void pulse(gsl_vector *w) {
	
	size_t halfSize = (size_t)((double)w->size-1)/2;
	
	double k = 2*M_PI/w->size;
	
	double *p = w->data;
	for (int i = 0; i <= halfSize; i++) 
		*p++ = k*i;
	for (int i = halfSize - w->size + 1; i <= -1; i++) 
		*p++ = k*i;
	
}

// Build the pulsation plane 2D
void pulse2(gsl_matrix *wx, gsl_matrix *wy) {
	
	gsl_vector *vx = gsl_vector_alloc(wx->size2);
	pulse(vx);
	
	gsl_vector *vy = gsl_vector_alloc(wx->size1);
	pulse(vy);
	
	meshgrid(vx, vy, wx, wy);
	
	gsl_vector_free(vy);
	gsl_vector_free(vx);
	
}
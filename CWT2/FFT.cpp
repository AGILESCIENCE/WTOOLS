
#include "FFT.h"

#include <gsl/gsl_fft_complex.h>
#include <gsl/gsl_fft_real.h>

void rfft2(const gsl_matrix *src, gsl_matrix_complex *dst) {
	
	int height = src->size1;
    int width  = src->size2;
	
    // Create some required matrices
    gsl_fft_complex_workspace *ws_column 	= gsl_fft_complex_workspace_alloc(height);
    gsl_fft_complex_workspace *ws_line		= gsl_fft_complex_workspace_alloc(width);
    gsl_fft_complex_wavetable *wv_column 	= gsl_fft_complex_wavetable_alloc(height);
    gsl_fft_complex_wavetable *wv_line		= gsl_fft_complex_wavetable_alloc(width);
	
    // Copy the real part and let the imaginary part to 0
    // The GSL works in place, directly on the dst matrix
    gsl_matrix_complex_set_zero(dst);
    
    gsl_vector_view dst_vec = gsl_vector_view_array_with_stride(dst->data, 2, height*width);
    gsl_vector_view src_vec = gsl_vector_view_array(src->data, height*width);
    gsl_vector_memcpy(&dst_vec.vector, &src_vec.vector);
	
    
    // gsl_vector_memcpy(&gsl_vector_view_array_with_stride(dst->data, 2, height*width).vector, &gsl_vector_view_array(src->data, height*width).vector);
	
    // Apply the FFT on the line i
    for(int i = 0 ; i < height ; ++i)
        gsl_fft_complex_forward (&dst->data[i*2*width], 1 , width, wv_line, ws_line);
	
    // Apply the FFT on the column j
    for(int j = 0 ; j < width ; ++j)
        gsl_fft_complex_forward (&dst->data[2*j], width, height, wv_column, ws_column);
	
    gsl_fft_complex_workspace_free(ws_column);
    gsl_fft_complex_workspace_free(ws_line);
    gsl_fft_complex_wavetable_free(wv_column);
    gsl_fft_complex_wavetable_free(wv_line);
	
}

void irfft2(const gsl_matrix_complex *src, gsl_matrix_complex *dst) {
	
	int height = src->size1;
    int width  = src->size2;
	
    // Create some required matrices
    gsl_fft_complex_workspace *ws_column 	= gsl_fft_complex_workspace_alloc(height);
    gsl_fft_complex_workspace *ws_line		= gsl_fft_complex_workspace_alloc(width);
    gsl_fft_complex_wavetable *wv_column 	= gsl_fft_complex_wavetable_alloc(height);
    gsl_fft_complex_wavetable *wv_line		= gsl_fft_complex_wavetable_alloc(width);
	
    // Copy the real and imaginary parts
    // The GSL works in place, directly on the dst matrix
    gsl_matrix_complex_memcpy(dst, src);
	
    // Apply the FFT on the line i
    for(int i = 0 ; i < height ; ++i)
        gsl_fft_complex_inverse(&dst->data[i*2*width],1 , width, wv_line, ws_line);
	
    // Apply the FFT on the column j
    for(int j = 0 ; j < width ; ++j)
        gsl_fft_complex_inverse(&dst->data[2*j],width, height, wv_column, ws_column);
	
    gsl_fft_complex_workspace_free(ws_column);
    gsl_fft_complex_workspace_free(ws_line);
    gsl_fft_complex_wavetable_free(wv_column);
    gsl_fft_complex_wavetable_free(wv_line);
	
}

void irfft2(gsl_matrix_complex *m) {
	
	int height = m->size1;
    int width  = m->size2;
	
    // Create some required matrices
    gsl_fft_complex_workspace *ws_column 	= gsl_fft_complex_workspace_alloc(height);
    gsl_fft_complex_workspace *ws_line		= gsl_fft_complex_workspace_alloc(width);
    gsl_fft_complex_wavetable *wv_column 	= gsl_fft_complex_wavetable_alloc(height);
    gsl_fft_complex_wavetable *wv_line		= gsl_fft_complex_wavetable_alloc(width);
	
    // Apply the FFT on the line i
    for(int i = 0 ; i < height ; ++i)
        gsl_fft_complex_inverse(&m->data[i*2*width],1 , width, wv_line, ws_line);
	
    // Apply the FFT on the column j
    for(int j = 0 ; j < width ; ++j)
        gsl_fft_complex_inverse(&m->data[2*j],width, height, wv_column, ws_column);
	
    gsl_fft_complex_workspace_free(ws_column);
    gsl_fft_complex_workspace_free(ws_line);
    gsl_fft_complex_wavetable_free(wv_column);
    gsl_fft_complex_wavetable_free(wv_line);
	
}

void realComplexMatrixMulElements(const gsl_matrix *a, gsl_matrix_complex *b) {
	
	double *pa = a->data;
	double *pb = b->data;
	for(unsigned int i = 0; i < a->size1; i++) {
		
		for(unsigned int j = 0; j < a->size2; j++) {
			*pb++ = *pb * *pa;
			*pb++ = *pb * *pa++;
		}
		
		pa += a->tda - a->size2;
		pb += b->tda - b->size2;
		
	}

}

void realComplexMatrixMulElements(const gsl_matrix *a, const gsl_matrix_complex *b, gsl_matrix_complex *c) {

	double *pa = a->data;
	double *pb = b->data;
	double *pc = c->data;
	for(unsigned int i = 0; i < a->size1; i++) {
		
		for(unsigned int j = 0; j < a->size2; j++) {
			*pc++ = *pb++ * *pa;
			*pc++ = *pb++ * *pa++;
		}
		
		pa += a->tda - a->size2;
		pb += b->tda - b->size2;
		pc += c->tda - c->size2;
		
	}
	
}

void copyRealPart(const gsl_matrix_complex *a, gsl_matrix *b) {

	double *pa = a->data;
	double *pb = b->data;
	
	for(unsigned int i = 0; i < a->size1; i++) {
		
		for(unsigned int j = 0; j < a->size2; j++) {
			*pb++ = *pa;
			pa += 2;
		}
		
		pa += a->tda - a->size2;
		pb += b->tda - b->size2;
	
	}
	
}

void copyImagPart(const gsl_matrix_complex *a, gsl_matrix *b) {
	
	double *pa = a->data + 1;
	double *pb = b->data;
	
	for(unsigned int i = 0; i < a->size1; i++) {
		
		for(unsigned int j = 0; j < a->size2; j++) {
			*pb++ = *pa;
			pa += 2;
		}
		
		pa += a->tda - a->size2;
		pb += b->tda - b->size2;
		
	}
	
}





#include <stdio.h>
#include <math.h>

#include "GetOptions.h"
#include "Histogram.h"

using namespace std;

void applyThreshold(gsl_matrix *m, double th) {
	double *p = m->data;
	for(unsigned int i = 0; i < m->size1; i++) {
		for (unsigned int j = 0;  j < m->size2; j++) {
			if (*p > th)
				*p = 1;
			else
				*p = 0;
			
			p++;
		}
		p -= m->size2;
		p += m->tda;
	}
}

int main (int argc, char * const argv[]) {
    
	printf("Maximum Entropy Thresholding - Version 0.0\n");
	
	GetOptions opt(argc, argv);
	
	if(opt.isValid()) {
		
		printf("Running\n");
		
		MatrixIO &inFile	= opt.getInputFile();
		MatrixIO &outFile	= opt.getOutputFile();
		
		gsl_matrix *m = gsl_matrix_alloc(inFile.getRowSz(), inFile.getColSz());
		
		for(unsigned int i = 0; i < inFile.getPlaneSz(); i++) {
			
			if(!inFile.read(m))
				printf("ERROR : unable to read data\n");
			
            double th = 0;
            if (opt.getHistSize()) {
                Histogram h(m, opt.getHistSize());
                th = h.getMET();
            } else {
                Histogram h(m, opt.getHistResolution());
                th = h.getMET();
            }
			
			if (opt.isVerbose()) {
				
				double minVal, maxVal;
				gsl_matrix_minmax(m, &minVal, &maxVal);
				
				printf("\t[%05d] min %10.3e max %10.3e th %10.3e\n", i+1, minVal, maxVal, th);
				
			}
			
			applyThreshold(m, th);
			
			if(!outFile.write(m))
				printf("ERROR : unable to write data\n");
			
		}
		
		gsl_matrix_free(m);
		
		
	}
	
	printf("End");
	
	return 0;
	
}

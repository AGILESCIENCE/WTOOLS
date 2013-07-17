#include <stdio.h>

#include "GetOptions.h"
#include "Labeling.h"
#include "ConnectedComponent.h"

using namespace std;

void copyPlane(const gsl_matrix *m, int *p) {
	double *pm = m->data;
	for(unsigned int i = 0; i < m->size1; i++) {
		for(unsigned int j = 0; j < m->size2; j++)
			*p++ = (int)*pm++;
		pm += m->tda - m->size2;
	}
}

int main (int argc, char * const argv[]) {
    
	printf("Connected components labelling - Version 1.0\n");
	
	GetOptions opt(argc, argv);
	
	if(opt.isValid()) {
		
		// Get input file
		MatrixIO &inFile = opt.getInputFile();
		
		// Allocate labels
		Labeling labels(inFile.getPlaneSz(), inFile.getRowSz(), inFile.getColSz());
		
		// Allocate temporary matrix (to be optimized)
		gsl_matrix *m = gsl_matrix_alloc(inFile.getRowSz(), inFile.getColSz());
		
		// Read plane by plane
        for(unsigned int i = 0; i < inFile.getPlaneSz(); i++) {
			
			if(!inFile.read(m)){
				printf("ERROR : unable to read data\n");
                break;
            }
            
            int *p = labels.GetPlane(i);
            if(!p) {
                printf("ERROR : unable to access label plane\n");
                break;
            }
            
            // Copy the data
            copyPlane(m, p);
            
        }
		
		gsl_matrix_free(m);
		
		// Labelling
		printf("Labeling\n");
		int n = labels.LabelingTwoSteps83D();
		if (n >= 0)
			printf("\tconnected components labelled\n");
		
		// Post process labels
		ConnectedComponentProcessor cp(labels.colSz, labels.rowSz, labels.nplan, labels.array_value);
        
        // Apply filte on component size
        if (opt.applyFilterCompSz()) {
            unsigned int minSz, maxSz;
            opt.getFilterCompSz(minSz, maxSz);
            cp.filterCompSz(minSz, maxSz);
        }
        
        // Apply filter on component radius
        if (opt.applyFilterRadius()) {
            float minRadius, maxRadius;
            opt.getFilterRadius(minRadius, maxRadius);
            cp.filterRadius(minRadius, maxRadius);
        }
        
        // Write to console
		cp.print();
        
        // Write list for DS9
        cp.printForDS9(opt.getOutputFileName(), 5);
		
	}
	
	printf("End");
	
	return 0;
	
}
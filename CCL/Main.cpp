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
		
        // Index of min/max scale
        unsigned int minScale   = opt.getMinScaleOfInterest() - 1;
        unsigned int maxScale   = opt.getMaxScaleOfInterest() - 1;
        unsigned int totScaleSz = maxScale - minScale + 1;
        
		// Allocate labels
		Labeling labels(totScaleSz, inFile.getRowSz(), inFile.getColSz());
		
		// Allocate temporary matrix (to be optimized)
		gsl_matrix *m = gsl_matrix_alloc(inFile.getRowSz(), inFile.getColSz());
		
		// Read plane by plane
        unsigned int j = 0;
        for(unsigned int i = 0; i < inFile.getPlaneSz(); i++) {
			
			if(!inFile.read(m)){
				printf("ERROR : unable to read data\n");
                break;
            }
            
            if(i >= minScale && i <= maxScale) {
            
                int *p = labels.GetPlane(j++);
                if(!p) {
                    printf("ERROR : unable to access label plane\n");
                    break;
                }
            
                // Copy the data
                copyPlane(m, p);
            
                // printf("Add scale %d\n", i);
                
            }
                
        }
		
		gsl_matrix_free(m);
		
		// Labelling
		printf("Labeling\n");
		int n = labels.LabelingTwoSteps83D();
		if (n >= 0)
			printf("\tconnected components labelled\n");
		
        // Only for DEBUG
        string tmpName = opt.getOutputFileName() + ".lbl";
        labels.saveToFile(tmpName.c_str());
        
		// Post process labels
		ConnectedComponentProcessor cp(labels.colSz, labels.rowSz, labels.nplan, labels.array_value);
        
        // Apply filter on component size
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
        cp.printForDS9(opt.getOutputFileName(), 0);
		
	}
	
	printf("End");
	
	return 0;
	
}

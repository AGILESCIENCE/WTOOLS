/*
 *  GetOptions.cpp
 *  ContinuousWaveletTransform2
 *
 *  Created by Alessio Aboudan on 1/25/12.
 *  Copyright 2012 IASF BO. All rights reserved.
 *
 */

#include "GetOptions.h"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <limits.h>
#include <float.h>

using namespace std;


GetOptions::GetOptions(int argc, char * const argv[]) : inFile(0), verbose(false), doFltCmpSz(false), fltMinCmpSz(0), fltMaxCmpSz(UINT_MAX), doFltRad(false), fltMinRad(0), fltMaxRad(FLT_MAX), isValidFlag(false)  {
	
	if(argc == 1) {
		printHelp();
		return;
	}
	
	parseOptions(argc, argv);
	
	checkOptions();
	
	processOptions();
	
}

GetOptions::~GetOptions() {
	
	if(inFile)
		delete inFile;
	
}

bool GetOptions::isValid() {
		
	bool inFileOk = false;
	if (inFile)
		if (inFile->isValid())
			inFileOk = true;

	return inFileOk & isValidFlag;

}

MatrixIO& GetOptions::getInputFile() { return dynamic_cast<MatrixIO&>(*inFile); }

void GetOptions::printHelp() {
    
    printf("Usage:\n\n");
    
    printf("\t-s --scales-of-interest min:max (optional)\n");
    printf("\t\tSelects the scales to be labelled.\n\n");
    
    printf("\t-c --component-size min:max (optional)\n");
    printf("\t\tSelects the components with a number of pixels in the specified range.\n\n");
    
    printf("\t-r --radius min:max (optional)\n");
    printf("\t\tSelects the components with a radius in the specified range.\n\n");
    
    printf("\t-i --input file_name (mandatory)\n\n");
    
    printf("\t-o --output file_name (mandatory)\n\n");
    
    printf("\t-v --verbose\n");
    printf("\t\tset verbose mode\n\n");
    
    printf("Examples:\n\n");
    
    printf("\tccl -v -c 5:50 -i test.met -o test.reg\n");
    printf("\tccl -v -r 1.5:15.0 -i test.met -o test.reg\n");
	
}

void GetOptions::parseOptions(int argc, char * const argv[]) {

	static char shortOptions[] = "i:o:s:vc:r:";
	
	static struct option longOptions[] = {
		{"input",				required_argument,	0, 'i'},
		{"output",				required_argument,	0, 'o'},
		{"scales-of-interest",	optional_argument,	0, 's'},
		{"verbose",				no_argument,		0, 'v'},
        {"component-size",		optional_argument,	0, 'c'},
		{"radius",              optional_argument,	0, 'r'},
		{0, 0, 0, 0}
	};
	
	while(true) {
		
		int optionIndex = 0;
		int c = getopt_long(argc, argv, shortOptions, longOptions, &optionIndex);
		
		// Detect the end of the options.
		if(c == -1)
			break;
		
		switch(c) {
				
			case 'i':
				inFileName = string(optarg);
				break;
				
			case 'o':
				outFileName = string(optarg);
				break;
			
			case 's':
				scalesOfInterestPar= string(optarg);
				break;
				
			case 'v':
				verbose = true;
				break;
                
            case 'c':
                doFltCmpSz = true;
                fltCmpSzPar = string(optarg);
                break;
                
            case 'r':
                doFltRad = true;
                fltRadPar = string(optarg); 
                break;
				
			default:
				printf("ERROR : unknown option -%c\n", c);
				return;
				
		}
		
	}
	
}

void GetOptions::checkOptions() {
	
	// Input file
	if(inFileName.empty()) {
		printf("ERROR : no input file\n");
		return;
	}

	// Default output file
	if(outFileName.empty()) {
		Fields fields = getFileNameFields(inFileName.c_str());
		outFileName = fields[0] + ".reg";
	}
	
}

void GetOptions::processOptions() {
		
	// Input file
	MatrixFITSIO *inFileFITS = NULL;
	Fields fields = getFileNameFields(inFileName.c_str());
	if(strcmp(fields[2].c_str(), "mtx") == 0)
		inFile = new MatrixFileIO(inFileName.c_str());
	else {
		inFileFITS = new MatrixFITSIO(inFileName.c_str());
		inFile = inFileFITS;
	}
	
	if(verbose) {
		printf("Input\n");
		printf("\tfile: %s\n", inFileName.c_str());
	}
	
	if(!inFile->isValid()) {
		printf("ERROR : unable to open %s\n", inFileName.c_str());
		return;
	}
    
    if(verbose) {
		printf("Output\n");
		printf("\tfile: %s\n", outFileName.c_str());
	}
    
    if (scalesOfInterestPar.empty()) {
        
        scaleMin = 0;
        scaleMax = inFile->getPlaneSz() - 1;
        
    } else {
        
        Fields fields = getFields(scalesOfInterestPar.c_str());
        if(fields.empty() || fields.size() != 2) {
            printf("ERROR : parsing scales of interest \n");
            return;
        }
        
        scaleMin = atoi(fields[0].c_str());
        scaleMax = atoi(fields[1].c_str());
        
    }

    if(verbose) {
		printf("Scales\n");
		printf("\tmin %d max %d\n", scaleMin, scaleMax);
	}
    
    isValidFlag = true;
	
	// Filter for components size
    if (doFltCmpSz) {
        
        Fields fields = getFields(fltCmpSzPar.c_str());
        if(fields.empty() || fields.size() != 2) {
            printf("ERROR : parsing filter for component size parameters \n");
            isValidFlag = false;
            return;
        }
        
        // TODO check for max/min
        
        fltMinCmpSz = atoi(fields[0].c_str());
        fltMaxCmpSz = atoi(fields[1].c_str());
        
    }
    
    // Filter for component radius
	if (doFltRad) {
        
        Fields fields = getFields(fltRadPar.c_str());
        if(fields.empty() || fields.size() != 2) {
            printf("ERROR : parsing filter for radius parameters \n");
            isValidFlag = false;
            return;
        }
        
        // TODO check for max/min
        
        fltMinRad = atof(fields[0].c_str());
        fltMaxRad = atof(fields[1].c_str());
        
    }
}

GetOptions::Fields GetOptions::getFields(const char *str, char sep) {
	string tmp = string(str);
	Fields fields;
	while(true) {
		size_t pos = tmp.find_first_of(sep);		
		if(pos == string::npos && tmp.size() != 0) {	// One field without sep char
			fields.push_back(tmp);						// Save string
			break;										// End loop
		} else {										// Field found
			if(pos != 0)
				fields.push_back(tmp.substr(0, pos));	// Save string
			tmp = tmp.substr(pos+1);					// Get remaining fields
			if(tmp.size() == 0)							// No more fields
				break;									// End loop
		}
	}
	return fields;
}

GetOptions::Fields GetOptions::getFileNameFields(const char* fname) {
	string tmp = string(fname);
	// Get extension (if any)
	string pathAndName, ext;
	size_t dotPos = tmp.find_last_of('.');
	if(dotPos != string::npos) {
		pathAndName = tmp.substr(0, dotPos);
		ext = tmp.substr(dotPos+1);
	} else {
		pathAndName = tmp;
		ext.clear();
	}
	// Get path and name (if any)
	string path, name;
	size_t slashPos = pathAndName.find_last_of('/');
	if(slashPos != string::npos) {
		path = pathAndName.substr(0, slashPos);
		name = pathAndName.substr(slashPos+1);
	} else {
		path.clear();
		name = pathAndName;
	}
	Fields fields;
	fields.push_back(path);
	fields.push_back(name);
	fields.push_back(ext);
	return fields;
}
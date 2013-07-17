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

using namespace std;


GetOptions::GetOptions(int argc, char * const argv[]) : scalesSz(-1), scalesMin(-1), scalesMax(-1), outDouble(false), verbose(false), isValidFlag(true) {
	
	if(argc == 1) {
		printHelp();
        isValidFlag = false;
		return;
	}
	
	parseOptions(argc, argv);
	processOptions();
	
}

/*
GetOptions::~GetOptions() {
	
}
 */

void GetOptions::printHelp() {
    
    printf("Usage:\n\n");
    
    printf("\t-w --wavelet type (optional)\n");
    printf("\t\tSpecifies the wavelet family to be used. Possible values of type are:\n");
    printf("\t\t  log    : Laplacian of Gaussian (default)\n\n");
    
    printf("\t-s --scale type:size:min:max (mandatory)\n");
    printf("\t\tDefines the scales at which to compute the WT. Possible values of type are:\n");
	printf("\t\t  linear    : linear scales between min and max values\n");
    printf("\t\t  dyadic    : dyadic scales between min and max values\n");
    printf("\t\tOther parameters are:\n");
    printf("\t\t  size      : number of scales to be used\n");
    printf("\t\t  min       : min scale\n");
    printf("\t\t  max       : max scale\n\n");

    printf("\t-i --input file_name (mandatory)\n\n");
    
    printf("\t-o --output file_name (mandatory)\n\n");
    
    printf("\t-d --out-double\n");
    printf("\t\twhen set the output WT will be saved in double precision\n\n");
    
    printf("\t-v --verbose\n");
    printf("\t\tset verbose mode\n\n");
    
    printf("Examples:\n\n");
    
    printf("\tcwt2 -v -w log -s dyadic:10:2:1024 -i test.cts -o test.wtf\n");

}

void GetOptions::parseOptions(int argc, char * const argv[]) {

	static char shortOptions[] = "w:s:i:o:dv";
	
	static struct option longOptions[] = {
		{"wavelet",		required_argument,	0, 'w'},
		{"scale",		required_argument,	0, 's'},
		{"input",		required_argument,	0, 'i'},
		{"output",		required_argument,	0, 'o'},
		{"out-double",	no_argument,		0, 'd'},
		{"verbose",		no_argument,		0, 'v'},
		{0, 0, 0, 0}
	};
	
	while(true) {
		
		int optionIndex = 0;
		int c = getopt_long(argc, argv, shortOptions, longOptions, &optionIndex);
		
		// Detect the end of the options.
		if(c == -1)
			break;
		
		switch(c) {
				
			case 'w':
				waveletType = string(optarg);
				break;
				
			case 's':
				scalesPar = string(optarg);
				break;
				
			case 'i':
				inFileName = string(optarg);
				break;
				
			case 'o':
				outFileName = string(optarg);
				break;
				
			case 'd':
				outDouble = true;
				break;

			case 'v':
				verbose = true;
				break;	
				
			default:
				printf("ERROR : unknown option -%c\n", c);
				return;
				
		}
		
	}
	
}

void GetOptions::processOptions() {
    
    /*
     * Wavelet type
     */
    
    if (waveletType.empty())
        waveletType = "log";
    
    /* 
     * Wavelet scales
     */
    
    // Wavelet scales
	Fields fields = getFields(scalesPar.c_str());
	if(fields.empty() | fields.size() != 4) {
		printf("ERROR : parsing scales parameters\n");
        isValidFlag = false;
		return;
	}
	
    scalesType  = fields[0].c_str();
    scalesSz    = atoi(fields[1].c_str());
	scalesMin   = atof(fields[2].c_str());
	scalesMax   = atof(fields[3].c_str());

    /*
     * Input file
     */
    
    if (inFileName.empty()) {
        printf("ERROR : missing input file\n");
        isValidFlag = false;
        return;
    }
    
    fields = getFileNameFields(inFileName.c_str());
    inFileType = fields[2].c_str();
    
    /*
     * Output file
     */
    
    if (outFileName.empty()) {
        printf("ERROR : missing output file\n");
        isValidFlag = false;
        return;
    }
    
    fields = getFileNameFields(outFileName.c_str());
    outFileType = fields[2].c_str();
    
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
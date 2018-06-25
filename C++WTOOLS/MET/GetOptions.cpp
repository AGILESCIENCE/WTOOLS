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
#include <cstring>

using namespace std;


GetOptions::GetOptions(int argc, char * const argv[]) : inFile(0), outFile(0), histSz(0), res(0), verbose(false) {

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

	if(outFile)
		delete outFile;

}

bool GetOptions::isValid() {

	bool inFileOk = false;
	if (inFile)
		if (inFile->isValid())
			inFileOk = true;

	bool outFileOk = false;
	if (outFile)
		if (outFile->isValid())
			outFileOk = true;

	return inFileOk && outFileOk;

}

MatrixIO& GetOptions::getInputFile() { return dynamic_cast<MatrixIO&>(*inFile); }

MatrixIO& GetOptions::getOutputFile() { return dynamic_cast<MatrixIO&>(*outFile); }

void GetOptions::printHelp() {

}

void GetOptions::parseOptions(int argc, char * const argv[]) {

	static char shortOptions[] = "n:r:i:o:v";

	static struct option longOptions[] = {
        {"histsize",	optional_argument,	0, 'n'},
		{"resolution",	optional_argument,	0, 'r'},
		{"input",		required_argument,	0, 'i'},
		{"output",		required_argument,	0, 'o'},
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

            case 'n':
				histSzPar = string(optarg);
				break;

			case 'r':
				resolutionPar = string(optarg);
				break;

			case 'i':
				inFileName = string(optarg);
				break;

			case 'o':
				outFileName = string(optarg);
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

void GetOptions::checkOptions() {

    // Default histogram size/resolution
    if(histSzPar.empty()) {
        if(resolutionPar.empty()) {
            histSzPar = "100";
        }
    }

	// Input file
	if(inFileName.empty()) {
		printf("ERROR : no input file\n");
		return;
	}

	// Default output file
	if(outFileName.empty()) {
		Fields fields = getFileNameFields(inFileName.c_str());
		outFileName = fields[0] + "_met.fts";
	}

}

void GetOptions::processOptions() {

	// Histogram size/resolution
    if(!histSzPar.empty()) {

        histSz = atoi(histSzPar.c_str());

        if(verbose) {
            printf("Histogram\n");
            printf("\tsize %10d\n", histSz);
        }

    } else {

        res = atof(resolutionPar.c_str());

        if(verbose) {
            printf("Histogram\n");
            printf("\tresolution %10.4e\n", res);
        }

    }

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

	// Output file
	MatrixFITSIO *outFileFITS = NULL;
	fields = getFileNameFields(outFileName.c_str());
	if(strcmp(fields[2].c_str(), "mtx") == 0)
		outFile = new MatrixFileIO(inFileName.c_str(), inFile->getRowSz(), inFile->getColSz(), inFile->getPlaneSz());
	else {

		// Use a byte to store the mask
		outFileFITS = new MatrixFITSIO(outFileName.c_str(), inFile->getRowSz(), inFile->getColSz(), inFile->getPlaneSz(), BYTE_IMG);
		outFile = outFileFITS;

		// Check data type inside the header

		// Copy header
		//if(inFileFITS)
		//	outFileFITS->copyHeaderFrom(*inFileFITS);

		// Add MET specific keywords
		outFileFITS->writeComment("MET derived mask");
		outFileFITS->writeKeyWord("HRES", res, "Histogram resolution");

	}

	if(verbose) {
		printf("Output\n");
		printf("\tfile: %s\n", outFileName.c_str());
		printf("\tdata type byte\n");
	}

	if(!outFile->isValid())
		printf("ERROR : unable to open %s\n", outFileName.c_str());

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

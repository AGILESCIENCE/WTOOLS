/*
 *  GetOptions.h
 *  ContinuousWaveletTransform2
 *
 *  Created by Alessio Aboudan on 1/25/12.
 *  Copyright 2012 IASF BO. All rights reserved.
 *
 */

#ifndef __GET_OPTIONS_H__
#define __GET_OPTIONS_H__

#include "MatrixIO.h"
#include <string>
#include <vector>

class GetOptions {
	
public:
	
	GetOptions(int argc, char * const argv[]);
	~GetOptions();
	
	bool isValid();
	
	MatrixIO& getInputFile();
	std::string getInputFileName() { return inFileName; };
	MatrixIO& getOutputFile();
	std::string getOutputFileName() { return outFileName; };
	double getResolution() { return res; };
	bool outputIsDouble() { return outDouble; };
	bool isVerbose() { return verbose; };
	
protected:
	
	std::string inFileName;
	MatrixIO *inFile;
	
	std::string outFileName;
	MatrixIO *outFile;
	
	std::string resolutionPar;
	double res;
	
	bool	outDouble;
	bool	verbose;
	
	void printHelp();
	void parseOptions(int argc, char * const argv[]);
	void checkOptions();
	void processOptions();
	
	typedef std::vector<std::string> Fields;
	
	Fields getFields(const char* str, char sep = ':');
	Fields getFileNameFields(const char* fname);
	
};

#endif // __GET_OPTIONS_H__
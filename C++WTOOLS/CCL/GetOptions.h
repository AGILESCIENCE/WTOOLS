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
	
	std::string getOutputFileName() { return outFileName; };
	
    unsigned int getMinScaleOfInterest() { return scaleMin; };
    unsigned int getMaxScaleOfInterest() { return scaleMax; };
    
	bool isVerbose() { return verbose; };
    
    bool applyFilterCompSz() { return doFltCmpSz; };
    void getFilterCompSz(unsigned int& minSz, unsigned int& maxSz) {
        minSz = fltMinCmpSz;
        maxSz = fltMaxCmpSz;
    };
    
    bool applyFilterRadius() { return doFltRad; };
    void getFilterRadius(float& minRadius, float& maxRadius) {
        minRadius = fltMinRad;
        maxRadius = fltMaxRad;
    }

protected:
	
	std::string scalesOfInterestPar;
	unsigned int scaleMin;
    unsigned int scaleMax;
	
	std::string inFileName;
	MatrixIO *inFile;
	
	std::string outFileName;
    
	bool verbose;
    
    bool doFltCmpSz;
    std::string fltCmpSzPar;
    unsigned int fltMinCmpSz;
    unsigned int fltMaxCmpSz;

    bool doFltRad;
    std::string fltRadPar;
    float fltMinRad;
    float fltMaxRad;
    
    bool isValidFlag;
    
	void printHelp();
	void parseOptions(int argc, char * const argv[]);
	void checkOptions();
	void processOptions();
	
	typedef std::vector<std::string> Fields;
	
	Fields getFields(const char* str, char sep = ':');
	Fields getFileNameFields(const char* fname);
	
};

#endif // __GET_OPTIONS_H__
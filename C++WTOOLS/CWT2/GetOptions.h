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

#include <string>
#include <vector>

class GetOptions {
	
public:
	
    typedef std::vector<std::string> Fields;
    
	GetOptions(int argc, char * const argv[]);
	// ~GetOptions();
    
    bool isValid() { return isValidFlag; };
	
    std::string waveletType;
    
    std::string scalesType;
    int         scalesSz;
	float       scalesMin;
	float       scalesMax;
    
    std::string inFileType;
    std::string inFileName;
    
    std::string outFileType;
    std::string outFileName;
    
    bool	outDouble;
	bool	verbose;
    	
protected:
	
	std::string scalesPar;
    
    bool isValidFlag;
	
	void printHelp();
	void parseOptions(int argc, char * const argv[]);
    void processOptions();
    
    Fields getFields(const char* str, char sep = ':');
	Fields getFileNameFields(const char* fname);
	
};

#endif // __GET_OPTIONS_H__
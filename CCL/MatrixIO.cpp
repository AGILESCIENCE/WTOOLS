/*
 * MatrixIO.cpp
 *
 *  Created on: Jan 17, 2012
 *      Author: alessio
 */

#include <memory.h>

#include "MatrixIO.h"

MatrixIO::MatrixIO() : rows(0), cols(0), planes(0) {

}

MatrixIO::MatrixIO(size_t rowSz, size_t colSz, size_t planeSz) : rows(rowSz), cols(colSz), planes(planeSz) {

}

MatrixIO::~MatrixIO() {
	
}

MatrixFileIO::MatrixFileIO(const char* fname) : MatrixIO(), pFile(NULL) {

	pFile = fopen(fname, "r");

	if(pFile != NULL)
		readHeader();

}

MatrixFileIO::MatrixFileIO(const char* fname, size_t rowSz, size_t colSz, size_t planeSz, DataType dataType) : MatrixIO(rowSz, colSz, planeSz), pFile(NULL), type(dataType){

	// Open file
	pFile = fopen(fname, "w");

	if(isValid())
		writeHeader();

}

MatrixFileIO::~MatrixFileIO() {
	
	fclose(pFile);
	
}

bool MatrixFileIO::MatrixFileIO::read(gsl_matrix *m) {
	
	if(isValid()) {
	
		if(m->size1 != rows || m->size2 != cols)
			return false;
		
		switch (type) {
			case Float:
				return readPlaneFloat(m);
			case Double:
				return readPlaneDouble(m);
			default:
				return false;
		}
		
	} else
		return false;

}

bool MatrixFileIO::MatrixFileIO::write(gsl_matrix *m) {
	
	if(isValid()) {
		
		if(m->size1 != rows || m->size2 != cols)
			return false;
		
		switch (type) {
			case Float:
				return writePlaneFloat(m);
			case Double:
				return writePlaneDouble(m);
			default:
				return false;
		}
		
	} else
		return false;
	
}

bool MatrixFileIO::MatrixFileIO::isValid() {
	return pFile && type!= None && rows && cols && planes;
}

inline void MatrixFileIO::readHeader() {
	
	// Reads the data type used in the file
	fread(&type, sizeof(unsigned char), 1, pFile);
	
	// Limits size to 32 bit to cope with Matlab types
	unsigned int sz[3];
	fread(sz, sizeof(unsigned int), 3, pFile);
	
	rows	= (size_t)sz[0];
	cols	= (size_t)sz[1];
	planes	= (size_t)sz[2];

}

inline void MatrixFileIO::writeHeader() {
	
	// Writes the data type used in the file
	fwrite(&type, sizeof(unsigned char), 1, pFile);
	
	// Limits size to 32 bit to cope with Matlab types
	unsigned int sz[3];
	sz[0] = (unsigned int)rows;
	sz[1] = (unsigned int)cols;
	sz[2] = (unsigned int)planes;
	
	fwrite(sz, sizeof(unsigned int), 3, pFile);
	
}

inline bool MatrixFileIO::readPlaneFloat(gsl_matrix *m) {
	
	size_t off = m->tda - m->size2;
	
	double *pm = m->data;
	for(size_t i = 0; i < rows; i++) {
		for(size_t j = 0; j < cols; j++) {
		
			float tmp;
			fread(&tmp, sizeof(float), 1, pFile);
			
			if(ferror(pFile)) {
				clearerr (pFile);
				return false;
			}
			
			*pm++ = (double)tmp;
		
		}
		
		pm += off;
		
	}
	
	return true;
	
}

inline bool MatrixFileIO::writePlaneFloat(gsl_matrix *m) {
	
	size_t off = m->tda - m->size2;
	
	double *pm = m->data;
	for(size_t i = 0; i < rows; i++) {
		for(size_t j = 0; j < cols; j++) {
			
			float tmp = (float)*pm++;
			fwrite(&tmp, sizeof(float), 1, pFile);
		
			if(ferror(pFile)) {
				clearerr(pFile);
				return false;
			}
		}
		
		pm += off;
		
	}
	
	return true;
	
}


inline bool MatrixFileIO::readPlaneDouble(gsl_matrix *m) {
	
	double *pm = m->data;
	for(size_t i = 0; i < rows; i++) {

		fread(pm, sizeof(double), cols, pFile);

		if(ferror(pFile)) {
			clearerr (pFile);
			return false;
		}

		pm += m->tda;

	}
	
	return true;
	
}

inline bool MatrixFileIO::writePlaneDouble(gsl_matrix *m) {

	double *pm = m->data;
	for(size_t i = 0; i < rows; i++) {

		fwrite(pm, sizeof(double), cols, pFile);

		if(ferror(pFile)) {
			clearerr(pFile);
			return false;
		}

		pm += m->tda;

	}
	
	return true;

}

#define FITS_MAX_DIM 3

MatrixFITSIO::MatrixFITSIO(const char* fname) : MatrixIO(), pFile(NULL), status(0), firstElement(1) {
	
	fits_open_file(&pFile, fname, READONLY, &status);
	
	// Get input image size
	int naxis;
	long naxes[FITS_MAX_DIM];
	memset(naxes, 0, FITS_MAX_DIM*sizeof(long));
	fits_get_img_param(pFile, FITS_MAX_DIM, &type, &naxis, naxes, &status);
	
	// Save image size
	cols = naxes[0];
	rows = naxes[1];
	if(naxes[2] == 0)
		planes = 1;
	else
		planes = naxes[2];
	
}

MatrixFITSIO::MatrixFITSIO(const char* fname, size_t rowSz, size_t colSz, size_t planeSz,  int dataType) : MatrixIO(rowSz, colSz, planeSz), pFile(NULL),  status(0), type(dataType), firstElement(1) {
	
	// Copy the file name
	// memset(fileName, fileNameSz, sizeof(char));
	// strcpy(fileName, fname);
	
	// Create output image
	fits_create_file(&pFile, fname, &status);
	
	// Set the size
	long naxes[3];
	naxes[0] = cols;
	naxes[1] = rows;
	if(planes == 1)
		naxes[2] = 0;
	else
		naxes[2] = planes;
	
	// Create image data
	fits_create_img(pFile, type, 3, naxes, &status);
	
}

MatrixFITSIO::~MatrixFITSIO() {
	
	fits_close_file(pFile, &status);
	
}

void MatrixFITSIO::copyHeaderFrom(const MatrixFITSIO &inFile) {
	
	// Get # of keywords of the input file
	int nkeys = 0;
	fits_get_hdrspace(inFile.pFile, &nkeys, NULL, &status); 
	
	// Copy keywords but skip the standard header keywords (first 9 elements)
	for(int i = 9; i <= nkeys; i++) { 
		
		// Read keyword
		char card[FLEN_CARD];
		if(fits_read_record(inFile.pFile, i, card, &status))
			break;
		
		fits_write_record(pFile, card, &status);
		
	}

}

void MatrixFITSIO::writeComment(const char *value) {
	fits_write_comment(pFile, value, &status);
}

void MatrixFITSIO::writeKeyWord(const char *name, const char *value, const char *comment) {	
	char tmp[128];
	strcpy(tmp, value);
	fits_write_key(pFile, TSTRING, name, tmp, comment, &status);
}

void MatrixFITSIO::writeKeyWord(const char *name, double value, const char *comment) {
	fits_write_key(pFile, TDOUBLE, name, &value, comment, &status);
}

void MatrixFITSIO::writeKeyWord(const char *name, float value, const char *comment) {
	fits_write_key(pFile, TFLOAT, name, &value, comment, &status);
}

void MatrixFITSIO::writeKeyWord(const char *name, int value, const char *comment) {
	fits_write_key(pFile, TINT, name, &value, comment, &status);
}


// Input
bool MatrixFITSIO::read(gsl_matrix *m) {
	
	if(isValid()) {
		
		if(m->size1 == rows || m->size2 == cols) {
			
			// Get a pointer to the matrix buffer
			double *pm = m->data;
			
			for(unsigned int i = 0; i < rows; i++) {
				
				// Read row
				fits_read_img(pFile, TDOUBLE, firstElement, cols, 0, pm, 0, &status);
				if(status) 
					return false;
				
				// Update matrix pointer
				pm += m->tda;
				
				// Update element count
				firstElement += cols;
				
			}
			
			return true;
			
		} else
			return false;

	} else
		return false;

}

// Output


bool MatrixFITSIO::write(gsl_matrix *m) {
	
	if(isValid()) {
		
		// reopen();
		
		if(m->size1 == rows || m->size2 == cols) {
			
			// Get a pointer to the matrix buffer
			double *pm = m->data;
			
			for(unsigned int i = 0; i < rows; i++) {
				
				// Write row
				fits_write_img(pFile, TDOUBLE, firstElement, cols, pm, &status);
				if(status)
					return false;
				
				// Update matrix pointer
				pm += m->tda;
				
				// Update element count
				firstElement += cols;
				
			}
			
			return true;			
			
		} else
			return false;
		
	} else
		return false;
	
}

// Data available
bool MatrixFITSIO::isValid() {
	return pFile;
}

/*
// Close and re-open the file (otherwise it is not possible to write out the data; strange CFITSIO issue)
void MatrixFITSIO::reopen() {
	if(enableReopen) {
		fits_close_file(pFile, &status);
		fits_open_file(&pFile, fileName, READWRITE, &status);
	}
}
*/

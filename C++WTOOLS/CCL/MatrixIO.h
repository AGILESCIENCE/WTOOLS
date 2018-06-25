/*
 * MatrixIO.h
 *
 *  Created on: Jan 17, 2012
 *      Author: alessio
 */

#ifndef __MATRIX_IO_H__
#define __MATRIX_IO_H__

#include <stdio.h>
#include "fitsio.h"
#include <gsl/gsl_matrix.h>

class MatrixIO {

public:
	
	MatrixIO();
	MatrixIO(size_t rowSz, size_t colSz, size_t planeSz);
	virtual ~MatrixIO();

	// Get size
	size_t getRowSz() { return rows; };
	size_t getColSz() { return cols; };
	size_t getPlaneSz() { return planes; };

	// Input
	virtual bool read(gsl_matrix *m) = 0;

	// Output
	virtual bool write(gsl_matrix *m) = 0;

	// Data available for read/write
	virtual bool isValid() = 0;

protected:
	
	size_t		rows;
	size_t		cols;
	size_t		planes;

};

class MatrixFileIO : public MatrixIO {

public:
	
	typedef enum {
		None = 0x00,
		Byte,	
		Word,  
		Integer,
		Float,
		Double
	} DataType;

	MatrixFileIO(const char* fname);
	MatrixFileIO(const char* fname, size_t rowSz, size_t colSz, size_t planeSz, DataType dataType = Float);
	virtual ~MatrixFileIO();

	// Get the data type used in the file
	DataType getDataType() { return type; };
	
	// Input
	bool read(gsl_matrix *m);

	// Output
	bool write(gsl_matrix *m);

	// Data available
	bool isValid();
	
protected:

	FILE *pFile;
	DataType type;
	
	void readHeader();
	void writeHeader();
	
	bool readPlaneFloat(gsl_matrix *m);
	bool writePlaneFloat(gsl_matrix *m);
	
	bool readPlaneDouble(gsl_matrix *m);
	bool writePlaneDouble(gsl_matrix *m);
	
};
 
class MatrixFITSIO : public MatrixIO {
	
public:
	
	MatrixFITSIO(const char* fname);
	MatrixFITSIO(const char* fname, size_t rowSz, size_t colSz, size_t planeSz, int dataType = FLOAT_IMG);
	virtual ~MatrixFITSIO();
	
	// Get the data type used in the file
	int getDataType() { return type; };
	
	// Header manipulation
	void copyHeaderFrom(const MatrixFITSIO &inFile);
	void writeComment(const char *value);
	void writeKeyWord(const char *name, const char *value, const char *comment);
	void writeKeyWord(const char *name, double value, const char *comment);
	void writeKeyWord(const char *name, float value, const char *comment);
	void writeKeyWord(const char *name, int value, const char *comment);
	
	// Input
	bool read(gsl_matrix *m);
	
	// Output
	bool write(gsl_matrix *m);
	
	// Data available
	bool isValid();
	
protected:
	
	/*
	static const unsigned int fileNameSz = 256;
	char fileName[fileNameSz];
	bool enableReopen;
	*/
	
	fitsfile *pFile;
	int status;
	int type;
	size_t firstElement;
	
	// void reopen();
	
}; 

/*

class MatrixInMemoryIO : public MatrixIO {

};
 
class MatrixSharedMemoryIO : public MatrixIO {
 
};
 
*/

#endif // __MATRIX_IO_H__

#include <stdio.h>

#include "GetOptions.h"
#include "RealIsotropicWaveletFcn.h"
#include "WaveletScales.h"
#include "MatrixIO.h"
#include "RICWT.h"
#include <cstring>

using namespace std;

RealIsotropicWaveletFcn2* getWavelet(GetOptions& opt) {

    RealIsotropicWaveletFcn2* wavelet = NULL;

    if(strcmp(opt.waveletType.c_str(), "log") == 0)

		wavelet = new LaplacianOfGaussianWaveletFcn2();

    else {

		printf("ERROR : unsupported wavelet type %s\n", opt.waveletType.c_str());
		return 0;

	}

    wavelet->printInfo();

    return wavelet;

}

WaveletScales* getScales(GetOptions& opt) {

    WaveletScales *scales = NULL;

    if(strcmp(opt.scalesType.c_str(), "linear") == 0)

		scales = new LinearWaveletScales(opt.scalesSz, opt.scalesMin, opt.scalesMax);

    else if((strcmp(opt.scalesType.c_str(), "dyadic") == 0))

        scales = new ExpWaveletScales(opt.scalesSz, opt.scalesMin, opt.scalesMax);

    else {

		printf("ERROR : unsupported scale type %s\n", opt.scalesType.c_str());
		return 0;

	}

    scales->printInfo();

    return scales;

}

MatrixIO* getInputFile(GetOptions& opt) {

    MatrixIO *inFile = NULL;

    if (strcmp(opt.inFileType.c_str(), "mtx") == 0)
        inFile = new MatrixFileIO(opt.inFileName.c_str());
    else
        inFile = new MatrixFITSIO(opt.inFileName.c_str());

    if(!inFile->isValid()) {
		printf("ERROR : unable to open %s\n", opt.inFileName.c_str());
        delete inFile;
		return 0;
	}

	if(opt.verbose) {
		printf("Input\n");
		printf("\tfile: %s\n", opt.inFileName.c_str());
	}

    return inFile;

}

MatrixIO* getOutputFile(GetOptions& opt, MatrixIO *inFile, WaveletScales *scales) {

    MatrixIO *outFile = NULL;

    if (strcmp(opt.outFileType.c_str(), "mtx") == 0)
        if (opt.outDouble)
            outFile = new MatrixFileIO(opt.outFileName.c_str(), inFile->getRowSz(), inFile->getColSz(), scales->getSize(), MatrixFileIO::Double);
        else
            outFile = new MatrixFileIO(opt.outFileName.c_str(), inFile->getRowSz(), inFile->getColSz(), scales->getSize());
    else {

        MatrixFITSIO *outFileFITS = NULL;

        if (opt.outDouble)
            outFileFITS = new MatrixFITSIO(opt.outFileName.c_str(), inFile->getRowSz(), inFile->getColSz(), scales->getSize(), DOUBLE_IMG);
        else
            outFileFITS = new MatrixFITSIO(opt.outFileName.c_str(), inFile->getRowSz(), inFile->getColSz(), scales->getSize());

        if (strcmp(opt.inFileType.c_str(), "mtx") != 0)
            outFileFITS->copyHeaderFrom(dynamic_cast<MatrixFITSIO&>(*inFile));

        // Add wavelet specific keywords
        outFileFITS->writeComment("Wavelet transform specific parameters");

        outFileFITS->writeKeyWord("WTYPE", opt.waveletType.c_str(), "Wavelet type");

        outFileFITS->writeKeyWord("WSCALE", opt.scalesType.c_str(), "Wavelet scale");
        outFileFITS->writeKeyWord("WSIZE", opt.scalesSz, "Number of scales considered");
        outFileFITS->writeKeyWord("WSMIN", opt.scalesMin, "Min scale");
        outFileFITS->writeKeyWord("WSMAX", opt.scalesMax, "Max scale");

        for (int i = 0; i < scales->getSize(); i++) {
            char buff[8];
            sprintf(buff, "WS%03d", i+1);
            outFileFITS->writeKeyWord(buff, scales->at(i), NULL);
        }

        outFile = outFileFITS;

    }

    if (!outFile->isValid()) {
        printf("ERROR : unable to open %s\n", opt.outFileName.c_str());
        delete outFile;
		return 0;
    }

    if(opt.verbose) {
		printf("Output\n");
		printf("\tfile: %s\n", opt.outFileName.c_str());
	}

    return outFile;
}


int main (int argc, char * const argv[]) {

	printf("Real Isotropic Continuous Wavelet Trasform 2D - Version 2.1\n");

	GetOptions opt(argc, argv);

	if(opt.isValid()) {

        RealIsotropicWaveletFcn2 *wavelet = getWavelet(opt);
        if (wavelet) {

            WaveletScales *scales = getScales(opt);
            if (scales) {

                MatrixIO *inFile = getInputFile(opt);
                if (inFile) {

                    MatrixIO *outFile = getOutputFile(opt, inFile, scales);
                    if (outFile) {

                        printf("Running\n");

                        ricwt2(wavelet, scales, inFile, outFile);

                        printf("End");

                    }

                    delete outFile;

                }

                delete inFile;

            }

            delete scales;

        }

        // delete wavelet;

	}

	return 0;
}

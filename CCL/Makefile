CC=g++

#CFITSIOINCDIR=-I/usr/local_agile/heasoft-6.6.2-x86_64/x86_64-unknown-linux-gnu-libc2.3.4/include/
CFITSIOINCDIR=-I$(GTB_CFITSIO_INCLUDE)
#CFITSIOLIBDIR=-L/usr/local_agile/heasoft-6.6.2-x86_64/x86_64-unknown-linux-gnu-libc2.3.4/lib/
CFITSIOLIBDIR=-L$(GTB_CFITSIO_LIB)
CFITSIOLIB=-lcfitsio

#GSLINCDIR=-I/usr/local_agile/gsl-1.15/include/
GSLINCDIR=-I$(GSL)/include/
#GSLLIBDIR=-L/usr/local_agile/gsl-1.15/lib/
GSLLIBDIR=-L$(GSL)/lib/
GSLLIB=-lgsl -lgslcblas

CFLAGS=$(CFITSIOINCDIR) $(GSLINCDIR) -c -Wall -I. 

LD=$(CC)

LFLAGS=$(CFITSIOLIBDIR) $(CFITSIOLIB) $(GSLLIBDIR) $(GSLLIB) 

SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

EXECUTABLE=ccl

all:$(OBJECTS)
	$(LD) -o $(EXECUTABLE) $^ $(LFLAGS)

clean:
	rm -rf *~ *o cwt2

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS) 

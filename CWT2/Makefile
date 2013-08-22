CC=g++

CFITSIOINCDIR=-I$(CFITSIO)/include
CFITSIOLIBDIR=-L$(CFITSIO)/lib
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

EXECUTABLE=cwt2

all:$(OBJECTS)
	$(LD) -o $(EXECUTABLE) $^ $(LFLAGS)

clean:
	rm -rf *~ *o cwt2

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS) 

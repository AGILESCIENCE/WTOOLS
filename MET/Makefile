CC=g++

CFITSIOINCDIR=-I$(CFITSIO)/include
CFITSIOLIBDIR=-L$(CFITSIO)/lib
CFITSIOLIB=-lcfitsio

GSLINCDIR=-I$(GSL)/include/
GSLLIBDIR=-L$(GSL)/lib/
GSLLIB=-lgsl -lgslcblas

CFLAGS=$(CFITSIOINCDIR) $(GSLINCDIR) -c -Wall -I. 

LD=$(CC)

LFLAGS=$(CFITSIOLIBDIR) $(CFITSIOLIB) $(GSLLIBDIR) $(GSLLIB) 

SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

EXECUTABLE=met

all:$(OBJECTS)
	$(LD) -o $(EXECUTABLE) $^ $(LFLAGS)

clean:
	rm -rf *~ *o met

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS) 

CC=g++

CFITSIOINCDIR=-I$(CFITSIO)/include
CFITSIOLIBDIR=-L$(CFITSIO)/lib
CFITSIOLIB=-lcfitsio

#GSLINCDIR=-I$(GSL)/include/
#GSLLIBDIR=-L$(GSL)/lib/
GSLINCDIR=-I../lib-gsl/include/
GSLLIBDIR=-L../lib-gsl/lib/
GSLLIB=-lgsl -lgslcblas

# Prefix for each installed program. Only ABSOLUTE PATH
EXEC_PREFIX=$(prefix)
# The directory to install the binary files in.
BINDIR=$(EXEC_PREFIX)/bin


CFLAGS=$(CFITSIOINCDIR) $(GSLINCDIR) -c -Wall -I.
LFLAGS=$(CFITSIOLIBDIR) $(CFITSIOLIB) $(GSLLIBDIR) $(GSLLIB)


SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

EXECUTABLE=cwt2

all: install

install: $(OBJECTS)
	$(CC) -o $(EXECUTABLE) $^ $(LFLAGS)
	test -d $(BINDIR) || mkdir -p $(BINDIR)
	cp -f -r -p $(EXECUTABLE) $(BINDIR)

clean:
	rm -rf *~ *o ccl

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

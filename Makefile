CXX = c++
CXXFLAGS = -std=c++17 -g
OBJECTS = \
	program.o

CPP_FILES = \
	program.cpp \
	bin.cpp \
	dic.cpp \
	rle.cpp \
	dif.cpp \
	for.cpp

all: program

program: program.o
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS)

program.o: $(CPP_FILES)
	$(CXX) $(CXXFLAGS) -c program.cpp

clean:
	rm -f program program.o

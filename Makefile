CXX = c++
CXXFLAGS = -std=c++17
OBJECTS = \
	main.o

all: main

main: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f main
	rm -f $(OBJECTS)

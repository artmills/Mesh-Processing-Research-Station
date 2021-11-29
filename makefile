#CXXFLAGS=-std=c++17 -O3
CXXFLAGS=-std=c++17 -g

OBJDIR=obj

SOURCES=main.cpp vertex.cpp meshcomponent.cpp loader.cpp shaderprogram.cpp basicshader.cpp perlinnoise.cpp shadowshader.cpp geometry.cpp polyhedron.cpp meshanalysis.cpp subdivision.cpp smoothing.cpp view.cpp meshfactory.cpp mousepicker.cpp

OBJECTS=$(patsubst %.cpp,$(OBJDIR)/%.o,$(SOURCES))
#LLIBS=$(shell pkg-config --cflags --libs libglut)
LLIBS=-lGL -lGLEW -lGLU /usr/lib64/libglut.so -lm

build: $(OBJECTS)
	g++ $(CPPFLAGS) $(LLIBS) -o build $(OBJECTS) 

$(OBJECTS): | obj

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/%.o: %.cpp %.hpp
	g++ $(CPPFLAGS) -c $< -o $@

$(OBJDIR)/main.o: main.cpp
	g++ $(CPPFLAGS) -c main.cpp -o $(OBJDIR)/main.o

.PHONY : clean
clean:
	rm build $(OBJECTS)

# set this variable to the director in which you saved the common files
commondir = ../common/
TARGETS = $(shell find -name "lab?-?.cpp" | sort)

all: $(TARGETS:.cpp=)

%: %.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c
	gcc -Wall -ggdb -o $@ -I$(commondir) -I../common/Linux -DGL_GLEXT_PROTOTYPES $< $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c -lXt -lX11 -lGL -lm -lstdc++

clean :
	rm lab?-?

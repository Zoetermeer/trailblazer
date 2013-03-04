SRCS = $(wildcard *.cpp)
INCS = $(wildcard *.hpp)
TESTSRCS = $(wildcard tests/*.cpp)
OBJS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
TESTOBJS = $(OBJS) $(patsubst %.cpp, %.o, $(wildcard tests/*.cpp))
GLFW = ./glfw-2.7.7
CXX = c++
CPPFLAGS = -g -O0 -I./glm-0.9.4.2 -I./glfw-2.7.7/include -std=c++11 -stdlib=libc++
TESTFLAGS = -DTEST
LDFLAGS = -L. -stdlib=libc++ -lglfw -framework GLUT -framework Cocoa -framework OpenGL -framework IOKit

all: $(OBJS)
	make cocoa --directory=$(GLFW)
	cp $(GLFW)/lib/cocoa/libglfw.a .;
	$(CXX) $(LDFLAGS) -o world $(OBJS)

test_compile: $(SRCS) $(TESTSRCS)
	$(CXX) -c $(CPPFLAGS) $(TESTFLAGS) $^

test: test_compile $(TESTOBJS)
	$(CXX) $(LDFLAGS) -ldl -lcppunit -o test $(TESTOBJS)

check:
	${MAKE} test
	./test
	${MAKE} clean
	${MAKE} all

clean: 
	rm -f *.o
	rm -f ./tests/*.o
	rm -d -r -f test.dSYM
	rm -f test
	rm -f world 

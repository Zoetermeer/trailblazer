SRCS = $(wildcard *.cpp)
INCS = $(wildcard *.hpp)
TESTSRCS = $(wildcard tests/*.cpp)
OBJS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
TESTOBJS = $(OBJS) $(patsubst %.cpp, %.o, $(wildcard tests/*.cpp))
GLFW = ./glfw-2.7.7
LIBNOISE_SRCS = ./libnoise-1.0.0/noise/src
CXX = c++
CPPFLAGS = -g -O0 -DRUN_TESTS -DGTEST_USE_OWN_TR1_TUPLE=1 -I$(GTEST)/include -I./glm-0.9.4.2 -I./glfw-2.7.7/include -I./libnoise-1.0.0/noise/src -std=c++11 -stdlib=libc++
LDFLAGS = -L. -L./$(GTEST)/ -stdlib=libc++ -lgtest -lglfw -lnoise -framework GLUT -framework Cocoa -framework OpenGL -framework IOKit
GTEST = ./gtest-1.6.0

all: $(OBJS)
	make cocoa --directory=$(GLFW)
	cp $(GLFW)/lib/cocoa/libglfw.a .;
	make --directory=$(LIBNOISE_SRCS)
	cp $(LIBNOISE_SRCS)/libnoise.a .;
	$(CXX) $(LDFLAGS) -o world $(OBJS)

test_compile: $(SRCS) $(TESTSRCS)
	make cocoa --directory=$(GLFW)
	cp $(GLFW)/lib/cocoa/libglfw.a .;
	make --directory=$(LIBNOISE_SRCS)
	cp $(LIBNOISE_SRCS)/libnoise.a .;
	$(CXX) -c $(CPPFLAGS) $(TESTFLAGS) $^

test: $(TESTOBJS)
	make cocoa --directory=$(GLFW)
	cp $(GLFW)/lib/cocoa/libglfw.a .;
	make --directory=$(LIBNOISE_SRCS)
	cp $(LIBNOISE_SRCS)/libnoise.a .;
	$(CXX) $(LDFLAGS) -lgtest -o test $(TESTOBJS)

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
	rm -f libglfw.a
	rm -f libnoise.a

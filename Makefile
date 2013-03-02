SRCS = $(wildcard *.cpp)
INCS = $(wildcard *.hpp)
TESTSRCS = $(wildcard tests/*.cpp)
OBJS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
TESTOBJS = $(OBJS) $(patsubst %.cpp, %.o, $(wildcard tests/*.cpp))
CXX = c++
CPPFLAGS = -g -O0 -I./glm-0.9.4.2 -std=c++11
TESTFLAGS = -DTEST
LDFLAGS = -framework GLUT -framework OpenGL

all: $(OBJS)
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

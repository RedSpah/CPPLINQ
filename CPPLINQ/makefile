CFLAGS = -I.
DEPS = cpplinq.h

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CFLAGS)

cpplinqmake: testing.cpp
	$(CXX) -o test testing.o -I.

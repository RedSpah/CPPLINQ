CFLAGS = -std=c++11 -Wall -Wextra 

cpplinqmake: testing.cpp
	$(CXX) -o test testing.cpp -I. $(CFLAGS)

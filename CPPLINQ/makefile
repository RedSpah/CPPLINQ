
cpplinqmake: testing.cpp
ifeq ($(CXX),g++)
	$(CXX) -o test testing.cpp -I. -Wall -Wextra -std=c++0x -fmax-errors=64
else
	$(CXX) -o test testing.cpp -I. -Wall -Wextra -std=c++11 -ferror-limit=64
endif
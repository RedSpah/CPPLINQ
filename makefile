
cpplinqmake: testing.cpp
	$(CXX) -o test testing.cpp -I. -Wall -Wextra  $(CPPVERFLAG)
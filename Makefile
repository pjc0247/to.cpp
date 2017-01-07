build :
	gcc tests.cpp -o tests -std=c++0x
  
test : build
	./tests

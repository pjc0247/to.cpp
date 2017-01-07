build :
	clang tests.cpp -o tests
  
test : build
	./tests

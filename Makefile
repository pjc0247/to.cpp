build :
	cc tests.cpp -o tests
  
test : build
	./tests

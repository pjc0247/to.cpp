build :
	gcc tests.cpp -o tests
  
test : build
	./tests

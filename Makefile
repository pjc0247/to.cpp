build :
  gcc test.cpp -o test
  
test : build
  ./test

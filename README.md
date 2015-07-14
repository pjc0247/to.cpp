to.cpp
====

to::_types
----
  ```c++
  std::cout<< to::_int("4132"); // 4132
  std::cout<< to::_unsigned_int("9999999999999999999999999999"); // out_of_range exception
  ```
  
  * Supported Types
    * int
    * 
  
  
to::_string
----

  __Fundamental types__
  ```c++
  int v = 1234;
  std::cout<< to::_string(v); // 1234
  std::cout<< to::_string(4.44f); // 4.44
  
  int *ptr = &v;
  std::cout<< to::_string(v); // #<int * 00001234>
  
  bool b = true;
  std::cout<< to::_string(b); // true
  ```
  __STL Containers__
  ```c++
  std::vector<int> v({1,2,3,4});
  std::cout<< to::_string(v); // [1, 2, 3, 4]
  
  std::map<std::string, std::string> m({
    {"name", "park"},
    {"chicken", "good"}});
  std::cout<< to::_string(m); // {name : park, chicken : good}
  
  std::map<std::string, std::vector<std::string>> m2({
    {"fruits", {"apple", "banana", "orange"}},
  	{"colors", {"yellow", "red", "blue"}}});
  std::cout<< to::_string(m2); // {fruits : [apple, banana, orange], colors : [yellow, red, blue]}
  
  std::string str = "hello world";
  std::cout<< to::_string(str); // hello world
  
  std::cout<< to::_string({1,2,3,4}); // [1, 2, 3, 4]
  std::cout<< to::_string({{1,2}, {3,4}}); // not implemented yet
  ```
  __Custom Classes__
  ```c++
  class Foo{
  public:
    int v;
    
    std::string to_string(){
      return "#<Foo v:" + to::_string(v) + ">";
    }
  }
  
  Foo f;
  f.v = 15;
  
  std::cout<< to::_string(f); // #<Foo v:15>
  ```
  ```c++
  Bar b;
  std::cout<< to::_string(b); // #<Bar 00001234>
  ```
  __is_string_convertible__
  ```c++
  std::cout<< to::is_string_convertible<int>::value; // true
  
  std::cout<< to::is_string_convertible<Foo>::value; // true
  std::cout<< to::is_string_convertible<Bar>::value; // false
  ```

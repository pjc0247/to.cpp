to.cpp
====

to::_types
----
  ```c++
  std::cout<< to::_int("4132"); // 4132
  std::cout<< to::_unsigned_int("9999999999999999999999999999"); // out_of_range exception
  ```
  
  * __Supported Types__
    * signed/unsigned int
    * signed/unsigned long
    * 나중에 추가
  
to::_string
----

  __Fundamental types__
  ```c++
  int v = 1234;
  std::cout<< to::_string(v); // 1234
  std::cout<< to::_string(4.44f); // 4.44
  
  bool b = true;
  std::cout<< to::_string(b); // true
  
  int *ptr = &v;
  std::cout<< to::_string(v); // #<int * 00001234>
  
  // char *, const char * 타입은 포인터로 취급되지 않고 문자열로 취급됩니다.
  char *s = "hello world";
  std::cout<< to::_string(s); // hello world
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

Options
----
* __TO_STRING_WITH_QMARKS__
  std::string, char *, const char *형에 대해 to::_string을 수행할 때 문자열의 앞뒤로 __"__를 삽입합니다.<br>
  이 옵션을 활용하면 STL 컨테이너에서 json 스트링을 빌드할 수 있습니다.

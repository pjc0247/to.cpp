#define CATCH_CONFIG_MAIN

#include "catch.h"
#include "to.h"

TEST_CASE("to::_int / to::_unsigned_int", "to::int") {
	
	SECTION("기본 형 변환") {
		REQUIRE(to::_int("1234") == 1234);
		REQUIRE(to::_unsigned_int("-1234") == -1234);
	}
	SECTION("형 변환 실패") {
		REQUIRE_THROWS_AS(
			to::_int("2147483648"),
			std::out_of_range);
		REQUIRE_THROWS_AS(
			to::_int("abcd"),
			std::invalid_argument);

		REQUIRE_THROWS_AS(
			to::_unsigned_int("-99999999999999999999"),
			std::out_of_range);
		REQUIRE_THROWS_AS(
			to::_unsigned_int("abcd"),
			std::invalid_argument);
	}
}

class foo {
public:
	int v;

	std::string to_string() const {
		return "#<foo v:" + to::_string(v) + ">";
	};
};
class bar {
public:
	int v;
};

TEST_CASE("to::_string", "to::_string") {

	SECTION("기본 형 변환") {
		REQUIRE(to::_string(1234) == "1234");

		int v = 99;
		REQUIRE(to::_string(v) == "99");

		int n[] = {1,2,3,4};
		REQUIRE(to::_string(n) == "[1, 2, 3, 4]");

		char str_ary[] = "HelloWorld";
		REQUIRE(to::_string(str_ary) == "HelloWorld");

		char *str_ptr = "HelloWorld_2";
		REQUIRE(to::_string(str_ptr) == "HelloWorld_2");
	}

	SECTION("STL Container 변환") {
		REQUIRE(to::_string({ 1,2,3,4 }) == "[1, 2, 3, 4]");

		std::vector<int> v({5,6,7,8});
		REQUIRE(to::_string(v) == "[5, 6, 7, 8]");

		std::map<std::string,std::string> m({
			{"key", "value"}, {"a", "b"} });
#if defined(TO_STRING_WITH_QMARKS)
		REQUIRE(to::_string(m) == "{\"a\" : \"b\", \"key\" : \"value\"}");
#else
		REQUIRE(to::_string(m) == "{key : value, a : b}");
#endif
		
		std::map<std::string,std::vector<std::string>> m2({
			{"color", {"red", "blue", "green"}},
			{"numbers", {"1", "2", "3"}} });
#if defined(TO_STRING_WITH_QMARKS)
		REQUIRE(to::_string(m2) == "{\"color\" : [\"red\", \"blue\", \"green\"], \"numbers\" : [\"1\", \"2\", \"3\"]}");
#else
		REQUIRE(to::_string(m2) == "{color : [red, blue, green], numbers : [1, 2, 3]}");
#endif
	}

	SECTION("커스텀 클래스 변환") {
		foo f;
		f.v = 15;
		REQUIRE(to::_string(f) == "#<foo v:15>");
	}
}

TEST_CASE("to::has_string_converter", "to::has_string_converter") {

	SECTION("커스텀 클래스") {
		REQUIRE(to::is_string_convertible<foo>::value == 1);
		REQUIRE(to::is_string_convertible<bar>::value == 0);
	}
}

#pragma once

#include <string>
#include <iostream>
#include <type_traits>
#include <vector>
#include <stack>
#include <deque>
#include <queue>
#include <list>
#include <forward_list>
#include <map>
#include <unordered_map>
#include <sstream>

#define TO_STL_STR_WITH_QUATO

#if __cplusplus > 199711L || _MSC_VER >= 11900
#define HAS_NOEXCEPT
#else
#define noexcept throw()
#endif

// TO::_TYPES
namespace to{
	int _int(const std::string &v) throw(std::out_of_range, std::invalid_argument){
		return stoi(v); 
	}
	unsigned int _unsigned_int(const std::string &v) throw(std::out_of_range, std::invalid_argument){
		auto lresult = std::stoul(v);
		unsigned int iresult = lresult;
		if(lresult != iresult) throw std::out_of_range("out of range");
		return iresult;
	}
	long _long(const std::string &v) throw(std::out_of_range, std::invalid_argument){
		return std::stoi(v);
	}
	unsigned long _unsigned_long(const std::string &v) throw(std::out_of_range, std::invalid_argument){
		return std::stoul(v);
	}
};

// TO::_STRING
namespace to{
	template<typename T>
	struct is_initializer_list{
		enum{value = false};
	};

	template<typename T>
	struct is_initializer_list<std::initializer_list<T>>{
		enum{value = true};
	};

	template <typename T>
	struct get_elem_type{
	};

	template <typename T>
	struct get_elem_type<std::vector<T>>{
		typedef T type;
	};

	template <template <typename ELEM, typename = std::allocator<ELEM>> class CONTAINER>
	struct get_container_type{
	};

	/*
	template <>
	struct get_container_type<std::vector>{
		typedef std::vector type;
	};
	*/

	class embed_cvts{
	public:
	
		template <typename T>//, typename C = char>//, template <typename T, typename = std::allocator<T>> class C>
		struct cvt{
			/* nothing -> deduction failure */
		};
	
	#define SINGLE_ELEM_ITERATABLE_CVT(type) \
		template <typename T> \
		struct cvt<type<T>>{ \
			static std::string to_string(type<T> &n){ \
				std::string result; \
				for(auto &v : n){ \
					if(!result.empty()) \
						result.append( ", " ); \
					result.append( to::_string(v) ); \
				} \
				return "[" + result + "]"; \
			} \
		};

	#define PAIR_ITERATABLE_CVT(type) \
		template <typename T, typename C> \
		struct cvt<type<T,C>>{ \
			static std::string to_string(type<T,C> &n){ \
				std::string result; \
				for(auto &p : n){ \
					if(!result.empty()) \
						result.append( ", " ); \
					result.append( to::_string(p.first) ); \
					result.append( " : " ); \
					result.append( to::_string(p.second) ); \
				} \
				return "{" + result + "}"; \
			} \
		};

		SINGLE_ELEM_ITERATABLE_CVT(std::initializer_list)
		SINGLE_ELEM_ITERATABLE_CVT(std::vector)
		SINGLE_ELEM_ITERATABLE_CVT(std::deque)
		SINGLE_ELEM_ITERATABLE_CVT(std::stack)
		SINGLE_ELEM_ITERATABLE_CVT(std::list)
		SINGLE_ELEM_ITERATABLE_CVT(std::forward_list)

		PAIR_ITERATABLE_CVT(std::map)
		PAIR_ITERATABLE_CVT(std::unordered_map)

		template <>
		struct cvt<std::string>{
			static std::string to_string(const std::string &str){
				return str;
			}
		};

	#define _FUNDAMENTAL_CVT(type) \
		template <> \
		struct cvt<type>{ \
			static std::string to_string(type n){ \
				std::ostringstream s; \
				s<< n; \
				return s.str(); \
			} \
		};

		_FUNDAMENTAL_CVT(char)
		_FUNDAMENTAL_CVT(unsigned char)
		_FUNDAMENTAL_CVT(short)
		_FUNDAMENTAL_CVT(unsigned short)
		_FUNDAMENTAL_CVT(int)
		_FUNDAMENTAL_CVT(unsigned int)
		_FUNDAMENTAL_CVT(long)
		_FUNDAMENTAL_CVT(unsigned long)
		_FUNDAMENTAL_CVT(long long)
		_FUNDAMENTAL_CVT(unsigned long long)
		_FUNDAMENTAL_CVT(float)
		_FUNDAMENTAL_CVT(double)

		template <>
		struct cvt<bool>{
			static std::string to_string(bool n){
				return n ? "true" : "false";
			}
		};
		template <>
		struct cvt<const char*>{
			static std::string to_string(const char *n){
				return std::string(n);
			}
		};
		template <>
		struct cvt<char*>{
			static std::string to_string(char *n){
				return std::string(n);
			}
		};
	};

	template <typename T> 
	struct default_convertible{
		typedef char yes;
		typedef long no;

		template <typename C> static yes test( decltype(&embed_cvts::cvt<C>::to_string) ) ;
		template <typename C> static no test(...);

	public:
		enum{value = sizeof(test<T>(0)) == sizeof(yes)};
	};

	template <typename T>
	struct has_to_string{
	private:
		template <typename T, bool A> 
		struct get_to_string_ret_type{
			enum{value = false};
		};
		template <typename T> 
		struct get_to_string_ret_type<T, true>{
			enum{value = std::is_same<std::string,  decltype(((T*)nullptr)->to_string()) >::value};
		};

		typedef char yes;
		typedef long no;

		template <typename C> static yes test( decltype(&C::to_string) ) ;
		template <typename C> static no test(...);

	public:
	   /* to_string 메소드가 있는지 검사한다 ->
		* to_string 메소드 리턴형이 std::string인지 검사한다. */
		enum{value = get_to_string_ret_type<Hello, sizeof(test<T>(0)) == sizeof(yes) >::value};
	};


	template <typename T>
	struct is_string_convertible {
		enum{value = has_to_string<T>::value || default_convertible<T>::value};
	};


	/* to_string메소드가 있는 경우 */
	template <typename T>
	typename std::enable_if<has_to_string<T>::value, std::string>::type
	_string(T o) noexcept{
		return o.to_string();
	}

	/* to_string이 없지만, 기본형 타입 변환이 가능한 경우 */
	template <typename T>
	typename std::enable_if<(!has_to_string<T>::value) &&
		default_convertible<T>::value &&
		(!is_initializer_list<T>::value), std::string>::type
	_string(T o) noexcept{
		return embed_cvts::cvt<T>::to_string(o);
	}
	/* std::initializer_list인 경우 */
	template <typename T>
	typename std::enable_if<(!has_to_string<std::initializer_list<T>>::value) &&
		default_convertible<std::initializer_list<T>>::value, std::string>::type
	_string(std::initializer_list<T> o) noexcept{
		return embed_cvts::cvt<std::initializer_list<T>>::to_string(o);
	}
	/* std::initializer_list인 경우 */
	template <typename T>
	typename std::enable_if<(!has_to_string<std::initializer_list<T>>::value) &&
		default_convertible<std::initializer_list<T>>::value, std::string>::type
	_string(std::initializer_list<std::initializer_list<T>> o) noexcept{
		return embed_cvts::cvt<std::initializer_list<std::initializer_list<T>>>::to_string(o);
	}

	/* to_string이 없고, 기본형 타입도 아닌 경우 */
	template <typename T>
	typename std::enable_if<(!has_to_string<T>::value) &&
		(!default_convertible<T>::value) &&
		(!std::is_pointer<T>::value), std::string>::type
	_string(T o) noexcept{
		char tmp[512];
		sprintf_s(tmp, "#<%s %p>", typeid(T).name(), &o);
		return tmp;
	}

	/* 포인터 타입인 경우 */
	template <typename T>
	typename std::enable_if<(!has_to_string<T>::value) &&
		(!default_convertible<T>::value) &&
		std::is_pointer<T>::value, std::string>::type
	_string(T o) noexcept{
		char tmp[512];
		sprintf_s(tmp, "#<%s %p>", typeid(T).name(), o);
		return tmp;
	}
}

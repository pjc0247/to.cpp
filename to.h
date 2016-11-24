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
#include <tuple>
#include <memory>

#if __cplusplus > 199711L || _MSC_VER >= 1900
#define HAS_NOEXCEPT
#else
#define noexcept throw()
#endif

#define CREATE_TO_STRING(...) \
    std::string to_string() const { \
        auto raw_values = std::make_tuple(__VA_ARGS__); \
        constexpr size_t tuple_size = std::tuple_size<decltype(raw_values)>::value; \
        auto values = to::value_builder<tuple_size>::build(raw_values); \
        char keys[] = #__VA_ARGS__; \
        return \
            "#<" + std::string(typeid(*this).name()) + " " + \
            to::tostring_builder<tuple_size - 1, sizeof(keys)>::build(keys, values) + \
            ">";\
    }

// TO::INTERNAL
namespace to {

    template <size_t I, size_t SIZE>
    struct tostring_builder {
        static std::string build(const char (&keys)[SIZE], const std::vector<std::string> &values) {
            return 
                tostring_builder<I - 1, SIZE>::build(keys, values) + ", " +
                to::nth_token(I, keys) + " : " + values[values.size() - I - 1];
        }
    };
    template <size_t SIZE>
    struct tostring_builder<0, SIZE> {
        static std::string build(const char (&keys)[SIZE], const std::vector<std::string> &values) {
            return to::nth_token(0, keys) + " : " + values[values.size()-1];
        }
    };

    template <size_t I>
    struct value_builder {
        template <class... TARGS, class... ARGS>
        static std::vector<std::string> build(const std::tuple<TARGS...>& targ, ARGS&&... args) {
            auto vec = value_builder<I - 1>::build(targ, std::get<I - 1>(targ), std::forward<ARGS>(args)...);
            vec.push_back(to::_string(std::get<I - 1>(targ)).c_str());
            return vec;
        }
    };
    template <>
    struct value_builder<0> {
        template <class... TARGS, class... ARGS>
        static std::vector<std::string> build(const std::tuple<TARGS...>& targ, ARGS&&... args) {
            return std::vector<std::string>();
        }
    };

    constexpr unsigned short _make_16(unsigned char a, unsigned char b) {
        return ((unsigned short)((a & 0xff) | ((b & 0xff) << 8)));
    }
    constexpr unsigned char _lo_8(unsigned short a) {
        return ((unsigned char)(a & 0xff));
    }
    constexpr unsigned char _hi_8(unsigned short a) {
        return ((unsigned char)(a >> 8) & 0xff);
    }

    template <size_t I, size_t OFFSET>
    struct tokenizer {
        constexpr static int find(int target, const char a[]) {
            return
                a[I] == ',' ?
                    target == 0 ?
                        _make_16(I + 1, OFFSET - I) :
                        tokenizer<I - 1, I - 1>::find(target - 1, a)
                        :
                    tokenizer<I - 1, OFFSET>::find(target, a);
        }
    };
    template <size_t OFFSET>
    struct tokenizer<0, OFFSET> {
        constexpr static int find(int target, const char a[]) {
            return _make_16(0, OFFSET + 1);
        }
    };
    template <size_t SIZE>
    inline std::string nth_token(int target, const char(&a)[SIZE]) {
        int ret = tokenizer<SIZE - 2, SIZE - 2>::find(target, a);
        return std::string(a + _lo_8(ret), _hi_8(ret));
    }
}

// TO::_TYPES
namespace to {
#pragma warning (push)
#pragma warning (disable : 4290)
    int _int(const std::string &v) throw(std::out_of_range, std::invalid_argument) {
        return stoi(v);
    }
    unsigned int _unsigned_int(const std::string &v) throw(std::out_of_range, std::invalid_argument) {
        auto lresult = std::stoul(v);
        unsigned int iresult = lresult;
        if (lresult != iresult) throw std::out_of_range("out of range");
        return iresult;
    }
    long _long(const std::string &v) throw(std::out_of_range, std::invalid_argument) {
        return std::stoi(v);
    }
    unsigned long _unsigned_long(const std::string &v) throw(std::out_of_range, std::invalid_argument) {
        return std::stoul(v);
    }
#pragma warning (pop)
};

// TO::_STRING
namespace to {
    template<typename T>
    struct is_initializer_list {
        enum { value = false };
    };
    template<typename T>
    struct is_initializer_list<std::initializer_list<T>> {
        enum { value = true };
    };

    class embed_cvts {
    public:

        template <typename T>//, typename C = char>//, template <typename T, typename = std::allocator<T>> class C>
        struct cvt {
            /* nothing -> deduction failure */
        };

#define SINGLE_ELEM_ITERATABLE_CVT(type) \
    template <typename T> \
    struct cvt<type<T>>{ \
      static std::string to_string(const type<T> &n){ \
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
      static std::string to_string(const type<T,C> &n){ \
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
        struct cvt<std::string> {
            static std::string to_string(const std::string &str) {
#if defined(TO_STRING_WITH_QMARKS)
                return '"' + str + '"';
#else
                return str;
#endif
            }
        };

#define _FUNDAMENTAL_CVT(type) \
    template <> \
    struct cvt<type>{ \
      static std::string to_string(const type n){ \
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

        /* 배열형 */
        /*   TODO : stl iteratable이랑 합치기 */
        template <typename T, size_t LEN>
        struct cvt<T[LEN]> {
            static std::string to_string(T(&n)[LEN]) {
                std::string result;
                for (auto &v : n) {
                    if (!result.empty())
                        result.append(", ");
                    result.append(to::_string(v));
                }
                return "[" + result + "]";
            }
        };

        template <>
        struct cvt<bool> {
            static std::string to_string(bool n) {
                return n ? "true" : "false";
            }
        };

        template <size_t LEN>
        struct cvt<char[LEN]> {
            static std::string to_string(char(&n)[LEN]) {
                return std::string(n);
            }
        };
        template <size_t LEN>
        struct cvt<const char[LEN]> {
            static std::string to_string(const char(&n)[LEN]) {
                return std::string(n);
            }
        };
        template <>
        struct cvt<char*> {
            static std::string to_string(char *n) {
                return std::string(n);
            }
        };
        template <>
        struct cvt<const char*> {
            static std::string to_string(const char *n) {
                return std::string(n);
            }
        };

        template <typename T>
        struct cvt<std::shared_ptr<T>> {
            static std::string to_string(const std::shared_ptr<T> &ptr) {
                char fmt[128];
                sprintf_s(fmt, "#<shared_ptr use_count: %d data: ",
                    ptr.use_count());
                return fmt + to::_string(*(ptr.get())) + ">";
            }
        };
    };

    template <typename T>
    struct embed_convertible {
        typedef char yes;
        typedef long no;

        template <typename C> static yes test(decltype(&embed_cvts::cvt<C>::to_string));
        template <typename C> static no test(...);

    public:
        enum { value = sizeof(test<T>(0)) == sizeof(yes) };
    };

    template <typename T>
    struct has_to_string {
    private:
        template <typename T, bool A>
        struct is_ret_string {
            enum { value = false };
        };
        template <typename T>
        struct is_ret_string<T, true> {
            enum { value = std::is_same<std::string, decltype(((T*)nullptr)->to_string()) >::value };
        };

        typedef char yes;
        typedef long no;

        template <typename C> static yes test(decltype(&C::to_string));
        template <typename C> static no test(...);

    public:
        /* to_string 메소드가 있는지 검사한다 ->
        * to_string 메소드 리턴형이 std::string인지 검사한다. */
        enum { value = is_ret_string<T, sizeof(test<T>(0)) == sizeof(yes) >::value };
    };

    template <typename T>
    struct has_string_converter {
        enum { value = has_to_string<T>::value || embed_convertible<T>::value };
    };

    /* TODO : 조건 정리 */

#define TO_INTERNAL_PARAM const char *unused = nullptr

    /* to_string메소드가 있는 경우 */
    template <typename T>
    typename std::enable_if<
        has_to_string<T>::value,
        std::string>::type
        _string(const T &o, TO_INTERNAL_PARAM) noexcept {
        return o.to_string();
    }

    /* to_string이 없지만, 기본형 타입 변환이 가능한 경우 */
    template <typename T>
    typename std::enable_if<
        (!has_to_string<T>::value) &&
        embed_convertible<T>::value &&
        (!is_initializer_list<T>::value),
        std::string>::type
        _string(const T &o, TO_INTERNAL_PARAM) noexcept {
        return embed_cvts::cvt<T>::to_string(o);
    }
    template <typename T>
    typename std::enable_if<
        (!has_to_string<T>::value) &&
        embed_convertible<T>::value &&
        (!is_initializer_list<T>::value),
        std::string>::type
        _string(T &o, TO_INTERNAL_PARAM) noexcept {
        return embed_cvts::cvt<T>::to_string(o);
    }

    /* 배열 */
    template <typename T, size_t LEN>
    typename std::enable_if<
        (!has_to_string<T>::value) &&
        embed_convertible<T>::value &&
        (!is_initializer_list<T>::value),
        std::string>::type
        _string(T(&o)[LEN], TO_INTERNAL_PARAM) noexcept {

        return embed_cvts::cvt<T[LEN]>::to_string(o);
    }

    /* std::initializer_list인 경우 */
    template <typename T>
    typename std::enable_if<
        (!has_to_string<std::initializer_list<T>>::value) &&
        embed_convertible<std::initializer_list<T>>::value,
        std::string>::type
        _string(const std::initializer_list<T> &o, TO_INTERNAL_PARAM) noexcept {
        return embed_cvts::cvt<std::initializer_list<T>>::to_string(o);
    }

    /* enum */
    template <typename T>
    typename std::enable_if<
        std::is_enum<T>::value,
        std::string>::type
        _string(const T o, const char *name) noexcept {

		if (name != nullptr)
			return std::string(name);

        char tmp[512];
        sprintf_s(tmp, "#<%s %d>", typeid(T).name(), o);
        return tmp;
    }

    /* to_string이 없고, 기본형 타입도 아닌 경우 */
    template <typename T>
    typename std::enable_if<
        (!has_to_string<T>::value) &&
        (!embed_convertible<T>::value) &&
        (!std::is_enum<T>::value) && 
        (!std::is_pointer<T>::value && !std::is_same<std::nullptr_t, T>::value),
        std::string>::type
        _string(const T &o, TO_INTERNAL_PARAM) noexcept {
        char tmp[512];
        sprintf_s(tmp, "#<%s %p>", typeid(T).name(), &o);
        return tmp;
    }

    /* 포인터 타입인 경우 */
    template <typename T>
    typename std::enable_if<
        (!has_to_string<T>::value) &&
        (!embed_convertible<T>::value) &&
        (std::is_pointer<T>::value || std::is_same<std::nullptr_t, T>::value) &&
        (!std::is_same<T, char*>::value),
        std::string>::type
        _string(T o, TO_INTERNAL_PARAM) noexcept {
        char tmp[512];
        sprintf_s(tmp, "#<%s %p>", typeid(T).name(), o);
        return tmp;
    }
}

#define to_s(x) to::_string(x, #x)

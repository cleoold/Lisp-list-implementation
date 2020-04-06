// -std=c++17 or /std:c++17 (inline variable, fold expression)
#pragma once
#include <iostream>

namespace scm {
    #define typ typename

    // node of cons list
    template<typ T, T...> struct item;
    template <typ T, T n> struct item<T, n> {
        using type = item;
        using value_type = T;
        static constexpr T value = n;
    };
    template<typ T> struct item<T> {
        using type = item;
        using value_type = T;
    };

    template<typ ...Tn> struct list {
        using type = list;
        static constexpr size_t size = sizeof...(Tn);
    };
    using null = list<>;

    template<typ, typ> struct cons;
    template<typ I, typ ...In> struct cons<I, list<In...>> : list<I, In...> {};
    template<typ I, typ L> using cons_t = typ cons<I, L>::type;

    // first item of the list
    template<typ> struct first;
    template<typ I, typ ...In> struct first<list<I, In...>> : I {};
    // returns item<...> itself
    template<typ L> using first_t = typ first<L>::type;
    // value of first item if it has one
    template<typ L> inline constexpr typ first<L>::value_type first_v = first<L>::value;
    // type content of first item, used for accessing nested list
    template<typ L> using first_vt = typ first<L>::value_type;

    // rest of the list
    template<typ> struct rest;
    template<typ I, typ ...In> struct rest<list<I, In...>> : list<In...> {};
    template<typ L> using rest_t = typ rest<L>::type;

    // gets nth element type
    template<typ L, size_t i> struct get : get<typ rest<L>::type, i-1> {};
    template<typ L> struct get<L, 0> : first<L> {};
    template<typ L, size_t i> using get_t = typ get<L, i>::type;
    template<typ L, size_t i> inline constexpr typename get<L, i>::value_type get_v = get<L, i>::value;
    template<typ L, size_t i> using get_vt = typ get<L, i>::value_type;

    namespace effects {
        template<typ VT, VT v> static inline void _print_item(item<VT, v>) { std::cout << v; }
        template<typ VT> static inline void _print_item(item<VT>) { std::cout << typeid(VT).name(); }
        static inline void _print_item(item<null>) { std::cout << "()"; }
        template<typ ...In> static inline void _print_item(item<list<In...>>);
        template<typ I, typ ...In> static inline void _print_impl(list<I, In...>) {
            std::cout << '(', _print_item(I());
            ((std::cout << ", ", _print_item(In())),...);
            std::cout << ')';
        }
        template<typ ...In> static inline void _print_item(item<list<In...>>) { _print_impl(list<In...>()); }
        static inline void _print_impl(null) { std::cout << "()"; }
        // prints list
        template<typ L> inline void println(L t = L()) {
            _print_impl(std::move(t));
            std::cout << '\n';
        }
    }
    #undef typ
}

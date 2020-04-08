// -std=c++17 or /std:c++17 (auto variable)
#pragma once
#include <cstddef>

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
        #if (!defined(__GNUG__) || defined(__clang__))
        static constexpr T value = T();
        #endif
    };
    // inferred type item
    template<auto I> using v_item = item<decltype(I), I>;

    struct null {
        using type = null;
        static constexpr size_t size = 0;
    };

    template<typ F, typ R> struct cons;
    namespace _impl {
        template<typ L, size_t i> struct get : get<typ L::rest, i-1> {};
        template<typ L> struct get<L, 0> : L::first {};

        template<typ...> struct list;
        template<typ I> struct list<I> : cons<I, null> {};
        template<typ I, typ ...In> struct list<I, In...> : cons<I, typ list<In...>::type> {};

        template<typ L, typ RL> struct reverse_acc : reverse_acc<typ L::rest, cons<typ L::first, RL>> {};
        template<typ RL> struct reverse_acc<null, RL> : RL {};
        template<typ L> struct reverse : reverse_acc<L, null> {};

        template<template<typ, typ> typ F, typ Init, typ L> struct typeFoldr
            : F<typ L::first, typ typeFoldr<F, Init, typ L::rest>::type> {};
        template<template<typ, typ> typ F, typ Init> struct typeFoldr<F, Init, null>
            : Init {};

        /* old way!
        template<template<typ> typ F, typ L, bool keep> struct typeFilter_h;
        template<template<typ> typ F, typ L> struct typeFilter_h<F, L, true>
            : cons<typ L::first, typ typeFilter_h<F, typ L::rest, F<typ L::rest::first>::value>::type> {};
        template<template<typ> typ F, typ L> struct typeFilter_h<F, L, false>
            : typeFilter_h<F, typ L::rest, F<typ L::rest::first>::value> {};
        template<template<typ> typ F, typ I> struct typeFilter_h<F, cons<I, null>, true>
            : cons<I, null> {};
        template<template<typ> typ F, typ I> struct typeFilter_h<F, cons<I, null>, false>
            : null {};
        template<template<typ> typ F, bool keep> struct typeFilter_h<F, null, keep>
            : null {};
        template<template<typ> typ F, typ L> struct typeFilter:
            typeFilter_h<F, L, F<typ L::first>::value> {};
        */
        // using functors
        template<bool keep> struct keep_or;
        template<> struct keep_or<true> { template<typ I, typ RR> struct fctr : cons<I, RR> {}; };
        template<> struct keep_or<false> { template<typ I, typ RR> struct fctr : RR {}; };
        template<template<typ> typ F> struct filter_f {
            template<typ I, typ RR> using fctr = typ keep_or<F<I>::value>::template fctr<I, RR>;
        };
        template<template<typ> typ F, typ L> struct typeFilter
            : typeFoldr<filter_f<F>::template fctr, null, L> {};

        template<template<typ> typ F, typ L> struct typeMap
            : cons<typ F<typ L::first>::type, typeMap<F, typ L::rest>> {};
        template<template<typ> typ F> struct typeMap<F, null> : null {};
    }

    template<typ F, typ R> struct cons {
        using type = cons;
        using first = F;
        // type content of first item, used for accessing nested list
        using first_vt = typ F::value_type;
        using first_l = first_vt;
        // // value of first item if it has one
        static constexpr typ F::value_type first_v = F::value;
        using rest = R;

        // gets ith element type
        template<size_t i> using get = typ _impl::get<cons, i>::type;
        template<size_t i> using get_vt = typ get<i>::value_type;
        template<size_t i> using get_l = get_vt<i>;
        template<size_t i> static constexpr typ get<i>::value_type get_v = get<i>::value;

        // length of list is updating
        static constexpr size_t size = 1 + R::size;
        static constexpr bool isnull = size == 0;
    };

    // friendly list builder
    template<typ ...In> using list = typ _impl::list<In...>::type;
    // reverse a list
    template<typ L> using reverse = typ _impl::reverse<L>::type;
    // foldr function (init is a type of an item)
    template<template<typ, typ> typ F, typ Init, typ L> using typeFoldr = typ _impl::typeFoldr<F, Init, L>::type;
    // filter function
    template<template<typ> typ F, typ L> using typeFilter = typ _impl::typeFilter<F, L>::type;
    // map function
    template<template<typ> typ F, typ L> using typeMap = typ _impl::typeMap<F, L>::type;

    #undef typ
}

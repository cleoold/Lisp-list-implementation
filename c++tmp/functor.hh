#pragma once
#include "cons.hh"

namespace functors {
    // I1, ... are packed scm::item or v_item
    #define typ typename
    // fold arithmetic ops
    #define DEF_BIOP(NAME, OP) \
    template<typ I1, typ I2> struct NAME : scm::v_item<(I1::value OP I2::value)> {};
    DEF_BIOP(bin_add, +)
    DEF_BIOP(bin_sub, -)
    DEF_BIOP(bin_mul, *)
    DEF_BIOP(bin_div, /)
    DEF_BIOP(bin_land, &&)
    DEF_BIOP(bin_lor, ||)
    DEF_BIOP(bin_band, &)
    DEF_BIOP(bin_bor, |)
    DEF_BIOP(bin_bxor, ^)
    DEF_BIOP(bin_lshift, <<)
    DEF_BIOP(bin_rshift, >>)
    #undef DEF_BIOP

    // transforms
    template<typ I> struct identity : I {};
    #define DEF_TRANSFORM(NAME, EXPR) \
    template<typ I> struct NAME : scm::v_item<EXPR> {};
    DEF_TRANSFORM(keep, true)
    DEF_TRANSFORM(iseven, ((I::value & 1) != 1))
    DEF_TRANSFORM(isodd, ((I::value & 1) == 1))
    DEF_TRANSFORM(ispowerof2, (I::value && !(I::value&(I::value-1))))
    DEF_TRANSFORM(negate, -I::value)
    DEF_TRANSFORM(lnot, !I::value)
    DEF_TRANSFORM(abs, (I::value > 0 ? I::value : -I::value))
    DEF_TRANSFORM(get_first_v, (I::value_type::first::value))
    DEF_TRANSFORM(get_rest_vt, (I::value_type::rest))
    #undef DEF_TRANSFORM

    #define DEF_OP_BY(NAME, OP) \
    template<auto t> struct NAME { template<typ I> struct fctr : scm::v_item<(I::value OP t)> {}; };
    #define DEF_OP_TO(NAME, OP) \
    template<auto t> struct NAME { template<typ I> struct fctr : scm::v_item<(t OP I::value)> {}; };
    DEF_OP_BY(equal_to, ==)
    DEF_OP_BY(nequal_to, !=)
    DEF_OP_BY(greater_than, >)
    DEF_OP_BY(greater_eq, >=)
    DEF_OP_BY(less_than, <)
    DEF_OP_BY(less_eq, <=)
    // item OP param
    DEF_OP_BY(add_by, +)
    DEF_OP_BY(sub_by, -)
    DEF_OP_BY(mul_by, *)
    DEF_OP_BY(div_by, /)
    DEF_OP_BY(land_by, &&)
    DEF_OP_BY(lor_by, ||)
    DEF_OP_BY(band_by, &)
    DEF_OP_BY(bor_by, |)
    DEF_OP_BY(bxor_by, ^)
    DEF_OP_BY(lshift_by, <<)
    DEF_OP_BY(rshift_by, >>)
    // param OP item
    DEF_OP_TO(sub_to, -)
    DEF_OP_TO(div_to, /)
    DEF_OP_TO(lshift_to, <<)
    DEF_OP_TO(rshift_to, >>)
    #undef DEF_OP_BY
    #undef DEF_OP_TO

    // composing transforms
    template<template<typ> typ F1, template<typ> typ F2> struct f_and {
        template<typ I> struct fctr : scm::item<bool, (F1<I>::value && F2<I>::value)> {};
    };
    template<template<typ> typ F1, template<typ> typ F2> struct f_or {
        template<typ I> struct fctr : scm::item<bool, (F1<I>::value || F2<I>::value)> {};
    };
    template<template<typ> typ F1> struct f_not {
        template<typ I> struct fctr : scm::item<bool, (!F1<I>::value)> {};
    };

    template<template<typ> typ F1, template<typ> typ F2> struct compose2 {
        template<typ I> struct fctr : F1<F2<I>> {};
    };
    // this is honestly a cons list, though!
    template<template<typ> typ...> struct compose;
    template<template<typ> typ I> struct compose<I> : compose2<I, identity> {};
    template<template<typ> typ I, template<typ> typ ...In> struct compose<I, In...>
        : compose2<I, compose<In...>::template fctr> {};

    #undef typ
}

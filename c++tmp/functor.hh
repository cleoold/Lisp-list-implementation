//   lisp list (cons cells) in tmp
//
//   (c) cos in April 2020
// some predefined functors
#pragma once
#include "cons.hh"

namespace functors {
    // I1, ... are packed scm::item or v_item
    #define typ typename
    // fold arithmetic ops
    #define DEF_BIEXPR(NAME, EXPR) \
        template<typ I1, typ I2> struct NAME : scm::v_item<EXPR> {};
    #define DEF_BIOP(NAME, OP) \
        DEF_BIEXPR(NAME, (I1::value OP I2::value))
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
    DEF_BIEXPR(bin_max, (I1::value > I2::value ? I1::value : I2::value))
    DEF_BIEXPR(bin_min, (I1::value < I2::value ? I1::value : I2::value))
    #undef DEF_BIOP
    #undef DEF_BIEXPR

    // define transforms as you like
    template<typ I> struct identity : I {};
    #define DEF_TRANSFORM(NAME, EXPR) \
        template<typ I> struct NAME : scm::v_item<EXPR> {};
    DEF_TRANSFORM(keep, true)
    DEF_TRANSFORM(iseven, ((I::value & 1) != 1))
    DEF_TRANSFORM(isodd, ((I::value & 1) == 1))
    DEF_TRANSFORM(ispowerof2, (I::value && !(I::value&(I::value-1))))
    DEF_TRANSFORM(squared, (I::value * I::value))
    DEF_TRANSFORM(negate, (-I::value))
    DEF_TRANSFORM(lnot, (!I::value))
    DEF_TRANSFORM(bnot, (~I::value))
    DEF_TRANSFORM(abs, (I::value > 0 ? I::value : -I::value))
    DEF_TRANSFORM(get_first_v, (I::value_type::first::value))
    DEF_TRANSFORM(get_rest_vt, (I::value_type::rest))
    #undef DEF_TRANSFORM

    template<typ t> struct become { template<typ I> struct fctr : t {}; };
    #define DEF_CURRIED_EXPR(NAME, EXPR) \
        template<auto t> struct NAME { template<typ I> struct fctr : scm::v_item<EXPR> {}; };
    #define DEF_OP_BY(NAME, OP) \
        DEF_CURRIED_EXPR(NAME, (I::value OP t))
    #define DEF_OP_TO(NAME, OP) \
        DEF_CURRIED_EXPR(NAME, (t OP I::value))
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
    DEF_CURRIED_EXPR(max_by, (I::value > t ? I::value : t))
    DEF_CURRIED_EXPR(min_by, (I::value < t ? I::value : t))
    // param OP item
    DEF_OP_TO(sub_to, -)
    DEF_OP_TO(div_to, /)
    DEF_OP_TO(lshift_to, <<)
    DEF_OP_TO(rshift_to, >>)
    #undef DEF_OP_BY
    #undef DEF_OP_TO
    #undef DEF_CURRIED_EXPR

    // composing transforms (functor compose), by using a binary op in between
    // eg functor1 && functor2 ...
    // since my cons list is not templated, i cannot wrap a functor in an
    // scm::item or insert it into an scm::list to use higher order functions
    // to compose them. sigh! but they behave the same using macro
    #define DEF_COMPOSE_EXPR(NAME, EXPR) \
        template<template<typ> typ...> struct NAME; \
        template<template<typ> typ F1, template<typ> typ F2> struct NAME<F1, F2> { \
            template<typ I> struct fctr : scm::v_item<EXPR> {}; \
        }; \
        template<template<typ> typ F1, template<typ> typ ...Fn> struct NAME<F1, Fn...> \
            : NAME<F1, NAME<Fn...>::template fctr> {};
    #define DEF_COMPOSE_OP(NAME, OP) \
        DEF_COMPOSE_EXPR(NAME, (F1<I>::value OP F2<I>::value))
    DEF_COMPOSE_OP(fc_add, +)
    DEF_COMPOSE_OP(fc_sub, -)
    DEF_COMPOSE_OP(fc_mul, *)
    DEF_COMPOSE_OP(fc_div, /)
    DEF_COMPOSE_OP(fc_land, &&)
    DEF_COMPOSE_OP(fc_lor, ||)
    DEF_COMPOSE_OP(fc_band, &)
    DEF_COMPOSE_OP(fc_bor, |)
    DEF_COMPOSE_OP(fc_bxor, ^)
    DEF_COMPOSE_OP(fc_lshift, <<)
    DEF_COMPOSE_OP(fc_rshift, >>)
    DEF_COMPOSE_EXPR(fc_max, (F1<I>::value > F2<I>::value ? F1<I>::value : F2<I>::value))
    DEF_COMPOSE_EXPR(fc_min, (F1<I>::value < F2<I>::value ? F1<I>::value : F2<I>::value))
    #undef DEF_COMPOSE_OP
    #undef DEF_COMPOSE_EXPR

    // composing transforms, by x => F1(F2(F3(x))) etc
    template<template<typ> typ...> struct compose;
    template<template<typ> typ F1, template<typ> typ F2> struct compose<F1, F2> {
        template<typ I> struct fctr : F1<F2<I>> {};
    };
    template<template<typ> typ F1, template<typ> typ ...Fn> struct compose<F1, Fn...>
        : compose<F1, compose<Fn...>::template fctr> {};

    #undef typ
}

//   lisp list (cons cells) in tmp
//
//   (c) cos in April 2020
#include <utility>
#include "functor.hh"
#include "effects.hh"

using namespace scm;

// this is how I would define a function
template<typename I> using my_func = v_item<((I::value+1)*(I::value-1))>;
// it is equivalent to this, using my predefined ones in functor.hh
template<typename I> using my_func2 = functors::fc_mul<functors::add_by<1>::fctr, functors::sub_by<1>::fctr>::fctr<I>;

template<typename L> static void printcons(const char *n, bool nicer = false) {
    std::cout << n << ": " <<
    (nicer ? effects::strlst::tostring<L>() : effects::strcons::tostring<L>())
    << '\n';
}

int main(void) {
    // example of a list construction
    // note all items are wrapped in scm::item class
    // (1, -4, (6, (8), 7), 2, ())
    using a =
    cons<item<int, 1>, cons<item<int, -4>,
                cons<item<cons<item<unsigned, 6>, cons<item<cons<item<long, 8>, null>>, cons<item<unsigned, 7>, null>>>>,
            cons<item<int, 2>, cons<item<null>, null>>>>>;
    static_assert(a::rest::rest::first_l::first_v == 6, "");
    printcons<a>("a");

    // example of a friendlier list construction
    // ((1, 4), (1, 7), (8, int*))
    using b =
    list<
        item<list<item<int, 1>, item<int, 4>>>,
        item<list<item<int, 1>, item<long, 7>>>,
        item<list<item<int, 8>, item<decltype(new int[2])>>>
    >;
    static_assert(b::get_l<2>::get_v<0> == 8, "");
    printcons<b>("b");

    // an integer list, used as some examples
    // (1, 9, 20, 3, -2, 0)
    using c =
    list<v_item<1>, v_item<9>, v_item<20>, v_item<3>, v_item<-2>, item<int, 0>>;
    static_assert(c::size == 6, "");
    printcons<c>("c");

    // list is not changed after foldr(cons, null, c) (a fact about foldr)
    static_assert(std::is_same_v<typeFoldr<cons, null, c>, c>, "");
    // compute their sum using foldr
    static_assert(typeFoldr<functors::bin_add, v_item<0>, c>::value == 31, "");

    // for c, entries greater than 0 and odd
    // (1, 9, 3)
    using c1 = typeFilter<functors::fc_land<functors::greater_than<0>::fctr, functors::isodd>::fctr, c>;
    printcons<c1>("c1");

    // test invoking my functor directly... effectively x => (x+1)*(x-1)
    static_assert(my_func<v_item<-3>>::value == 8, "");
    // that functor just defined is eventually equivalent to the following...
    static_assert(my_func2<v_item<-3>>::value == 8, "");
    // for c, for each item, apply my functor
    // (0, 80, 399, 8, 3, -1)
    using c2 = typeMap<my_func, c>;
    printcons<c2>("c2");

    // for b, take first list value, then negate
    // (-1, -1, -8)
    using b1 = typeMap<functors::compose<functors::negate, functors::get_first_v>::fctr, b>;
    printcons<b1>("b1");

    // for c, for each item, take abs, then +10, then x3, then subtract from 1.
    // effectively x => 1-(abs(x)+10)*3
    // (-32, -56, -89, -38, -35, -29)
    using c3 = typeMap<functors::compose<
        functors::sub_to<1>::fctr,
        functors::mul_by<3>::fctr,
        functors::add_by<10>::fctr,
        functors::abs
    >::fctr, c>;
    printcons<c3>("c3");

    // another fact about RACKET foldl: foldl(cons, null, c) is the same as reverse(c)
    // (see implementation)
    static_assert(std::is_same_v<typeFoldl<cons, null, c>, reverse<c>>, "");

    // can concat a, b and c, then print the result in a nicer format
    // (1, -4, (6, (8), 7), 2, (), (1, 4), (1, 7), (8, int*), 1, 9, 20, 3, -2, 0)
    using a_b_c = append<append<a, b>, c>;
    printcons<a_b_c>("a+b+c: ", true);
    // from to the construction of my cons list, one can consider the following...
    // template<class I1, class I2> using append_f = item<append<class I1::value_type, class I2::value_type>>;
    // using a_b_c = typeFoldr<append_f, item<null>, list<item<a>, item<b>, item<c>>>::value_type;
    // try.
}

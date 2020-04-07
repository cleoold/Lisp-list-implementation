#include <cstddef>
#include <utility>
#include "functor.hh"

int main(void) {
    using namespace scm;
    // example of a list construction
    // (1, -4, (6, (8), 7), 2, ())
    using a =
    cons<item<int, 1>, cons<item<int, -4>,
                cons<item<cons<item<unsigned, 6>, cons<item<cons<item<long, 8>, null>>, cons<item<unsigned, 7>, null>>>>,
            cons<item<int, 2>, cons<item<null>, null>>>>>;
    static_assert(a::rest::rest::first_l::first_v == 6, "");

    // example of a friendlier list construction
    // ((1, 4), (2, 7), (8, int*))
    using b =
    list<
        item<list<item<int, 1>, item<int, 4>>>,
        item<list<item<int, 1>, item<long, 7>>>,
        item<list<item<int, 8>, item<decltype(new int[2])>>>
    >;
    static_assert(b::get_l<2>::get_v<0> == 8, "");

    // an integer list, used as some examples
    // (1, 9, 20, 3, -2, 0)
    using c =
    list<v_item<1>, v_item<9>, v_item<20>, v_item<3>, v_item<-2>, item<int, 0>>;
    static_assert(c::size == 6, "");

    // not changed after foldr(cons, null, c) (a fact about foldr)
    static_assert(std::is_same_v<typeFoldr<cons, null, c>, c>, "");
    // their sum using foldr
    static_assert(typeFoldr<functors::bin_add, v_item<0>, c>::value == 31, "");
    // for c, entries greater than 0 and odd
    // (1, 9, 3)
    using c1 = typeFilter<functors::f_and<functors::greater_than<0>::fctr, functors::isodd>::fctr, c>;

    // for c, for each item, negate
    // (-1, -9, -20, -3, 2, 0)
    using c2 = typeMap<functors::negate, c>;

    // for b, take first list value, then negate
    // (-1, -1, -8)
    using b1 = typeMap<functors::compose<functors::negate, functors::get_first_v>::fctr, b>;

    // for c, for each item, take abs, then +10, then x3, then subtract from 1. effectively x => 1-(abs(x)+10)*3
    // (-32, -56, -89, -38, -35, -29)
    using c3 = typeMap<
        functors::compose<functors::sub_to<1>::fctr, functors::mul_by<3>::fctr,
            functors::add_by<10>::fctr, functors::abs>::fctr,
        c>;

}

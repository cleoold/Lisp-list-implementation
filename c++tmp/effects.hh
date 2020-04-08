// -std=c++17 or /std:c++17 (fold expression)
//   lisp list (cons cells) in tmp
//
//   (c) cos in April 2020
// for no reason I don't want to use stuff from type_traits
#pragma once
#include <iostream>
#include <sstream>
#include "cons.hh"

namespace scm {
    #define typ typename
    namespace effects {
        namespace _impl {
            template<typ ...S> inline std::string conc(S ...s) {
                auto oss = std::ostringstream{};
                ((oss << std::move(s)),...);
                return oss.str();
            }
        }
        namespace strcons {
            inline constexpr const char* stritem(item<null>*) { return "null"; }
            inline constexpr const char* strlist(null*) { return "null"; }
            template<typ VT, VT v> inline std::string stritem(item<VT, v>*) { return std::to_string(v); }
            template<typ F, typ R> inline std::string stritem(item<cons<F, R>>*);
            template<typ VT> inline constexpr const char* stritem(item<VT>*) { return typeid(VT).name(); }
            template<typ F, typ R> inline std::string strlist(cons<F, R>*) {
                return _impl::conc("(cons ", stritem(static_cast<F*>(nullptr)), ' ', strlist(static_cast<R*>(nullptr)), ')');
            }
            template<typ F, typ R> inline std::string stritem(item<cons<F, R>>*) { return strlist((static_cast<cons<F, R>*>(nullptr))); }
            // stringify list
            template<typ L> inline std::string tostring() {
                return strlist(static_cast<L*>(nullptr));
            }
        }
        namespace strlst {
            inline constexpr const char* stritem(item<null>*) { return "[]"; }
            inline constexpr const char* strlist(null*) { return "[]"; }
            template<typ VT, VT v> inline std::string stritem(item<VT, v>*) { return std::to_string(v); }
            template<typ F, typ R> inline std::string stritem(item<cons<F, R>>*);
            template<typ VT> inline constexpr const char* stritem(item<VT>*) { return typeid(VT).name(); }
            template<typ F> inline std::string strlist(cons<F, null>*) {
                return _impl::conc(stritem(static_cast<F*>(nullptr)), ']');
            }
            template<typ F, typ R> inline std::string strlist(cons<F, R>*) {
                return _impl::conc(stritem(static_cast<F*>(nullptr)), ", ", strlist(static_cast<R*>(nullptr)));
            }
            template<typ F, typ R> inline std::string stritem(item<cons<F, R>>*) {
                return '[' + strlist((static_cast<cons<F, R>*>(nullptr)));
            }
            // stringify list
            template<typ L> inline std::string tostring() {
                return '[' + strlist(static_cast<L*>(nullptr));
            }
        }
    }
    #undef typ
}

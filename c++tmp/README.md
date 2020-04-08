All the miracle happens from

```cpp
template<class F, class R> struct cons {
    using first = F;
    using rest = R;
}
```

The file `test.cpp` has detailed demonstration about what's going on.

standard: `-std=c++17`
compiled on x64 gcc: 7.1, 7.4, 8.3, 9.3
compiled on x64 clang: 5.0, 6.0, 7.0, 8.0
compiled on x64 msvc: 19.24

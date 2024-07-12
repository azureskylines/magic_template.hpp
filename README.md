# magic_template.hpp
wacky compiler hack, jank beware

### c++20 or above

## usage

```cpp
template <typename T>
auto print_type() -> void {
    auto type_name = magic_template::of_arg<__FUNCSIG__, "T">().c_str();
    std::cout << type_name << std::endl;
}

template <int T>
auto print_value() -> void {
    auto value_str = magic_template::of_arg<__FUNCSIG__, "T">().c_str();
    std::cout << value_str << std::endl;
}

// example:
print_type<int>(); // "int"
print_type<std::string>(); // "std::basic_string<char>"
print_type<some_struct>(); // "some_struct"

print_value<1>(); // "1"
print_value<-2>(); // "-2"
print_value<7>(); // "7"
```

## credits
BasedInc/libhat for fixed_string
magic_enum for the general idea

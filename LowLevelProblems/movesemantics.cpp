#include <bits/stdc++.h>

template<typename T>
constexpr std::remove_reference_t<T>&& custom_move(T&& t) noexcept {
    return static_cast<std::remove_reference_t<T>&&>(t);
};

template<typename T>
constexpr T&& custom_forward(std::remove_reference_t<T>&& t) noexcept {
    static_assert(!std::is_lvalue_reference<T>, "Cant forward rvalue as lvalue"); // int& k = std::custom_forward<int&>(5);
    return static_cast<T&&>(t);
}


template<typename T>
constexpr T&& custom_forward(std::remove_reference_t<T>& t) noexcept {
    return static_cast<T&&>(t);
}
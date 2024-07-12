#pragma once

#include <format>
#include <string>
#include <vector>

#if defined(_MSC_VER) || defined(__clang__)
#if defined(__clang__) && !defined(_MSC_VER)
#error "magic-template requires msvc or clang-cl."
#endif
#else
#error "magic-template requires msvc or clang-cl."
#endif

namespace magic_template {
    // types taken from libhat
    namespace types {
        template<typename Char, size_t N, template<size_t> typename Derived>
        struct basic_fixed_string {
            using const_reference = const Char &;
            using const_pointer = const Char *;
            using const_iterator = const_pointer;

            static constexpr auto npos = static_cast<size_t>(-1);

            constexpr basic_fixed_string(std::basic_string_view<Char> str) {
                std::copy_n(str.data(), N, value);
            }

            constexpr basic_fixed_string(const Char (&str)[N + 1]) {
                std::copy_n(str, N, value);
            }

            [[nodiscard]] constexpr const_iterator begin() const {
                return this->c_str();
            }

            [[nodiscard]] constexpr const_iterator end() const {
                return this->begin() + this->size();
            }

            [[nodiscard]] constexpr const_reference operator[](size_t pos) const {
                return this->value[pos];
            }

            [[nodiscard]] constexpr const_reference at(size_t pos) const {
                return this->value[pos];
            }

            [[nodiscard]] constexpr const_reference front() const {
                return this->value[0];
            }

            [[nodiscard]] constexpr const_reference back() const {
                return this->value[size() - 1];
            }

            [[nodiscard]] constexpr const_pointer c_str() const {
                return static_cast<const Char *>(this->value);
            }

            [[nodiscard]] constexpr const_pointer data() const {
                return this->c_str();
            }

            [[nodiscard]] constexpr size_t size() const {
                return N;
            }

            [[nodiscard]] constexpr bool empty() const {
                return this->size() == 0;
            }

            template<
                size_t Pos = 0,
                size_t Count = npos,
                size_t M = (Count == npos ? N - Pos : Count)
            >
            [[nodiscard]] constexpr auto substr() const -> Derived<M> {
                static_assert(Pos + M <= N);
                Char buf[M + 1]{};
                std::copy_n(this->value + Pos, M, buf);
                return buf;
            }

            template<size_t M, size_t K = N + M>
            constexpr auto operator+(const basic_fixed_string<Char, M, Derived> &str) const -> Derived<K> {
                Char buf[K + 1]{};
                std::copy_n(this->value, this->size(), buf);
                std::copy_n(str.value, str.size(), buf + this->size());
                return buf;
            }

            template<size_t M>
            constexpr auto operator+(const Char (&str)[M]) const {
                return *this + Derived<M - 1>{str};
            }

            template<size_t M>
            constexpr bool operator==(const basic_fixed_string<Char, M, Derived> &str) const {
                return std::equal(this->begin(), this->end(), str.begin(), str.end());
            }

            constexpr bool operator==(std::basic_string<Char> str) const {
                return std::equal(this->begin(), this->end(), str.begin(), str.end());
            }

            constexpr bool operator==(std::basic_string_view<Char> str) const {
                return std::equal(this->begin(), this->end(), str.begin(), str.end());
            }

            constexpr bool operator==(const Char *str) const {
                return std::equal(this->begin(), this->end(), str, str + std::char_traits<Char>::length(str));
            }

            constexpr std::basic_string<Char> str() const {
                return {this->begin(), this->end()};
            }

            constexpr std::basic_string_view<Char> to_view() const {
                return {this->begin(), this->end()};
            }

            Char value[N + 1]{};
        };

        template<size_t N>
        struct fixed_string : basic_fixed_string<char, N, fixed_string> {
            using basic_fixed_string<char, N, fixed_string>::basic_fixed_string;
        };

        template<size_t N>
        fixed_string(const char(&str)[N]) -> fixed_string<N - 1>;

        template<size_t N, size_t M>
        constexpr auto operator+(const char (&c_str)[N], const basic_fixed_string<char, M, fixed_string>& l_str) {
            return fixed_string{c_str} + l_str;
        }
    }

    using types::fixed_string;

    [[nodiscard]] constexpr auto find_arg(const std::string& sig, const std::string& arg) -> std::vector<char> {
        const auto template_list = " " + sig.substr(sig.find('[') + 1, sig.find(']') - sig.find('[') - 1) + ",";
        const size_t arg_offset = template_list.find(" " + arg + " = ") + arg.size() + 4;
        const auto str = template_list.substr(arg_offset, template_list.find(',', arg_offset) - arg_offset);
        return {str.begin(), str.end()};
    }

    template<fixed_string t_sig, fixed_string t_arg>
    consteval auto of_arg() -> auto {
        const auto str = find_arg(t_sig.c_str(), t_arg.c_str());
        constexpr auto size = find_arg(t_sig.c_str(), t_arg.c_str()).size();
        return fixed_string<size>{std::string(str.begin(), str.end())};
    }

    template<auto t_forwarded>
    consteval auto of() -> auto {
        return of_arg<__FUNCSIG__, "t_forwarded">();
    }
}

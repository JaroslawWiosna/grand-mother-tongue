#pragma once

#include "../3rd_party/aids.hpp"

#include <optional>

struct PersonID {
    aids::String_View value{};
};

// https://stackoverflow.com/a/17017281
namespace std {
template <> struct hash<PersonID> {
    std::size_t operator()(const PersonID &k) const {
        using aids::operator""_sv;
        if ("NN"_sv == k.value) {
            return 0;
        }
        if ("UN"_sv == k.value) {
            return 1;
        }
        if ('Q' != k.value.data[0]) {
            aids::println(stderr, k.value);
        }
        assert('Q' == k.value.data[0]);
        auto v = k.value;
        v.chop(1);
        auto result = v.as_integer<std::size_t>();
        return unwrap_or_panic(result);
    }
};
} // namespace std

bool operator==(const PersonID &lhs, const PersonID &rhs) {
    return (lhs.value == rhs.value);
}

bool operator!=(const PersonID &lhs, const PersonID &rhs) {
    return !(lhs == rhs);
}

struct Person {
    PersonID id{};
    aids::Maybe<aids::String_View> name{};
    aids::Maybe<aids::String_View> country{};
    aids::Maybe<PersonID> father{};
    aids::Maybe<PersonID> mother{};
};

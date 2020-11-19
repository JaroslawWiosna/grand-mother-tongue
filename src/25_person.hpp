#pragma once

#include <string>
#include <optional>

struct PersonID {
    std::string value{};
};

// https://stackoverflow.com/a/17017281
namespace std {
template <> struct hash<PersonID> {
    std::size_t operator()(const PersonID &k) const {
        using std::hash;
        using std::size_t;
        using std::string;

        return (hash<string>()(k.value));
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
    std::optional<std::string> name{};
    std::optional<std::string> country{};
    std::optional<PersonID> father{};
    std::optional<PersonID> mother{};
};

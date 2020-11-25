#pragma once

#include "../3rd_party/aids-patched.hpp"

#include <unordered_map>
struct PersonID;

// template<typename Key, typename Value>
// struct Pair {
//     Key key{};
//     Value value{};
// };

struct Blood {
    bool unknown{true};
    aids::Hash_Map<aids::String_View, double> map{};
};

// namespace std {
// template <> struct hash<aids::String_View> {
//     std::size_t operator()(const aids::String_View &k) const {
//         using aids::operator""_sv;
//         using std::hash;
//         using std::size_t;
//         using std::string;

//         char buf[512] = {0};
//         aids::String_Buffer sbuffer = {sizeof(buf), buf};
//         aids::sprint(&sbuffer, k);
//         assert(strlen(buf) < 500);

//         return (hash<string>()(buf));
//     }
// };
// } // namespace std

struct PhoneBook {
    std::unordered_map<PersonID, Person> hashmap{};
    bool contains(PersonID);
    bool contains(aids::Maybe<PersonID>);

    Blood origin_by_blood(PersonID);
    void print_origin_by_blood(PersonID);

    int find_native_tongue_in_wikidata();
    int find_names_in_wikidata();
    int find_parents_in_wikidata();
    void dump(std::string filepath);
};

PhoneBook parse(aids::String_View filepath);

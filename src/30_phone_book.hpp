#pragma once

#include "../3rd_party/aids-patched.hpp"

struct PersonID;

struct Blood {
    bool unknown{true};
    aids::Hash_Map<aids::String_View, double> map{};
};

struct PhoneBook {
    aids::Hash_Map<PersonID, Person> hashmap{};
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

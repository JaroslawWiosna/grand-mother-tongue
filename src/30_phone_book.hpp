#pragma once

#include <unordered_map>

struct PersonID;

using Blood = std::unordered_map<std::string, double>;

struct PhoneBook {
    std::unordered_map<PersonID, Person> hashmap{};
    bool contains(PersonID);
    bool contains(std::optional<PersonID>);

    Blood origin_by_blood(PersonID);
    void print_origin_by_blood(PersonID);
};

PhoneBook parse(std::string filepath);

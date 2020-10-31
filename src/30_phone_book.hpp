#pragma once

#include <unordered_map>

struct PhoneBook {
    std::unordered_map<PersonID, Person> hashmap{};
};

PhoneBook parse(std::string filepath);

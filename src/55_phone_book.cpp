#include "30_phone_book.hpp"

#include "01_utils.hpp"

#include <iostream>

PhoneBook parse(std::string filepath) {
    PhoneBook res{};
    auto database = load_file_to_vector_of_lines(filepath);
    for (auto &line : database) {
        if (line.find(std::string{':'}) != std::string::npos) {
            std::cout << " [INFO] Found a record:          "
                      << line << "\n";
            std::string id = line.substr(0, line.find(std::string{":"}));
            line = line.substr(line.find(std::string{":"}) + 1);
            std::string name = line.substr(0, line.find(std::string{":"}));
            std::string country = line.substr(line.find(std::string{":"}) + 1);
            trim(id);
            trim(name);
            trim(country);

            Person person{};
            person.id.value = id;
            person.name = name;
            person.country = country;

            res.hashmap[person.id] = person;

        } else if (line.find(std::string{'='}) != std::string::npos) {
            std::cout << " [INFO] Found parents:           "
                      << line << "\n";
            std::string id = line.substr(0, line.find(std::string{"="}));
            line = line.substr(line.find(std::string{"="}) + 1);
            std::string father = line.substr(0, line.find(std::string{"+"}));
            std::string mother = line.substr(line.find(std::string{"+"}) + 1);

            trim(id);
            trim(father);
            trim(mother);

            auto search = res.hashmap.find(PersonID{id});
            if (search != res.hashmap.end()) {
                search->second.father = PersonID{father};
                search->second.mother = PersonID{mother};
            } else {
                std::cout << "Not found\n";
                exit(1);
            }

        } else {
            std::cout << " [INFO] This line is not parsed: "
                      << line << "\n";
        }
    }
    return res;
}

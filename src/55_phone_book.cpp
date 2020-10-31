#include "30_phone_book.hpp"

#include "01_utils.hpp"

#include <iostream>

bool PhoneBook::contains(PersonID id) {
    return (hashmap.end() != hashmap.find(id));
}

bool PhoneBook::contains(std::optional<PersonID> id) {
    if (not id.has_value()) {
        return {};
    }
    return contains(id.value());
}

Blood PhoneBook::origin_by_blood(PersonID id) {
    Blood res;
    auto father = hashmap[id].father;
    auto mother = hashmap[id].mother;
    if (!contains(father) && !contains(mother)) {
        res[hashmap[id].country] = 1.0;
    } else {
        if (contains(father)) {
            for (const auto & [ key, value ] :
                 origin_by_blood(father.value())) {
                res[key] += 0.5 * value;
            }
        }
        if (contains(mother)) {
            for (const auto & [ key, value ] :
                 origin_by_blood(mother.value())) {
                res[key] += 0.5 * value;
            }
        }
    }
    return res;
}
void PhoneBook::print_origin_by_blood(PersonID id) {
    auto res = origin_by_blood(id);
    std::cout << "<< ORIGIN BY BLOOD >>\n";
    std::cout << hashmap[id].name << "\n";
    for (const auto & [ key, value ] : res) {
        if (key != "Unknown") {
            std::cout << "  " << key << " " << value << "\n";
        }
    }
    std::cout << "\n";
}

PhoneBook parse(std::string filepath) {
    PhoneBook res{};
    auto database = load_file_to_vector_of_lines(filepath);
    for (auto &line : database) {
        if (line.find(std::string{':'}) != std::string::npos) {
            std::cout << " [INFO] Found a record:          " << line << "\n";
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
            std::cout << " [INFO] Found parents:           " << line << "\n";
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
            std::cout << " [INFO] This line is not parsed: " << line << "\n";
        }
    }
    return res;
}

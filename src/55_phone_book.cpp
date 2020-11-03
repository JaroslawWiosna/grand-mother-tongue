#include "29_libcurl_and_json.hpp"
#include "30_phone_book.hpp"

#include "01_utils.hpp"


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
    std::cout << hashmap[id].name.value_or("NN") << "\n";
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
                if (father == "NN" || father.empty()) {
                    search->second.father = {};
                } else {
                    search->second.father = PersonID{father};
                }
                if (mother == "NN" || mother.empty()) {
                    search->second.mother = {};
                } else {
                    search->second.mother = PersonID{mother};
                }
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

void PhoneBook::find_names_in_wikidata() {
    for (auto & [ key, value ] : hashmap) {
        if (not value.name.has_value()) {
            value.name = get_name(key);
            std::cout << "[FOUND NAME] " << value.name.value_or("NN") << "\n";
        }
    }
}

// returns true if at least one parent was found
// return false if no parent was found
bool PhoneBook::find_parents_in_wikidata() {
    bool result = {};
    std::unordered_map<PersonID, Person> new_parents{};
    for (const auto & [ key, value ] : hashmap) {
        if (not hashmap[key].father.has_value()) {
            auto father = get_father(key);
            if (father.has_value()) {
                new_parents[key].father = father;
                result = true;
                std::cout << key.value << "'s father is "
                          << father.value().value << "\n";
            }
        }
        if (not hashmap[key].mother.has_value()) {
            auto mother = get_mother(key);
            if (mother.has_value()) {
                new_parents[key].mother = mother;
                result = true;
                std::cout << key.value << "'s mother is "
                          << mother.value().value << "\n";
            }
        }
    }
    if (result == true) {
        for (const auto & [ key, value ] : new_parents) {
            if (not hashmap[key].father.has_value()) {
                hashmap[key].father = value.father;
                std::cout << "[ADD FATHER] " << key.value << "'s father is "
                          << value.father.value().value << "\n";
            }
            if (not hashmap[key].mother.has_value()) {
                hashmap[key].mother = value.mother;
                std::cout << "[ADD MOTHER] " << key.value << "'s mother is "
                          << value.mother.value().value << "\n";
            }
            if (not contains(value.father)) {
                hashmap[value.father.value()] = Person{};
                std::cout << "[ADD NEW] " << value.father.value().value << "\n";
            }
            if (not contains(value.mother)) {
                hashmap[value.mother.value()] = Person{};
                std::cout << "[ADD NEW] " << value.mother.value().value << "\n";
            }
        }
    }
    return result;
}

void PhoneBook::dump(std::string filepath) {
    std::ofstream f;
    f.open(filepath);
    for (const auto & [ key, value ] : hashmap) {
        f << key.value << " : " << value.name.value_or("NN") << " : "
          << value.country << "\n";
    }
    f << "\n";
    for (const auto & [ key, value ] : hashmap) {
        f << key.value << " = "
          << (value.father.has_value() ? value.father.value().value : "NN")
          << " + "
          << (value.mother.has_value() ? value.mother.value().value : "NN")
          << "\n";
    }

    f.close();
}

#include "29_libcurl_and_json.hpp"
#include "30_phone_book.hpp"

#include "01_utils.hpp"


bool PhoneBook::contains(PersonID id) {
    return (hashmap.end() != hashmap.find(id));
}

bool PhoneBook::contains(aids::Maybe<PersonID> id) {
    return (not id.has_value) ? false : contains(id.unwrap);
}

Blood PhoneBook::origin_by_blood(PersonID id) {
    using aids::operator""_sv;

    // std::cout << std::hash<PersonID>(id) << '\n';
    Blood res{};
    auto father = hashmap[id].father;
    auto mother = hashmap[id].mother;
    if (!contains(father) && !contains(mother)) {
        res[hashmap[id].country.value_or("Unknown"_sv)] = 1.0;
    } else {
        if (contains(father)) {
            for (const auto & [ key, value ] :
                origin_by_blood(father.unwrap)) {
                res[key] += 0.5 * value;
            }
        }
        if (contains(mother)) {
            for (const auto & [ key, value ] :
                 origin_by_blood(mother.unwrap)) {
                res[key] += 0.5 * value;
            }
        }
    }
    return res;
}
void PhoneBook::print_origin_by_blood(PersonID id) {
    using aids::operator""_sv;

    auto res = origin_by_blood(id);
    aids::println(stdout, "<< ORIGIN BY BLOOD >>");
    aids::println(stdout, hashmap[id].name.value_or("NN"_sv));
    for (const auto & [ key, value ] : res) {
        if (key != "Unknown"_sv) {
            println(stdout, "  ", key, "  ", (float)value);
        }
    }
    aids::println(stdout);
}

Blood_but_better PhoneBook::origin_by_blood_but_better(PersonID id) {
    using aids::operator""_sv;

    Blood_but_better res{};
    auto father_hash = hashmap[id].father;
    auto mother_hash = hashmap[id].mother;
    if (!contains(father_hash) && !contains(mother_hash)) {
        if (hashmap[id].country.has_value) {
            res.map.push({{hashmap[id].country.unwrap}, 1.0});
            res.unknown = false;
        }
    }
    if (contains(father_hash) && !contains(mother_hash)) {
        auto father = origin_by_blood_but_better(father_hash.unwrap);
        if (not father.unknown) {
            for (int i{}; i < father.map.size; ++i) {
                res.map.push({{father.map.data[i].key}, father.map.data[i].value / 2});
            }
            res.unknown = false;
        }
        if (hashmap[id].country.has_value) {
            res.map.push({{hashmap[id].country.unwrap}, 0.5});
            res.unknown = false;
        }
    }
    if (!contains(father_hash) && contains(mother_hash)) {
        auto mother = origin_by_blood_but_better(mother_hash.unwrap);
        if (not mother.unknown) {
            for (int i{}; i < mother.map.size; ++i) {
                res.map.push({{mother.map.data[i].key}, mother.map.data[i].value / 2});
            }
            res.unknown = false;
        }
        if (hashmap[id].country.has_value) {
            res.map.push({{hashmap[id].country.unwrap}, 0.5});
            res.unknown = false;
        }
    }
    if (contains(father_hash) && contains(mother_hash)) {
        auto father = origin_by_blood_but_better(father_hash.unwrap);
        if (not father.unknown) {
            for (int i{}; i < father.map.size; ++i) {
                res.map.push({{father.map.data[i].key}, father.map.data[i].value / 2});
            }
            res.unknown = false;
        }
        auto mother = origin_by_blood_but_better(mother_hash.unwrap);
        if (not mother.unknown) {
            for (int i{}; i < mother.map.size; ++i) {
                res.map.push({{mother.map.data[i].key}, mother.map.data[i].value / 2});
            }
            res.unknown = false;
        }
    }

    return res;
}

void PhoneBook::print_origin_by_blood_but_better(PersonID id) {
    using aids::operator""_sv;

    auto res = origin_by_blood_but_better(id);
    aids::println(stdout, "<< ORIGIN BY BLOOD >>");
    for (int i{}; i < res.map.size; ++i) {
        aids::println(stdout, " ", res.map.data[i].key, " ", (float)res.map.data[i].value);
    }
    aids::println(stdout);
}


PhoneBook parse(aids::String_View filepath) {
    PhoneBook res{};
    
    char buf[512] = {0};
    aids::String_Buffer sbuffer = {sizeof(buf), buf};
    aids::sprint(&sbuffer, filepath);
    auto content = aids::read_file_as_string_view(buf);
    if (not content.has_value) {
        println(stderr, "Could not read file `", filepath, "`");
        abort();
    }

    while (content.unwrap.count > 0) {
        // ID : NAME : country : father : mother
        auto line = content.unwrap.chop_by_delim('\n').trim();
        if (line.count == 0)   continue; // skip empty lines
        if (*line.data == '#') continue; // skip commentsa

        Person person{};
        person.id = PersonID{line.chop_by_delim(':').trim()};
        person.name = {true, line.chop_by_delim(':').trim()};
        person.country = {true, line.chop_by_delim(':').trim()};
        person.father = {true, PersonID{line.chop_by_delim(':').trim()}};
        person.mother = {true, PersonID{line.chop_by_delim(':').trim()}};

        person.name.has_value = ((person.name.unwrap.count == 0) ? false : true);
        person.country.has_value = ((person.country.unwrap.count == 0) ? false : true);
        person.father.has_value = ((person.father.unwrap.value.count == 0) ? false : true);
        person.mother.has_value = ((person.mother.unwrap.value.count == 0) ? false : true);

        res.hashmap[person.id] = person;
    }
    return res;
}

int PhoneBook::find_names_in_wikidata() {
    using aids::operator""_sv;

    RestApiUrlRequests reqs{};
    for (const auto & [ key, value ] : hashmap) {
        if (not value.name.has_value) {
            reqs.emplace_back(RestApiUrlRequest{to_stdstring(url_of_get_name(key))});
            // value.name = get_name(key);
        }
    }
    auto resps = request(reqs);
    for (auto & [ key, value ] : hashmap) {
        if (not value.name.has_value) {
            std::string url_of_get_nam = to_stdstring(url_of_get_name(key));
            auto it = std::find_if(resps.begin(), resps.end(), [&](const RestApiUrlResponse &r){return url_of_get_nam == r.url;});
            if (it != resps.end()) {
                auto result = extract_name((*it).res.value());
                if (result.has_value) {
                    value.name = {true, {result.unwrap}};
                    aids::println(stderr, "[FOUND NAME]          ", value.name.value_or(""_sv));
                }
            }
        }
    }
    return resps.size();
}

int PhoneBook::find_native_tongue_in_wikidata() {
    using aids::operator""_sv;

    RestApiUrlRequests reqs{};
    for (const auto & [ key, value ] : hashmap) {
        if (not value.country.has_value) {
            reqs.emplace_back(RestApiUrlRequest{to_stdstring(url_of_get_native(key))});
            // value.name = get_name(key);
        }
    }
    auto resps = request(reqs);
    for (auto & [ key, value ] : hashmap) {
        if (not value.country.has_value) {
            std::string url_of_get_nat = to_stdstring(url_of_get_native(key));
            auto it = std::find_if(resps.begin(), resps.end(), [&](const RestApiUrlResponse &r){return url_of_get_nat == r.url;});
            if (it != resps.end()) {
                auto result = extract_native((*it).res.value());
                if (result.has_value) {
                    value.country = {true, {result.unwrap}};
                    aids::println(stderr, "[FOUND NATIVE TONGUE] ", value.country.value_or(""_sv));
                }
            }
        }
    }
    return resps.size();
}

int PhoneBook::find_parents_in_wikidata() {
    using aids::operator""_sv;

    bool result = {true};
    std::unordered_map<PersonID, Person> new_parents{};
    char buf[512] = {0};
    RestApiUrlRequests reqs{};
    for (const auto & [ key, value ] : hashmap) {
        if (not hashmap[key].father.has_value) {
            reqs.emplace_back(RestApiUrlRequest{to_stdstring(url_of_get_father(key))});
        }
        if (not hashmap[key].mother.has_value) {
            reqs.emplace_back(RestApiUrlRequest{to_stdstring(url_of_get_mother(key))});
        }
    }
    auto resps = request(reqs);
    for (const auto & [ key, value ] : hashmap) {
        if (key == PersonID{"UN"_sv}) {
            continue;
        }
        if (not hashmap[key].father.has_value) {
            std::string url_of_get_fathe = to_stdstring(url_of_get_father(key));
            auto it = find_if(resps.begin(), resps.end(), [&](const RestApiUrlResponse &r){return url_of_get_fathe == r.url;});
            if (it != resps.end()) {
                auto result = extract_p22_or_p25((*it).res.value());
                if (result.has_value && result.unwrap.data[0] == 'Q') {
                    new_parents[key].father = {true, PersonID{{result.unwrap}}};
                }
            }
        }
        if (not hashmap[key].mother.has_value) {
            std::string url_of_get_mothe = to_stdstring(url_of_get_mother(key));
            auto it = find_if(resps.begin(), resps.end(), [&](const RestApiUrlResponse &r){return url_of_get_mothe == r.url;});
            if (it != resps.end()) {
                auto result = extract_p22_or_p25((*it).res.value());
                if (result.has_value && result.unwrap.data[0] == 'Q') {
                    new_parents[key].mother = {true, PersonID{{result.unwrap}}};
                }
            }
        }
    }   

    if (result == true) {
        for (const auto & [ key, value ] : new_parents) {
            if (key == PersonID{"UN"_sv}) {
                continue;
            }
            if (not hashmap[key].father.has_value) {
                if (value.father.has_value) {
                    hashmap[key].father = value.father;
                    aids::println(stderr, "[ADD FATHER]          ", key.value, "'s father is ", value.father.unwrap.value);
                }
            }
            if (not hashmap[key].mother.has_value) {
                if (value.mother.has_value) {
                    hashmap[key].mother = value.mother;
                    aids::println(stderr, "[ADD MOTHER]          ", key.value, "'s mother is ", value.mother.unwrap.value);
                }
            }
            if (not contains(value.father)) {
                if (value.father.has_value) {
                    hashmap[value.father.unwrap] = Person{};
                    aids::println(stderr, "[ADD NEW]             ", value.father.unwrap.value);
                }
            }
            if (not contains(value.mother)) {
                if (value.mother.has_value) {
                    hashmap[value.mother.unwrap] = Person{};
                    aids::println(stderr, "[ADD NEW]             ", value.mother.unwrap.value);
                }
            }
        }
    }
    return resps.size();
}

void PhoneBook::dump(std::string filepath) {
    using aids::operator""_sv;

    FILE* file = fopen(filepath.c_str(), "w+");
    for (const auto & [ key, value ] : hashmap) {
        aids::println(file, key.value, " : ", value.name.value_or("NN"_sv),
                " : ", value.country.value_or(""_sv),
                " : ", value.father.has_value ? value.father.unwrap.value : ""_sv,
                " : ", value.mother.has_value ? value.mother.unwrap.value : ""_sv);
    }
    fclose(file);
}

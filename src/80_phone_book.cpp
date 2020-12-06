#include "29_libcurl_and_json.hpp"
#include "30_phone_book.hpp"

#include "01_utils.hpp"


bool PhoneBook::contains(PersonID id) {
    for (size_t i = 0; i < hashmap.capacity; ++i) {
        if (hashmap.buckets[i].has_value) {
            if (hashmap.buckets[i].unwrap.key == id) {
                return true;
            }
        }
    }
    return false;
}

bool PhoneBook::contains(aids::Maybe<PersonID> id) {
    return (not id.has_value) ? false : contains(id.unwrap);
}

Blood PhoneBook::origin_by_blood(PersonID id) {
    using aids::operator""_sv;

    Blood res{};
    auto father_hash = hashmap[id]->father;
    auto mother_hash = hashmap[id]->mother;

    if (contains(father_hash)) {
        auto father = origin_by_blood(father_hash.unwrap);
        defer(destroy(father.map));
        if (not father.unknown) {
            for (size_t i = 0; i < father.map.capacity; ++i) {
                if (father.map.buckets[i].has_value) {
                    auto foo = res.map.get(father.map.buckets[i].unwrap.key);
                    if (foo.has_value) {
                        *foo.unwrap += father.map.buckets[i].unwrap.value / 2;
                        res.unknown = false;
                    } else {
                        res.map.insert(father.map.buckets[i].unwrap.key, father.map.buckets[i].unwrap.value / 2);
                        res.unknown = false;
                    }                   
                }
            }            
        } else {
            goto father_unknown_country;
        }
    } else {
father_unknown_country:
        if (hashmap[id]->country.has_value) {
            auto foo = res.map.get(hashmap[id]->country.unwrap);
            if (foo.has_value) {
                *foo.unwrap += 50.0;
                res.unknown = false;
            } else {
                res.map.insert(hashmap[id]->country.unwrap, 50.0);                
                res.unknown = false;
            }
        }        
    }

    if (contains(mother_hash)) {
        auto mother = origin_by_blood(mother_hash.unwrap);
        defer(destroy(mother.map));
        if (not mother.unknown) {
            for (size_t i = 0; i < mother.map.capacity; ++i) {
                if (mother.map.buckets[i].has_value) {
                    auto foo = res.map.get(mother.map.buckets[i].unwrap.key);
                    if (foo.has_value) {
                        *foo.unwrap += mother.map.buckets[i].unwrap.value / 2;
                        res.unknown = false;
                    } else {
                        res.map.insert(mother.map.buckets[i].unwrap.key, mother.map.buckets[i].unwrap.value / 2);
                        res.unknown = false;
                    }                   
                }
            }            
        } else {
            goto mother_unknown_country;
        }
    } else {
mother_unknown_country:
        if (hashmap[id]->country.has_value) {
            auto foo = res.map.get(hashmap[id]->country.unwrap);
            if (foo.has_value) {
                *foo.unwrap += 50.0;
                res.unknown = false;
            } else {
                res.map.insert(hashmap[id]->country.unwrap, 50.0);                
                res.unknown = false;
            }
        }        
    }
    return res;
}

void PhoneBook::print_origin_by_blood(PersonID id) {
    using aids::operator""_sv;

    auto res = origin_by_blood(id);
    defer(destroy(res.map));
    aids::println(stdout, "<< ORIGIN BY BLOOD >>");
    for (size_t i = 0; i < res.map.capacity; ++i) {
        if (res.map.buckets[i].has_value) {
            aids::println(stdout, " ", res.map.buckets[i].unwrap.key, " ", (float)res.map.buckets[i].unwrap.value);
        }
    }
    aids::println(stdout);
}


PhoneBook parse(aids::String_View filepath) {
    PhoneBook res{};
    
    char buf[512] = {0};
    aids::String_Buffer sbuffer = {sizeof(buf), buf, {}};
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
        res.hashmap[person.id]->name = {true, line.chop_by_delim(':').trim()};
        res.hashmap[person.id]->country = {true, line.chop_by_delim(':').trim()};
        res.hashmap[person.id]->father = {true, PersonID{line.chop_by_delim(':').trim()}};
        res.hashmap[person.id]->mother = {true, PersonID{line.chop_by_delim(':').trim()}};
        res.hashmap[person.id]->birth_year = {true, line.chop_by_delim(':').trim()};
        res.hashmap[person.id]->death_year = {true, line.chop_by_delim(':').trim()};

        res.hashmap[person.id]->name.has_value = ((person.name.unwrap.count == 0) ? false : true);
        res.hashmap[person.id]->country.has_value = ((person.country.unwrap.count == 0) ? false : true);
        res.hashmap[person.id]->father.has_value = ((person.father.unwrap.value.count == 0) ? false : true);
        res.hashmap[person.id]->mother.has_value = ((person.mother.unwrap.value.count == 0) ? false : true);
        res.hashmap[person.id]->birth_year.has_value = ((person.birth_year.unwrap.count == 0) ? false : true);
        res.hashmap[person.id]->death_year.has_value = ((person.death_year.unwrap.count == 0) ? false : true);
    }
    return res;
}

int PhoneBook::find_names_in_wikidata() {
    using aids::operator""_sv;

    RestApiUrlRequests reqs{};
    for (size_t i = 0; i < hashmap.capacity; ++i) {
        if (hashmap.buckets[i].has_value) {
            auto &key = hashmap.buckets[i].unwrap.key;
            auto &value = hashmap.buckets[i].unwrap.value;
            if (not value.name.has_value) {
                reqs.emplace_back(RestApiUrlRequest{to_stdstring(url_of_get_name(key))});
            }
        }
    }
    auto resps = request(reqs);

    for (size_t i = 0; i < hashmap.capacity; ++i) {
        if (hashmap.buckets[i].has_value) {
            auto &key = hashmap.buckets[i].unwrap.key;
            auto &value = hashmap.buckets[i].unwrap.value;
            if (not value.name.has_value) {
                std::string url_of_get_nam = to_stdstring(url_of_get_name(key));
                auto it = std::find_if(resps.begin(), resps.end(), [&](const RestApiUrlResponse &r){return url_of_get_nam == r.url;});
                if (it != resps.end()) {
                    auto result = extract_name((*it).res.value());
                    if (result.has_value) {
                        value.name = {true, {result.unwrap}};
                        if (verbosity) {
                            aids::println(stderr, "[FOUND NAME]          ", value.name.value_or(""_sv));
                        }
                    }
                }
            }
        }
    }
    return resps.size();
}

int PhoneBook::find_birth_year() {
    using aids::operator""_sv;

    RestApiUrlRequests reqs{};
    for (size_t i = 0; i < hashmap.capacity; ++i) {
        if (hashmap.buckets[i].has_value) {
            auto &key = hashmap.buckets[i].unwrap.key;
            auto &value = hashmap.buckets[i].unwrap.value;
            if (not value.birth_year.has_value) {
                reqs.emplace_back(RestApiUrlRequest{to_stdstring(url_of_get_birth_year(key))});
            }
        }
    }
    auto resps = request(reqs);

    for (size_t i = 0; i < hashmap.capacity; ++i) {
        if (hashmap.buckets[i].has_value) {
            auto &key = hashmap.buckets[i].unwrap.key;
            auto &value = hashmap.buckets[i].unwrap.value;
            if (not value.birth_year.has_value) {
                std::string url_of_get_birth = to_stdstring(url_of_get_birth_year(key));
                auto it = std::find_if(resps.begin(), resps.end(), [&](const RestApiUrlResponse &r){return url_of_get_birth == r.url;});
                if (it != resps.end()) {
                    auto result = extract_birth_year((*it).res.value());
                    if (result.has_value) {
                        while (result.unwrap.count > 0 && not isdigit(result.unwrap.data[0])) {
                            result.unwrap.chop(1);
                        }
                        auto chopped_year = result.unwrap.chop_by_delim('-');
                        value.birth_year = {true, chopped_year};
                        if (verbosity) {
                            aids::println(stderr, "[FOUND BIRTH YEAR]    *", value.birth_year.value_or(""_sv), "\t", value.name.unwrap);
                        }
                    }
                }
            }
        }
    }
    return resps.size();    
}

int PhoneBook::find_death_year() {
    using aids::operator""_sv;

    RestApiUrlRequests reqs{};
    for (size_t i = 0; i < hashmap.capacity; ++i) {
        if (hashmap.buckets[i].has_value) {
            auto &key = hashmap.buckets[i].unwrap.key;
            auto &value = hashmap.buckets[i].unwrap.value;
            if (not value.death_year.has_value) {
                reqs.emplace_back(RestApiUrlRequest{to_stdstring(url_of_get_death_year(key))});
            }
        }
    }
    auto resps = request(reqs);

    for (size_t i = 0; i < hashmap.capacity; ++i) {
        if (hashmap.buckets[i].has_value) {
            auto &key = hashmap.buckets[i].unwrap.key;
            auto &value = hashmap.buckets[i].unwrap.value;
            if (not value.death_year.has_value) {
                std::string url_of_get_death = to_stdstring(url_of_get_death_year(key));
                auto it = std::find_if(resps.begin(), resps.end(), [&](const RestApiUrlResponse &r){return url_of_get_death == r.url;});
                if (it != resps.end()) {
                    auto result = extract_death_year((*it).res.value());
                    if (result.has_value) {
                        while (result.unwrap.count > 0 && not isdigit(result.unwrap.data[0])) {
                            result.unwrap.chop(1);
                        }
                        auto chopped_year = result.unwrap.chop_by_delim('-');
                        value.death_year = {true, chopped_year};
                        if (verbosity) {
                            aids::println(stderr, "[FOUND DEATH YEAR]    +", value.death_year.value_or(""_sv), "\t", value.name.unwrap);
                        }
                    }
                }
            }
        }
    }
    return resps.size();    
}


int PhoneBook::find_native_tongue_in_wikidata() {
    using aids::operator""_sv;

    RestApiUrlRequests reqs{};
    for (size_t i = 0; i < hashmap.capacity; ++i) {
        if (hashmap.buckets[i].has_value) {
            auto &key = hashmap.buckets[i].unwrap.key;
            auto &value = hashmap.buckets[i].unwrap.value;
            if (not value.country.has_value) {
                reqs.emplace_back(RestApiUrlRequest{to_stdstring(url_of_get_native(key))});
                // value.name = get_name(key);
            }
        }
    }
    auto resps = request(reqs);
    for (size_t i = 0; i < hashmap.capacity; ++i) {
        if (hashmap.buckets[i].has_value) {
            auto &key = hashmap.buckets[i].unwrap.key;
            auto &value = hashmap.buckets[i].unwrap.value;
            if (not value.country.has_value) {
                std::string url_of_get_nat = to_stdstring(url_of_get_native(key));
                auto it = std::find_if(resps.begin(), resps.end(), [&](const RestApiUrlResponse &r){return url_of_get_nat == r.url;});
                if (it != resps.end()) {
                    auto result = extract_native((*it).res.value());
                    if (result.has_value) {
                        value.country = {true, {result.unwrap}};
                        if (verbosity) {
                            aids::println(stderr, "[FOUND NATIVE TONGUE] ", value.country.value_or(""_sv), "\t", value.name.unwrap);
                        }
                    }
                }
            }
        }
    }
    return resps.size();
}

int PhoneBook::find_parents_in_wikidata() {
    using aids::operator""_sv;

    bool result = {true};
    aids::Hash_Map<PersonID, Person> new_parents{};
    defer(destroy(new_parents));
    // char buf[512] = {0};
    RestApiUrlRequests reqs{};
    for (size_t i = 0; i < hashmap.capacity; ++i) {
        if (hashmap.buckets[i].has_value) {
            auto &key = hashmap.buckets[i].unwrap.key;
            // auto &value = hashmap.buckets[i].unwrap.value;
            if (not hashmap[key]->father.has_value) {
                reqs.emplace_back(RestApiUrlRequest{to_stdstring(url_of_get_father(key))});
            }
            if (not hashmap[key]->mother.has_value) {
                reqs.emplace_back(RestApiUrlRequest{to_stdstring(url_of_get_mother(key))});
            }
        }
    }
    auto resps = request(reqs);
    for (size_t i = 0; i < hashmap.capacity; ++i) {
        if (hashmap.buckets[i].has_value) {
            auto &key = hashmap.buckets[i].unwrap.key;
            // auto &value = hashmap.buckets[i].unwrap.value;
            if (key == PersonID{"UN"_sv}) {
                continue;
            }
            if (not hashmap[key]->father.has_value) {
                std::string url_of_get_fathe = to_stdstring(url_of_get_father(key));
                auto it = find_if(resps.begin(), resps.end(), [&](const RestApiUrlResponse &r){return url_of_get_fathe == r.url;});
                if (it != resps.end()) {
                    auto result = extract_p22((*it).res.value());
                    if (result.has_value && result.unwrap.data[0] == 'Q') {
                        new_parents[key]->father = {true, PersonID{{result.unwrap}}};
                    }
                }
            }
            if (not hashmap[key]->mother.has_value) {
                std::string url_of_get_mothe = to_stdstring(url_of_get_mother(key));
                auto it = find_if(resps.begin(), resps.end(), [&](const RestApiUrlResponse &r){return url_of_get_mothe == r.url;});
                if (it != resps.end()) {
                    auto result = extract_p25((*it).res.value());
                    if (result.has_value && result.unwrap.data[0] == 'Q') {
                        new_parents[key]->mother = {true, PersonID{{result.unwrap}}};
                    }
                }
            }
        }
    }   

    if (result == true) {
        for (size_t i = 0; i < new_parents.capacity; ++i) {
            if (new_parents.buckets[i].has_value) {
                auto &key = new_parents.buckets[i].unwrap.key;
                auto &value = new_parents.buckets[i].unwrap.value;
                if (key == PersonID{"UN"_sv}) {
                    continue;
                }
                if (not hashmap[key]->father.has_value) {
                    if (value.father.has_value) {
                        hashmap[key]->father = value.father;
                        if (verbosity) {
                            aids::println(stderr, "[ADD FATHER]          ", key.value, "'s father is ", value.father.unwrap.value);
                        }
                    }
                }
                if (not hashmap[key]->mother.has_value) {
                    if (value.mother.has_value) {
                        hashmap[key]->mother = value.mother;
                        if (verbosity) {
                            aids::println(stderr, "[ADD MOTHER]          ", key.value, "'s mother is ", value.mother.unwrap.value);
                        }
                    }
                }
                if (not contains(value.father)) {
                    if (value.father.has_value) {
                        *hashmap[value.father.unwrap] = Person{};
                        if (verbosity) {
                            aids::println(stderr, "[ADD NEW]             ", value.father.unwrap.value);
                        }
                    }
                }
                if (not contains(value.mother)) {
                    if (value.mother.has_value) {
                        *hashmap[value.mother.unwrap] = Person{};
                        if (verbosity) {
                            aids::println(stderr, "[ADD NEW]             ", value.mother.unwrap.value);
                        }
                    }
                }
            }
        }
    }
    return resps.size();
}

void PhoneBook::dump(aids::String_View filepath) {
    using aids::operator""_sv;
    const size_t size{filepath.count + 1};
    char *buf = (char*)malloc(filepath.count + 1);
    assert(buf);
    defer(free(buf));
    memset(buf, 0, size);
    memcpy(buf, filepath.data, size);

    FILE* file = fopen(buf, "w+");
    if (nullptr == file) {
        aids::println(stderr, "Failed to open file `", buf, "`: ", strerror(errno));
        aids::println(stderr, "Database will not be dumped.");
        return;
    }
    defer(fclose(file));
    for (size_t i = 0; i < hashmap.capacity; ++i) {
        if (hashmap.buckets[i].has_value) {
            auto &key = hashmap.buckets[i].unwrap.key;
            auto &value = hashmap.buckets[i].unwrap.value;
            aids::println(file, key.value, " : ", value.name.value_or("NN"_sv),
                    " : ", value.country.value_or(""_sv),
                    " : ", value.father.has_value ? value.father.unwrap.value : ""_sv,
                    " : ", value.mother.has_value ? value.mother.unwrap.value : ""_sv,
                    " : ", value.birth_year.value_or(""_sv),
                    " : ", value.death_year.value_or(""_sv)
            );
        }
    }
}

void PhoneBook::ahnentafel(PersonID id, size_t nr_generations) {
    aids::Dynamic_Array<PersonID> at{}; // at <-- ahnentafel
    at.push({}); // push empty
    at.push(id);
    size_t i{2};
    while(i < (size_t)(1 << nr_generations)) {
        if (i % 2 == 0) { // father
            auto tmp = hashmap[at.data[i >> 1]]->father.unwrap;
            at.push(tmp);
        } else { //mother
            auto tmp = hashmap[at.data[i >> 1]]->mother.unwrap;
            at.push(tmp);
        }
        ++i;
    }
    auto print_ahnentafel_entry = [&at](int i){
        assert(i > 0);
        aids::print(stdout, i);
        aids::print(stdout, ".\t");
        assert((size_t)i < ahnentafel_lookup_table_size);
        aids::print(stdout, ahnentafel_lookup_table[i]);
        aids::print(stdout, " ");
        aids::print(stdout, at.data[i].value);
        aids::print(stdout, '\n');
    };

    for (size_t i{1}; i < at.size; ++i) {
        print_ahnentafel_entry(i);
    }
}

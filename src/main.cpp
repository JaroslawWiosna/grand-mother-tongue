#include "10_libcurl.hpp"
#include "25_person.hpp"
#include "30_phone_book.hpp"

#include "35_libcurl.cpp"
#include "50_person.cpp"
#include "55_phone_book.cpp"

#include "../3rd_party/json.h"

#include <cassert>
#include <cstring>
#include <iostream>

std::string extract_p22_or_p25(std::string content) {
    size_t size = 64 * 1024;
    char *json = new char[size];
    memset((void *)json, '\0', size);
    memcpy(json, content.c_str(), content.size());

    struct json_value_s *root = json_parse(json, strlen(json));
    struct json_object_s *object = json_value_as_object(root);
    assert(object != NULL);
    assert(object->length == 1);

    struct json_object_element_s *claims = object->start;
    struct json_string_s *claims_name = claims->name;
    assert(0 == strcmp(claims_name->string, "claims"));
    assert(claims_name->string_size == strlen("claims"));
    std::cout << "claims name == " << claims->name->string << "\n";

    struct json_object_s *p25_object = json_value_as_object(claims->value);
    struct json_object_element_s *p25 = p25_object->start;
    struct json_string_s *p25_name = p25->name;
    // assert(0 == strcmp(p25_name->string, "P25"));
    // assert(p25_name->string_size == strlen("P25"));

    struct json_array_s *p25_array = json_value_as_array(p25->value);
    // struct json_array_element_s* p25_elem = p25_array->start->value;
    struct json_object_element_s *p25_elem =
        json_value_as_object(p25_array->start->value)->start;
    std::cout << p25_elem->name->string << "\n";

    struct json_object_s *mainsnak_object =
        json_value_as_object(p25_elem->value);
    struct json_object_element_s *mainsnak =
        mainsnak_object->start->next->next->next;
    struct json_string_s *datavalue = mainsnak->name;
    std::cout << datavalue->string << "\n";

    struct json_object_s *id_object = json_value_as_object(
        json_value_as_object(mainsnak->value)->start->value);
    struct json_object_element_s *id = id_object->start->next->next;
    std::cout << id->name->string << "\n";

    struct json_string_s *value = json_value_as_string(id->value);
    std::cout << value->string << "\n";

    std::string res{value->string};
    delete[] json;
    return res;
}

std::optional<PersonID> get_property(PersonID key, std::string property) {
    char buf[512] = {0};
    sprintf(buf,
            "https://www.wikidata.org/w/"
            "api.php?action=wbgetclaims&format=json&entity=%s&property=%s",
            key.value.c_str(), property.c_str());
    auto content = get_via_libcurl(buf);
    if (content.has_value()) {
        std::cout
            << "successfuly read json from the Internet by using libcurl\n";
    } else {
        std::cout << "libcurl failed :(\n";
        return {};
    }
    return PersonID{extract_p22_or_p25(content.value())};
}

std::optional<PersonID> get_father(PersonID key) {
    return get_property(key, "P22");
}

std::optional<PersonID> get_mother(PersonID key) {
    return get_property(key, "P25");
}

int main(int argc, char *argv[]) {
    std::cout << "Welcome to " << argv[0] << "\n";
    auto phoneBook = parse(argv[1]);

#if 0
    phoneBook.print_origin_by_blood(PersonID{"Q54058"});
    phoneBook.print_origin_by_blood(PersonID{"Q53449"});

    auto content = get_via_libcurl("https://www.wikidata.org/w/api.php?action=wbgetclaims&format=json&entity=Q54058&property=P25");
    if (content.has_value()) {
        std::cout << "successfuly read json from the Internet by using libcurl\n";
    } else {
        std::cout << "libcurl failed :(\n";
    }
#endif
    auto person01 = PersonID{"Q54058"};
    std::cout << "PersonID == " << person01.value << "\n";
    auto father = get_father(person01);
    std::cout << "\tfather == "
              << (father.has_value() ? father.value().value : "UNKNOWN")
              << "\n";
    auto mother = get_mother(person01);
    std::cout << "\tmother == "
              << (mother.has_value() ? mother.value().value : "UNKNOWN")
              << "\n";
}

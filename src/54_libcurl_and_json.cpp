#include "29_libcurl_and_json.hpp"

#include "../3rd_party/json.h"

#include <iostream>
#include <cassert>
#include <cstring>


std::optional<std::string> extract_p22_or_p25(std::string content) {
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
    if (not(0 == strcmp(claims_name->string, "claims"))) {
        return {};
    }
    assert(claims_name->string_size == strlen("claims"));
    // std::cout << "claims name == " << claims->name->string << "\n";

    struct json_object_s *p25_object = json_value_as_object(claims->value);
    if (p25_object == nullptr) {
        return {};
    }
    struct json_object_element_s *p25 = p25_object->start;
    if (p25 == nullptr) {
        return {};
    }
    struct json_string_s *p25_name = p25->name;
    if (p25_name == nullptr) {
        return {};
    }
    // assert(0 == strcmp(p25_name->string, "P25"));
    // assert(p25_name->string_size == strlen("P25"));

    struct json_array_s *p25_array = json_value_as_array(p25->value);
    // struct json_array_element_s* p25_elem = p25_array->start->value;
    struct json_object_element_s *p25_elem =
        json_value_as_object(p25_array->start->value)->start;
    // std::cout << p25_elem->name->string << "\n";

    struct json_object_s *mainsnak_object =
        json_value_as_object(p25_elem->value);
    struct json_object_element_s *mainsnak =
        mainsnak_object->start->next->next->next;
    struct json_string_s *datavalue = mainsnak->name;
    // std::cout << datavalue->string << "\n";

    struct json_object_s *id_object = json_value_as_object(
        json_value_as_object(mainsnak->value)->start->value);
    struct json_object_element_s *id = id_object->start->next->next;
    // std::cout << id->name->string << "\n";

    struct json_string_s *value = json_value_as_string(id->value);
    // std::cout << value->string << "\n";

    std::string res{value->string};
    free(root);
    delete[] json;
    if (res[0] == 'Q') {
        return res;
    }
    return {};
}

std::optional<std::string> extract_name(std::string content) {
    size_t size = 64 * 1024;
    char *json = new char[size];
    memset((void *)json, '\0', size);
    memcpy(json, content.c_str(), content.size());

    struct json_value_s *root = json_parse(json, strlen(json));
    struct json_object_s *object = json_value_as_object(root);
    assert(object != NULL);

    struct json_object_element_s *claims = object->start->next;
    struct json_string_s *claims_name = claims->name;
    assert(0 == strcmp(claims_name->string, "search"));
    // if (not (0 == strcmp(claims_name->string, "claims"))) {
    // return {};
    // }
    assert(claims_name->string_size == strlen("claims"));
    // std::cout << "claims name == " << claims->name->string << "\n";

    struct json_array_s *p25_object = json_value_as_array(claims->value);
    if (p25_object == nullptr) {
        return {};
    }
    struct json_object_element_s *p25 =
        json_value_as_object(p25_object->start->value)
            ->start->next->next->next->next->next->next;
    if (p25 == nullptr) {
        return {};
    }
    // assert(0 == strcmp(p25->name->string, "label"));
    if (not(0 == strcmp(p25->name->string, "label"))) {
        return {};
    }
    struct json_string_s *value = json_value_as_string(p25->value);
    free(root);
    delete[] json;
    std::string res{value->string};

    return res;
}

std::optional<PersonID> get_property(PersonID key, std::string property) {
    char buf[512] = {0};
    sprintf(buf,
            "https://www.wikidata.org/w/"
            "api.php?action=wbgetclaims&format=json&entity=%s&property=%s",
            key.value.c_str(), property.c_str());
    auto content = get_via_libcurl(buf);
#if 0
    if (content.has_value()) {
        std::cout
            << "successfuly read json from the Internet by using libcurl\n";
    } else {
        std::cout << "libcurl failed :(\n";
        return {};
    }
#endif
    auto result = extract_p22_or_p25(content.value());
    if (result.has_value() && result.value()[0] == 'Q') {
        return PersonID{result.value()};
    }
    return {};
}

std::optional<PersonID> get_father(PersonID key) {
    return get_property(key, "P22");
}

std::optional<PersonID> get_mother(PersonID key) {
    return get_property(key, "P25");
}

std::optional<std::string> get_name(PersonID key) {
    char buf[512] = {0};
    sprintf(buf,
            "https://www.wikidata.org/w/"
            "api.php?action=wbsearchentities"
            "&search=%s&language=en&format=json",
            key.value.c_str());
    auto content = get_via_libcurl(buf);
#if 0
    if (content.has_value()) {
        std::cout
            << "successfuly read json from the Internet by using libcurl\n";
    } else {
        std::cout << "libcurl failed :(\n";
        return {};
    }
#endif
    return extract_name(content.value());
}

//

std::string url_of_get_father(PersonID key) {
    std::string result = {"https://www.wikidata.org/w/api.php?action=wbgetclaims&format=json&entity="};
    result += key.value;
    result += "&property=";
    result += "P22";
    return result;
}

std::string url_of_get_mother(PersonID key) {
    std::string result = {"https://www.wikidata.org/w/api.php?action=wbgetclaims&format=json&entity="};
    result += key.value;
    result += "&property=";
    result += "P25";
    return result;
}

std::string url_of_get_name(PersonID key) {
    std::string result = {"https://www.wikidata.org/w/"};
    result += "api.php?action=wbsearchentities&search=";
    result += key.value;
    result += "&language=en&format=json";
    return result;
}
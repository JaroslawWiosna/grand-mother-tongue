#include "29_libcurl_and_json.hpp"

#include "../3rd_party/json.h"

#include <iostream>
#include <cassert>
#include <cstring>


aids::Maybe<aids::String_View> extract(aids::String_View content, aids::String_View property) {
    using aids::operator""_sv;

    aids::String_View query{};
    if ("P22"_sv == property) {
        query = "claims.P22[0].mainsnak.datavalue.value.id"_sv;
    } else if ("P25"_sv == property) {
        query = "claims.P25[0].mainsnak.datavalue.value.id"_sv;
    } else if ("P569"_sv == property) {
        query = "claims.P569[0].mainsnak.datavalue.value.time"_sv;
    } else if ("P570"_sv == property) {
        query = "claims.P570[0].mainsnak.datavalue.value.time"_sv;
    } else if ("name"_sv == property) {
        query = "search[0].label"_sv;
    } else {
        aids::panic("Property `", property, "` not recognized");
    }        

    size_t size = 64 * 1024;
    char *json = new char[size];
    memset((void *)json, '\0', size);
    memcpy(json, content.data, content.count);
    defer(delete[] json);

    struct json_value_s *root = json_parse(json, strlen(json));
    assert(root != NULL);
    defer(free(root));
    struct json_object_s *object = json_value_as_object(root);
    assert(object != NULL);
    assert(object->length == 1);

    struct json_object_s *wob = nullptr;
    struct json_object_element_s *wobj = nullptr;
    while (0 != query.count) {
        auto word = query.chop_by_delim('.');

        if (word.has_suffix("[0]"_sv)) {
            word.count -= 3;
            char word_buf[512] = {0};
            aids::String_Buffer sbuffer = {sizeof(word_buf), word_buf};
            aids::sprint(&sbuffer, word);

            wobj = wob->start;
            while (wobj && 0 != strcmp(wobj->name->string, word_buf)) {
                wobj = wobj->next;
            }
            assert(0 == strcmp(wobj->name->string, word_buf));

            struct json_array_s *arr = json_value_as_array(wobj->value);    
            json_object_element_s *wobj =
                    json_value_as_object(arr->start->value)->start;
            wob = json_value_as_object(wobj->value);
            auto word = query.chop_by_delim('.');
            continue;
        }

        if (not wob) { 
            wob = object;
        };
        wobj = wob->start;

        char word_buf[512] = {0};
        aids::String_Buffer sbuffer = {sizeof(word_buf), word_buf};
        aids::sprint(&sbuffer, word);

        wobj = wob->start;
        while (wobj && 0 != strcmp(wobj->name->string, word_buf)) {
            wobj = wobj->next;
        }
        assert(0 == strcmp(wobj->name->string, word_buf));
        wob = json_value_as_object(wobj->value);

    }
    struct json_string_s *value = json_value_as_string(wobj->value);
    char *buf = (char*)alloc(&region, 512);
    memset(buf, 0, 512);
    aids::String_Buffer sbuffer = {512, buf};
    aids::sprint(&sbuffer, value->string);
    assert(strlen(buf) < 500);
    return {true, {strlen(buf), buf}};
}

// cat $1 | jq .claims.P22[0].mainsnak.datavalue.value.id
// cat $1 | jq .claims.P25[0].mainsnak.datavalue.value.id
// cat $1 | jq .claims.P569[0].mainsnak.datavalue.value.time
// cat $1 | jq .claims.P570[0].mainsnak.datavalue.value.time

aids::Maybe<aids::String_View> extract_p22(std::string content) {
    using aids::operator""_sv;

    char *buf = (char*)alloc(&region, 4096);
    memset(buf, 0, 4096);
    aids::String_Buffer sbuffer = {4096, buf};
    aids::sprint(&sbuffer, content.c_str());
    return extract({strlen(buf), buf}, "P22"_sv);
}

aids::Maybe<aids::String_View> extract_p25(std::string content) {
    using aids::operator""_sv;

    char *buf = (char*)alloc(&region, 4096);
    memset(buf, 0, 4096);
    aids::String_Buffer sbuffer = {4096, buf};
    aids::sprint(&sbuffer, content.c_str());
    return extract({strlen(buf), buf}, "P25"_sv);}

aids::Maybe<aids::String_View> extract_birth_year(std::string content) {
    using aids::operator""_sv;

    char *buf = (char*)alloc(&region, 4096);
    memset(buf, 0, 4096);
    aids::String_Buffer sbuffer = {4096, buf};
    aids::sprint(&sbuffer, content.c_str());
    return extract({strlen(buf), buf}, "P569"_sv);}

aids::Maybe<aids::String_View> extract_death_year(std::string content) {
    using aids::operator""_sv;

    char *buf = (char*)alloc(&region, 4096);
    memset(buf, 0, 4096);
    aids::String_Buffer sbuffer = {4096, buf};
    aids::sprint(&sbuffer, content.c_str());
    return extract({strlen(buf), buf}, "P570"_sv);}

aids::Maybe<aids::String_View> extract_name(std::string content) {
    using aids::operator""_sv;

#if 0

    char *buf = (char*)alloc(&region, 4096);
    memset(buf, 0, 4096);
    aids::String_Buffer sbuffer = {4096, buf};
    aids::sprint(&sbuffer, content.c_str());
    return extract({strlen(buf), buf}, "name"_sv);
#endif

    size_t size = 64 * 1024;
    char *json = new char[size];
    memset((void *)json, '\0', size);
    memcpy(json, content.c_str(), content.size());
    defer(delete[] json);

    struct json_value_s *root = json_parse(json, strlen(json));
    assert(root != NULL);
    defer(free(root));
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

    char *buf = (char*)alloc(&region, 512);
    memset(buf, 0, 512);
    aids::String_Buffer sbuffer = {512, buf};
    aids::sprint(&sbuffer, value->string);
    assert(strlen(buf) < 500);
    return {true, {strlen(buf), buf}};
}

aids::Maybe<aids::String_View> extract_native(std::string content) {    
    size_t size = 64 * 1024;
    char *json = new char[size];
    memset((void *)json, '\0', size);
    memcpy(json, content.c_str(), content.size());
    defer(delete[] json);

    struct json_value_s *root = json_parse(json, strlen(json));
    assert(root != NULL);
    defer(free(root));

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

    struct json_object_s *language_object = json_value_as_object(
        json_value_as_object(mainsnak->value)->start->value);
    struct json_object_element_s *lang = language_object->start->next;

    struct json_string_s *value = json_value_as_string(lang->value);
    // std::cout << value->string << "\n";

    char *buf = (char*)alloc(&region, 512);
    memset(buf, 0, 512);
    aids::String_Buffer sbuffer = {512, buf};
    aids::sprint(&sbuffer, value->string);
    assert(strlen(buf) < 500);
    return {true, {strlen(buf), buf}};
}

aids::Maybe<PersonID> get_property_(PersonID key, std::string property) {
    char buf[512] = {0};
    aids::String_Buffer sbuffer = {sizeof(buf), buf};
    aids::sprint(&sbuffer,
            "https://www.wikidata.org/w/",
            "api.php?action=wbgetclaims&format=json&entity=",
            key.value,
            "&property=",
            property.c_str());
    auto content = get_via_libcurl(buf);
    auto result = extract_p22(content.value());
    if (result.has_value && result.unwrap.data[0] == 'Q') {
        return {true, PersonID{result.unwrap}};
    }
    return {};
}

aids::Maybe<PersonID> get_father(PersonID key) {
    char buf[512] = {0};
    aids::String_Buffer sbuffer = {sizeof(buf), buf};
    aids::sprint(&sbuffer,
            "https://www.wikidata.org/w/",
            "api.php?action=wbgetclaims&format=json&entity=",
            key.value,
            "&property=",
            "P22");
    auto content = get_via_libcurl(buf);
    auto result = extract_p22(content.value());
    if (result.has_value && result.unwrap.data[0] == 'Q') {
        return {true, PersonID{result.unwrap}};
    }
    return {};
}

aids::Maybe<PersonID> get_mother(PersonID key) {
    char buf[512] = {0};
    aids::String_Buffer sbuffer = {sizeof(buf), buf};
    aids::sprint(&sbuffer,
            "https://www.wikidata.org/w/",
            "api.php?action=wbgetclaims&format=json&entity=",
            key.value,
            "&property=",
            "P25");
    auto content = get_via_libcurl(buf);
    auto result = extract_p25(content.value());
    if (result.has_value && result.unwrap.data[0] == 'Q') {
        return {true, PersonID{result.unwrap}};
    }
    return {};
}

aids::Maybe<aids::String_View> get_name(PersonID key) {
    char buf[512] = {0};
    aids::String_Buffer sbuffer = {sizeof(buf), buf};
    aids::sprint(&sbuffer,
            "https://www.wikidata.org/w/",
            "api.php?action=wbsearchentities",
            "&search=",
            key.value,
            "&language=en&format=json");
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

aids::String_View url_of_get_father(PersonID key) {
    char *buf = (char*)alloc(&region, 512);
    memset(buf, 0, 512);
    aids::String_Buffer sbuffer = {512, buf};
    aids::sprint(&sbuffer,
    "https://www.wikidata.org/w/api.php?action=wbgetclaims&format=json&entity=",
    key.value,
    "&property=",
    "P22");
    assert(strlen(buf) < 500);
    return {strlen(buf), buf};
}

aids::String_View url_of_get_mother(PersonID key) {
    char *buf = (char*)alloc(&region, 512);
    memset(buf, 0, 512);
    aids::String_Buffer sbuffer = {512, buf};
    aids::sprint(&sbuffer,
    "https://www.wikidata.org/w/api.php?action=wbgetclaims&format=json&entity=",
    key.value,
    "&property=",
    "P25");
    assert(strlen(buf) < 500);
    return {strlen(buf), buf};
}

aids::String_View url_of_get_name(PersonID key) {
    char *buf = (char*)alloc(&region, 512);
    memset(buf, 0, 512);
    aids::String_Buffer sbuffer = {512, buf};
    aids::sprint(&sbuffer,
    "https://www.wikidata.org/w/",
    "api.php?action=wbsearchentities&search=",
    key.value,
    "&language=en&format=json");
    assert(strlen(buf) < 500);
    return {strlen(buf), buf};
}

aids::String_View url_of_get_native(PersonID key) {
    char *buf = (char*)alloc(&region, 512);
    memset(buf, 0, 512);
    aids::String_Buffer sbuffer = {512, buf};
    aids::sprint(&sbuffer,
    "https://www.wikidata.org/w/api.php?action=wbgetclaims&format=json&entity=",
    key.value,
    "&property=",
    "P1559");
    assert(strlen(buf) < 500);
    return {strlen(buf), buf};
}

aids::String_View url_of_get_birth_year(PersonID key) {
    char *buf = (char*)alloc(&region, 512);
    memset(buf, 0, 512);
    aids::String_Buffer sbuffer = {512, buf};
    aids::sprint(&sbuffer,
    "https://www.wikidata.org/w/api.php?action=wbgetclaims&format=json&entity=",
    key.value,
    "&property=",
    "P569");
    assert(strlen(buf) < 500);
    return {strlen(buf), buf};
}

aids::String_View url_of_get_death_year(PersonID key) {
    char *buf = (char*)alloc(&region, 512);
    memset(buf, 0, 512);
    aids::String_Buffer sbuffer = {512, buf};
    aids::sprint(&sbuffer,
    "https://www.wikidata.org/w/api.php?action=wbgetclaims&format=json&entity=",
    key.value,
    "&property=",
    "P570");
    assert(strlen(buf) < 500);
    return {strlen(buf), buf};
}

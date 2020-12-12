#include "41_subcmd.hpp"

// TODO(#51): subcmd_search does not support `multi-words search`
// TODO: subcmd_search prints only the first result
void subcmd_search(const char *input) {
    RestApiUrlRequests reqs{};
    std::string url = "https://www.wikidata.org/w/api.php?action=wbsearchentities&search=";
    url += input;
    url += "&language=en&format=json";
    reqs.emplace_back(RestApiUrlRequest{url});

    auto resps = request(reqs);
    // std::cout << resps[0].res.value() << '\n';

    size_t size = 64 * 1024;
    char *json = new char[size];
    memset((void *)json, '\0', size);
    memcpy(json, resps[0].res.value().c_str(), strlen(resps[0].res.value().c_str()));
    defer(delete[] json);

    struct json_value_s *root = json_parse(json, strlen(json));
    assert(root != NULL);
    defer(free(root));
    struct json_object_s *object = json_value_as_object(root);
    assert(object != NULL);

    // const int result_cnt = object->length;
    // std::cout << "result cnt == " << result_cnt << '\n';

    struct json_object_s *wob = nullptr;
    struct json_object_element_s *wobj = nullptr;

    if (not wob) { 
        wob = object;
    };
    wobj = wob->start;
    while (wobj && 0 != strcmp(wobj->name->string, "search")) {
        wobj = wobj->next;
    }
    assert(0 == strcmp(wobj->name->string, "search"));

    struct json_array_s *arr_id = json_value_as_array(wobj->value);    
    struct json_object_element_s *wobj_id = json_value_as_object(arr_id->start->value)->start;
    // struct json_object_s *wob_id = json_value_as_object(wobj_id->value);

    // std::cout << wobj_id->name->string << '\n';
    char *buf = (char*)alloc(&region, 512);
    memset(buf, 0, 512);
    aids::String_Buffer sbuffer = {512, buf, {}};
    aids::sprint(&sbuffer, json_value_as_string(wobj_id->value)->string);
//    std::cout << buf << '\n';
//    std::cout << '\n';

    std::string id = buf;

    while (wobj_id && 0 != strcmp(wobj_id->name->string, "description")) {
        wobj_id = wobj_id->next;
    }
    assert(0 == strcmp(wobj_id->name->string, "description"));
    // std::cout << json_value_as_string(wobj_id->value)->string << '\n';

    char *buf2 = (char*)alloc(&region, 512);
    memset(buf2, 0, 512);
    sbuffer = {512, buf2, {}};
    aids::sprint(&sbuffer, json_value_as_string(wobj_id->value)->string);
    // std::cout << buf2 << '\n';
    // std::cout << '\n';
    std::string desc = buf2;

    std::cout << "Result:\n";
    std::cout << id << '\t' << desc << '\n';

    exit(0);
}

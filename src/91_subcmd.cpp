#include "41_subcmd.hpp"

void subcmd_search(const char *input) {
    aids::println(stdout, "This is search handler (see #48)");

    RestApiUrlRequests reqs{};
    std::string url = "https://www.wikidata.org/w/api.php?action=wbsearchentities&search=";
    url += input;
    url += "&language=en&format=json";
    reqs.emplace_back(RestApiUrlRequest{url});

    auto resps = request(reqs);
    auto result = extract_1st_result_desc(resps[0].res.value());
    println(stdout, result);

    exit(0);
}

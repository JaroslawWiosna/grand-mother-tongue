#include "10_libcurl.hpp"
#include "25_person.hpp"
#include "29_libcurl_and_json.hpp"
#include "30_phone_book.hpp"

#include "35_libcurl.cpp"
#include "50_person.cpp"
#include "54_libcurl_and_json.cpp"
#include "55_phone_book.cpp"

#include <cassert>
#include <cstring>
#include <iostream>

namespace experimental {
struct RestApiUrlRequest {
  std::string url{};
};
struct RestApiUrlResponse {
  std::string url{};
  std::optional<std::string> res{};
  std::string *buf = nullptr;
  int httpCode{};
};
using RestApiUrlRequests = std::vector<RestApiUrlRequest>;
using RestApiUrlResponses = std::vector<RestApiUrlResponse>;

std::size_t callback(const char *in, std::size_t size, std::size_t num,
                     std::string *out) {
    const std::size_t totalBytes(size * num);
    out->append(in, totalBytes);
    return totalBytes;
}

static void add_transfer(CURLM *cm, RestApiUrlResponses responses, int i) {
    CURL *eh = curl_easy_init();
    curl_easy_setopt(eh, CURLOPT_WRITEFUNCTION, callback);
    curl_easy_setopt(eh, CURLOPT_WRITEDATA, responses[i].buf);
    curl_easy_setopt(eh, CURLOPT_URL, responses[i].url.c_str());
    curl_easy_setopt(eh, CURLOPT_PRIVATE, responses[i].url.c_str());
    curl_multi_add_handle(cm, eh);
}

RestApiUrlResponses request(const RestApiUrlRequests requests) {
    RestApiUrlResponses responses{};
    for (const auto &request : requests) {
        RestApiUrlResponse response{};
        response.url = request.url;
        response.buf = new std::string{};
        responses.push_back(response);
    }

    const int MAX_PARALLEL = (requests.size() < 8) ? requests.size() : 8;
    const int NUM_URLS = requests.size();

    CURLM *cm;
    CURLMsg *msg;
    unsigned int transfers = 0;
    int msgs_left = -1;
    int still_alive = 1;

    curl_global_init(CURL_GLOBAL_ALL);
    cm = curl_multi_init();
    curl_multi_setopt(cm, CURLMOPT_MAXCONNECTS, (long)MAX_PARALLEL);

    for (transfers = 0; transfers < MAX_PARALLEL; transfers++) {
        add_transfer(cm, responses, transfers);
    }

    do {
        curl_multi_perform(cm, &still_alive);

        while ((msg = curl_multi_info_read(cm, &msgs_left))) {
            if (msg->msg == CURLMSG_DONE) {
                char *url;
                CURL *e = msg->easy_handle;
                int httpCode{};
                curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &url);
                // curl_easy_getinfo(msg->easy_handle, CURLINFO_RESPONSE_CODE, &httpCode);
                fprintf(stderr, "R: %d - %s\n", msg->data.result,
                        curl_easy_strerror(msg->data.result));

//                if (0 == msg->data.result ) {
//                    for (auto &response : responses) {
//                        if (0 == strcmp(response.url.c_str(), url)) {
//                            response.res = std::string{*response.buf};
//                            break;
//                        }
//                    }
//                }                
//                for (auto &response : responses) {
//                    std::cout << "response.buf == " << *(response.buf) << '\n';
//                }

                curl_multi_remove_handle(cm, e);
                curl_easy_cleanup(e);
            } else {
                fprintf(stderr, "E: CURLMsg (%d)\n", msg->msg);
            }
            if (transfers < NUM_URLS)
                add_transfer(cm, responses, transfers++);
        }
        if (still_alive) {
            curl_multi_wait(cm, NULL, 0, 1000, NULL);
        }

    } while (still_alive || (transfers < NUM_URLS));

    curl_multi_cleanup(cm);
    curl_global_cleanup();

    for (auto &response : responses) {
        response.res = std::string{*response.buf};
        delete response.buf;
    }
    return responses;
}

}

int main_(void) {
  using namespace experimental;
  #if 0
  RestApiUrlRequests reqs{};
  reqs.emplace_back(RestApiUrlRequest{"https://www.wikidata.org/w/api.php?action=wbgetclaims&format=json&entity=Q53459&property=P22"});
  reqs.emplace_back(RestApiUrlRequest{"https://www.wikidata.org/w/api.php?action=wbgetclaims&format=json&entity=Q93408&property=P22"});
  reqs.emplace_back(RestApiUrlRequest{"https://www.wikidata.org/w/api.php?action=wbgetclaims&format=json&entity=Q54056&property=P22"});
  reqs.emplace_back(RestApiUrlRequest{"https://www.wikidata.org/w/api.php?action=wbgetclaims&format=json&entity=Q229434&property=P22"});
//  reqs.emplace_back(RestApiUrlRequest{"https://www.wikidata.org/w/api.php?action=wbgetclaims&format=json&entity=Q54058&property=P22"});
//  reqs.emplace_back(RestApiUrlRequest{"https://www.wikidata.org/w/api.php?action=wbgetclaims&format=json&entity=Q314514&property=P22"});
//  reqs.emplace_back(RestApiUrlRequest{"https://www.wikidata.org/w/api.php?action=wbgetclaims&format=json&entity=Q1799532&property=P22"});
//  reqs.emplace_back(RestApiUrlRequest{"https://www.wikidata.org/w/api.php?action=wbgetclaims&format=json&entity=Q53449&property=P22"});
//  reqs.emplace_back(RestApiUrlRequest{"https://www.wikidata.org/w/api.php?action=wbgetclaims&format=json&entity=Q53998&property=P22"});
//  reqs.emplace_back(RestApiUrlRequest{"https://www.wikidata.org/w/api.php?action=wbgetclaims&format=json&entity=Q231166&property=P22"});
//  reqs.emplace_back(RestApiUrlRequest{"https://www.wikidata.org/w/api.php?action=wbgetclaims&format=json&entity=Q965795&property=P22"});
//   reqs.emplace_back(RestApiUrlRequest{"https://www.wikidata.org/wiki/Special:EntityData/Q42.json"});
  auto responses = request(reqs);

  for (const auto &item : responses) {
      std::cout << item.url << "\n";
      std::cout << item.res.value_or("...") << "\n";
      std::cout << "\n";
  }
  #endif
  return {};
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
#if 0
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
#endif



    // while (phoneBook.find_parents_in_wikidata()) {
    //     std::cout << "phoneBook size == " << phoneBook.hashmap.size() <<
    //     "\n";
    // }
    // std::cout << "phoneBook size == " << phoneBook.hashmap.size() << "\n";
    phoneBook.find_parents_in_wikidata();
    phoneBook.find_names_in_wikidata();
    phoneBook.find_native_tongue_in_wikidata();
    phoneBook.dump("dumped.txt");

    // std::cout << get_name(PersonID{"Q454769"}).value_or(":/") << "\n";
    return 0;
}

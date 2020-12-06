#include "10_libcurl.hpp"

#include <curl/curl.h>

#include <memory>
#include <iostream>

// based on https://stackoverflow.com/a/10828221
// and based on https://codereview.stackexchange.com/q/53938 
bool ping(aids::String_View webpage) {
    CURL *eh = curl_easy_init();
    char buf[512] = {0};
    aids::String_Buffer sbuffer = {sizeof(buf), buf, {}};
    aids::sprint(&sbuffer, webpage);

    curl_easy_setopt(eh, CURLOPT_URL, buf);
    curl_easy_setopt(eh, CURLOPT_NOBODY, true);

    // follow "Location: " redirects
    curl_easy_setopt(eh, CURLOPT_FOLLOWLOCATION, true);

    // // return the transfer as a string
    // curl_easy_setopt(eh, CURLOPT_RETURNTRANSFER, 1);

    // disable output verbose information
    curl_easy_setopt(eh, CURLOPT_VERBOSE, false);

    // max number of seconds to allow cURL function to execute
    curl_easy_setopt(eh, CURLOPT_TIMEOUT, 5);
    auto res = curl_easy_perform(eh);
    (void)res;
    int httpCode{};
    curl_easy_getinfo(eh, CURLINFO_HTTP_CODE, &httpCode);
    curl_easy_cleanup(eh);

    if (httpCode >= 200 && httpCode < 300) {
        return true;
    } else {
        return false;
    }
}

#if 0
namespace {
std::size_t callback_(const char *in, std::size_t size, std::size_t num,
                     std::string *out) {
    const std::size_t totalBytes(size * num);
    out->append(in, totalBytes);
    return totalBytes;
}
} // namespace

static size_t write_cb(char *data, size_t n, size_t l, void *userp) {
    /* take care of the data here, ignored in this example */
    (void)data;
    (void)userp;
    return n * l;
}
#endif

// LIBCURL_MULTIPLE

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

    const unsigned int MAX_PARALLEL = (requests.size() < 8) ? requests.size() : 8;
    const unsigned int NUM_URLS = requests.size();

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
                // int httpCode{};
                curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &url);
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


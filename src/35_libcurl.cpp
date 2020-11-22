#include "10_libcurl.hpp"

#include <curl/curl.h>

#include <memory>
#include <iostream>

namespace {
std::size_t callback_(const char *in, std::size_t size, std::size_t num,
                     std::string *out) {
    const std::size_t totalBytes(size * num);
    out->append(in, totalBytes);
    return totalBytes;
}
} // namespace

std::optional<std::string> get_via_libcurl(std::string url) {
    char url_cstr[1024] = {0};
    sprintf(url_cstr, "%s", url.c_str());

    CURL *curl = curl_easy_init();

    // Set remote URL.
    curl_easy_setopt(curl, CURLOPT_URL, url_cstr);

    // Don't bother trying IPv6, which would increase DNS resolution time.
    curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);

    // Don't wait forever, time out after 10 seconds.
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);

    // Follow HTTP redirects if necessary.
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    // Response information.
    int httpCode{0};
    //    std::shared_ptr<std::string> httpData(new std::string());
    std::string *httpData = new std::string{};

    // Hook up data handling function.
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_);

    // Hook up data container (will be passed as the last parameter to the
    // callback handling function).  Can be any pointer type, since it will
    // internally be passed as a void pointer.
    // std::cout << "httpData == " << *httpData << "\n";
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData);
    // std::cout << "httpData == " << *httpData << "\n";

    // Run our HTTP GET command, capture the HTTP response code, and clean up.
    // std::cout << "httpData == " << *httpData << "\n";
    curl_easy_perform(curl);
    // std::cout << "httpData == " << *httpData << "\n";
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    // std::cout << "httpData == " << *httpData << "\n";
    curl_easy_cleanup(curl);

    if (httpCode == 200) {
        std::string tmp{*httpData};
        delete httpData;
        return tmp;
    }
    delete httpData;
    return {};
}

static size_t write_cb(char *data, size_t n, size_t l, void *userp) {
    /* take care of the data here, ignored in this example */
    (void)data;
    (void)userp;
    return n * l;
}

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


#include "10_libcurl.hpp"

#include <curl/curl.h>

#include <memory>
#include <iostream>

namespace
{
    std::size_t callback(
            const char* in,
            std::size_t size,
            std::size_t num,
            std::string* out)
    {
        const std::size_t totalBytes(size * num);
        out->append(in, totalBytes);
        return totalBytes;
    }
}

std::optional<std::string> get_via_libcurl(std::string url) {
    char url_cstr[1024] = {0};
    sprintf(url_cstr, "%s", url.c_str());

    CURL* curl = curl_easy_init();

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
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);

    // Hook up data container (will be passed as the last parameter to the
    // callback handling function).  Can be any pointer type, since it will
    // internally be passed as a void pointer.
    std::cout << "httpData == " << *httpData << "\n";
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData);
    std::cout << "httpData == " << *httpData << "\n";

    // Run our HTTP GET command, capture the HTTP response code, and clean up.
    std::cout << "httpData == " << *httpData << "\n";
    curl_easy_perform(curl);
    std::cout << "httpData == " << *httpData << "\n";
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    std::cout << "httpData == " << *httpData << "\n";
    curl_easy_cleanup(curl);

    if (httpCode == 200) {
        std::string tmp{*httpData};
        free(httpData);
        return tmp;
    }
    free(httpData);
    return {};
}

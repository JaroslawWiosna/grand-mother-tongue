#pragma once

#include "../3rd_party/aids-patched.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                    [](int ch) { return !std::isspace(ch); }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         [](int ch) { return !std::isspace(ch); })
                .base(),
            s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s) {
    ltrim(s);
    return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s) {
    rtrim(s);
    return s;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
}


// https://stackoverflow.com/a/19922123
auto load_file(std::string path) -> std::string {
    std::ifstream inFile;
    inFile.open(path);

    std::stringstream strStream;
    strStream << inFile.rdbuf();

    return strStream.str();
}

// https://stackoverflow.com/a/13172579
auto load_file_to_vector_of_lines(std::string path)
    -> std::vector<std::string> {
    std::ifstream inFile;
    inFile.open(path);

    std::stringstream strStream;
    strStream << inFile.rdbuf();
    std::string to;

    std::vector<std::string> res{};
    while (std::getline(strStream, to, '\n')) {
        res.push_back(to);
    }
    return res;
}

std::string to_stdstring(aids::String_View sv) {
    char buf[512] = {0};
    aids::String_Buffer sbuffer = {sizeof(buf), buf};
    aids::sprint(&sbuffer, sv);
    assert(strlen(buf) < 500);
    return std::string{buf};
}

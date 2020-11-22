#include "../3rd_party/aids.hpp"

#include "10_libcurl.hpp"
#include "25_person.hpp"
#include "29_libcurl_and_json.hpp"
#include "30_phone_book.hpp"

#include "60_libcurl.cpp"
#include "75_person.cpp"
#include "79_libcurl_and_json.cpp"
#include "80_phone_book.cpp"

#include <cassert>
#include <cstring>
#include <iostream>

constexpr int MAX_CURL_CALLS = 5;
static int curl_calls_cnt{};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        aids::panic("Usage: ./grand-mother-tongue <Wikidata Item>");
    }
    auto phoneBook = PhoneBook{};
    phoneBook.hashmap[PersonID{argv[1]}] = Person{};

    while (curl_calls_cnt < MAX_CURL_CALLS) {
        curl_calls_cnt += phoneBook.find_parents_in_wikidata();
        curl_calls_cnt += phoneBook.find_names_in_wikidata();
        curl_calls_cnt += phoneBook.find_native_tongue_in_wikidata();
    }
    phoneBook.dump("dumped.txt");

    phoneBook.print_origin_by_blood(PersonID{argv[1]});

    return 0;
}

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

int main(int argc, char *argv[]) {
    std::cout << "Welcome to " << argv[0] << "\n";
    auto phoneBook = parse(argv[1]);

    phoneBook.find_parents_in_wikidata();
    phoneBook.find_names_in_wikidata();
    phoneBook.find_native_tongue_in_wikidata();
    phoneBook.dump("dumped.txt");

    return 0;
}

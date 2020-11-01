#include "10_libcurl.hpp"
#include "25_person.hpp"
#include "30_phone_book.hpp"

#include "35_libcurl.cpp"
#include "50_person.cpp"
#include "55_phone_book.cpp"

#include <iostream>


int main(int argc, char *argv[]) {
    std::cout << "Welcome to " << argv[0] << "\n";
    auto phoneBook = parse(argv[1]);

    phoneBook.print_origin_by_blood(PersonID{"Q54058"});
    phoneBook.print_origin_by_blood(PersonID{"Q53449"});

    auto content = get_via_libcurl("https://www.wikidata.org/wiki/Special:EntityData/Q51058.json");
    if (content.has_value()) {
        std::cout << "successfuly read json from the Internet by using libcurl\n";
    } else {
        std::cout << "libcurl failed :(\n";
    }

}

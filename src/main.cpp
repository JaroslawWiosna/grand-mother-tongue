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

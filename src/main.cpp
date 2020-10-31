#include "25_person.hpp"
#include "30_phone_book.hpp"

#include "50_person.cpp"
#include "55_phone_book.cpp"

#include <iostream>


int main(int argc, char *argv[]) {
    std::cout << "Welcome to " << argv[0] << "\n";
    auto phoneBook = parse(argv[1]);
}

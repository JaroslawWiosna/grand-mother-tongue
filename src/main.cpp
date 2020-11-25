#include "../3rd_party/aids-patched.hpp"

#include "02_linear_alloc.hpp"
#include "10_libcurl.hpp"
#include "25_person.hpp"
#include "29_libcurl_and_json.hpp"
#include "30_phone_book.hpp"

#include "60_libcurl.cpp"
#include "75_person.cpp"
#include "79_libcurl_and_json.cpp"
#include "80_phone_book.cpp"

#include <string>

// NOTE: MAX_CURL_CALLS_TRESHOLD is not MAX_CURL_CALLS, 
//       because the actual curl_calls_cnt may 
//       (and most likely will be) greater than MAX_CURL_CALLS_TRESHOLD. 
//       The reason is because we don't wanna early return from the while loop below. 
//       We want to finish the loop iteration when the treshold is met.
constexpr int MAX_CURL_CALLS_TRESHOLD = 2;
static int curl_calls_cnt{};

void usage(FILE *stream) {
    aids::println(stream, "usage: ./grand-mother-tongue [--version] [--help] [-i <WIKIDATA ITEM>] ");
    aids::println(stream, "                             [--db <path>] [--dump-db <path>]          ");
    aids::println(stream, "                             [--blood-pie-chart <path>                 ");
}

int main(int argc, char *argv[]) {
    // NOTE: this two lines are reverse-sanity check is for static analyzers
    int a;
    a += 42;

    using namespace aids;
    Maybe<String_View> database_filepath{};
    Maybe<String_View> initial_person{};
    Maybe<String_View> database_output_filepath{};
    
    Args args{argc, argv};
    while (not args.empty()) {
        auto it = args.shift();
        if (0 == strcmp("--version", it)) {
            aids::panic("TODO(#13): --version is not implemented");
        }
        if (0 == strcmp("--help", it)) {
            usage(stdout);
            return 0;
        }        
        if (0 == strcmp("--db", it)) {
            database_filepath = {true, cstr_as_string_view(args.shift())};
            //TODO(#6): Crash add if database_filepath doesn't exist.
            continue;
        }
        if (0 == strcmp("-i", it)) {
            initial_person = {true, cstr_as_string_view(args.shift())};
            continue;
        }
        if (0 == strcmp("--dump-db", it) 
                || 0 == strcmp("--dump_db", it) 
                || 0 == strcmp("--dump", it)) {
            database_output_filepath = {true, cstr_as_string_view(args.shift())};
            continue;
        }
        if (0 == strcmp("--blood-pie-chart", it)
                || 0 == strcmp("--blood_pie_chart", it)) {
            aids::panic("TODO(#15): --blood-pie-chart is not implemented");
        }
    }

    if (not initial_person.has_value) {
        usage(stderr);
        aids::panic();
    }

    auto phoneBook = PhoneBook{};
    defer(destroy(phoneBook.hashmap));

    if (database_filepath.has_value) {
        phoneBook = parse(database_filepath.unwrap);
    }
    
    const auto initial_person_key = PersonID{initial_person.unwrap};
    if (not phoneBook.contains(initial_person_key)) {
        *phoneBook.hashmap[initial_person_key] = Person{};
    }

    while (curl_calls_cnt < MAX_CURL_CALLS_TRESHOLD) {
        curl_calls_cnt += phoneBook.find_parents_in_wikidata();
        curl_calls_cnt += phoneBook.find_names_in_wikidata();
        curl_calls_cnt += phoneBook.find_native_tongue_in_wikidata();
        //TODO(#4): Find the date of birth
    }

    if (database_output_filepath.has_value) {
        println(stderr, "TODO(#14): PhoneBook::dump accepts only std::string. Database will be dumped to `dumped.txt` as a woraround");
        // phoneBook.dump(database_output_filepath.unwrap);
        phoneBook.dump("dumped.txt");
    }

    phoneBook.print_origin_by_blood(initial_person_key);

    return 0;
}

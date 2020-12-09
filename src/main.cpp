#include "../3rd_party/aids-patched.hpp"

#include "version.cpp"
#include "02_linear_alloc.hpp"
#include "03_ahnentafel_lookup_table.hpp"
#include "04_shared_ptr.hpp"
#include "10_libcurl.hpp"
#include "25_person.hpp"
#include "29_libcurl_and_json.hpp"
#include "30_phone_book.hpp"

#include "60_libcurl.cpp"
#include "75_person.cpp"
#include "79_libcurl_and_json.cpp"
#include "80_phone_book.cpp"
#include "98_tests.cpp"

#include <string>

// NOTE: MAX_CURL_CALLS_TRESHOLD is not MAX_CURL_CALLS, 
//       because the actual curl_calls_cnt may 
//       (and most likely will be) greater than MAX_CURL_CALLS_TRESHOLD. 
//       The reason is because we don't wanna early return from the while loop below. 
//       We want to finish the loop iteration when the treshold is met.
constexpr int MAX_CURL_CALLS_TRESHOLD = 2000;
static int curl_calls_cnt{};

void usage(FILE *stream) {
    aids::println(stream, "usage: ./grand-mother-tongue [--version] [--help] [-i <WIKIDATA ITEM>] ");
    aids::println(stream, "                             [-g N] [--db <path>] [--dump-db <path>]   ");
    aids::println(stream, "                             [--blood-plot <path>] [-v]");
    aids::println(stream, "                             [--ahnentafel N]");
}

int main(int argc, char *argv[]) {
#ifdef GRAND_MOTHER_TONGUE_TEST
    test();
    return 0;
#endif

    using namespace aids;
    Maybe<String_View> database_filepath{};
    Maybe<String_View> initial_person{};
    Maybe<String_View> database_output_filepath{};
    Maybe<size_t> max_gen{};
    Maybe<String_View> blood_pie_chart_output_filepath{};
    Maybe<size_t> ahnentafel{};
    
    Args args{argc, argv};
    while (not args.empty()) {
        auto it = args.shift();
        if (0 == strcmp("--version", it)) {
            println(stdout, version);
            return 0;
        }
        if (0 == strcmp("--help", it)) {
            usage(stdout);
            return 0;
        }        
        if (0 == strcmp("--db", it)) {
            char *it = args.shift();
            if (not file_exists(it)) {
                aids::panic(it, " does not exist");
            }
            database_filepath = {true, cstr_as_string_view(it)};
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
        if (0 == strcmp("--blood-plot", it)
                || 0 == strcmp("--blood_plot", it)) {
            blood_pie_chart_output_filepath = {true, cstr_as_string_view(args.shift())};
        }
        if (0 == strcmp("-g", it)
                || 0 == strcmp("--max_generations", it)
                || 0 == strcmp("--max-generations", it)) {
            String_View sv = cstr_as_string_view(args.shift());
            max_gen = sv.as_integer<size_t>();
            continue;
        }
        if (0 == strcmp("-v", it)
                || 0 == strcmp("--verbose", it)) {
            // auto sv = cstr_as_string_view(args.shift());
            // verbosity = sv.as_integer<size_t>().unwrap;
            verbosity = 1;
            continue;
        }
        if (0 == strcmp("--ahnentafel", it)) {
            String_View sv = cstr_as_string_view(args.shift());
            auto num = sv.as_integer<size_t>();
            assert(num.has_value);
            ahnentafel = {true, num.unwrap};
            continue;
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

    size_t current_gen{};
    constexpr const size_t GEN_LIMIT{1000};
    while (curl_calls_cnt < MAX_CURL_CALLS_TRESHOLD && current_gen < max_gen.value_or(GEN_LIMIT)) {
        if (not ping("https://www.wikidata.org/w/api.php"_sv)) {
            aids::panic("No Interget connection. Aborting...");
        }
        curl_calls_cnt += phoneBook.find_parents_in_wikidata();
        curl_calls_cnt += phoneBook.find_names_in_wikidata();
        curl_calls_cnt += phoneBook.find_native_tongue_in_wikidata();
        // curl_calls_cnt += phoneBook.find_birth_year();
        // curl_calls_cnt += phoneBook.find_death_year();

        current_gen++;
    }

    if (database_output_filepath.has_value) {
        phoneBook.dump(database_output_filepath.unwrap);
    }

    phoneBook.print_origin_by_blood(initial_person_key);

    if (ahnentafel.has_value) {
        phoneBook.ahnentafel(initial_person_key, ahnentafel.unwrap);
    }

    if (blood_pie_chart_output_filepath.has_value) {
        using aids::operator""_sv;

        auto res = phoneBook.origin_by_blood(initial_person_key);
        defer(destroy(res.map));
        {
            FILE* file = fopen("data.dat", "w+");
            if (nullptr == file) {
                aids::panic("Failed to open file `data.dat`: ", strerror(errno));            
            }
            defer(fclose(file));

            size_t data_entry_cnt{};
            for (size_t i = 0; i < res.map.capacity; ++i) {
                if (res.map.buckets[i].has_value) {
                    // auto &key = res.map.buckets[i].unwrap.key;
                    // auto &value = res.map.buckets[i].unwrap.value;
                    aids::println(file, data_entry_cnt++, " ", res.map.buckets[i].unwrap.key, " ", (float)res.map.buckets[i].unwrap.value);
                }
            }
        }

        FILE *gnuplot = popen("gnuplot -p", "w");
        // TODO(#33): bars in histogram are not labelled by value
        //       https://stackoverflow.com/questions/43715496/gnuplot-histogram-label-values
        const   char* const gnuplot_cmds = 
        "reset\n"
        "set term png\n"
        "set output '%.*s'\n"
        "set boxwidth 0.5\n"
        "set datafile separator whitespace\n"
        "set style fill solid\n"
        "set zeroaxis\n"
        "plot 'data.dat' using 1:3:xtic(2) with boxes\n";
        fprintf(gnuplot, 
                gnuplot_cmds, 
                blood_pie_chart_output_filepath.unwrap.count, 
                blood_pie_chart_output_filepath.unwrap.data);
        fflush(gnuplot);
        pclose(gnuplot);
    }

    return 0;
}

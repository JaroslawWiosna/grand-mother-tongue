//TODO(#43): Consider moving `ahnentafel_lookup_table_generator` implementation to source code of `grand-mother-tongue`
// I really like static const array of cstrings because there is no need of memory manage it.
// I do realize that having a separate binary is an overkill, but I like compite time array, so I quite enjoy this.
// This low-priority todo is a simply note for myself, that perhaps `get_relation` could be implemented cleaner.

#define AIDS_IMPLEMENTATION
#include "../3rd_party/aids-patched.hpp"

using namespace aids;
int main(int argc, char *argv[]) {
    if (argc != 2) {
        panic();
    }
    auto limit = String_View{strlen(argv[1]), argv[1]}.as_integer<size_t>();
    if (not limit.has_value) {
        panic();
    }

    auto get_relation = [](size_t i) -> void{
        const char *relation[] = {
            "",
            "Probant",
            "father",
            "mother",
            "paternal grandfather",
            "paternal grandmother",
            "maternal grandfather",
            "maternal grandmother",
        };
        constexpr size_t relation_size = (sizeof(relation) / sizeof(relation[0]));
        static_assert(8 == relation_size);

        if (i < relation_size) {
            aids::print(stdout, relation[i]);
        } else {
            // 1000 -> drop first digit, then '0' is father, '1' is mother
            int it{};
            
            char buf[64] = {'\0'};
            int buf_size{};
            while(i != 1) {
                if (i % 2 == 0) {
                    buf[it++] = 'f'; // father
                } else {
                    buf[it++] = 'm'; // mother
                }
                buf_size++;
                i /= 2;
            }
            while (buf_size > 0) {
                if (buf[buf_size-- - 1] == 'f') {
                    aids::print(stdout, "father");
                } else {
                    aids::print(stdout, "mother");
                }
                if (buf_size != 0) {
                    aids::print(stdout, "'s ");
                } else {
                    break;
                }
            }
        }
    };

    auto print_ahnentafel_entry = [&get_relation](int i){
        aids::print(stdout, '"');
        get_relation(i);
        aids::print(stdout, '"');
        aids::print(stdout, ",");
        aids::print(stdout, '\n');
    };

    //////////////////////

    println(stdout, "#pragma once"); 
    println(stdout, "constexpr const char * const ahnentafel_lookup_table[] = {"); 
    for (size_t i{}; i < limit.unwrap; ++i) {
        print_ahnentafel_entry(i);
    }
    println(stdout, "};"); 
    println(stdout); 
    
    
    println(stdout, "constexpr const size_t ahnentafel_lookup_table_size = (sizeof(ahnentafel_lookup_table) / sizeof(ahnentafel_lookup_table[0]));");
    println(stdout, "static_assert(", limit.unwrap ," == ahnentafel_lookup_table_size);");

}



void test() {
    using aids::operator""_sv;
    {
        auto Q53449_P22 = aids::read_file_as_string_view("./wikidata-json-examples/Q53449_P22.json");
        if (not Q53449_P22.has_value) {
            aids::panic("No such file `../wikidata-json-examples/Q53449_P22.json`");
        }    

        auto result = extract(Q53449_P22.unwrap, "P22"_sv);
        assert("Q53998"_sv == result.unwrap);
    }    
    {
        auto Q53449_P25 = aids::read_file_as_string_view("./wikidata-json-examples/Q53449_P25.json");
        if (not Q53449_P25.has_value) {
            aids::panic("No such file `../wikidata-json-examples/Q53449_P25.json`");
        }    

        auto result = extract(Q53449_P25.unwrap, "P25"_sv);
        assert("Q231166"_sv == result.unwrap);
    }    
    {
        auto Q53449_P569 = aids::read_file_as_string_view("./wikidata-json-examples/Q53449_P569.json");
        if (not Q53449_P569.has_value) {
            aids::panic("No such file `../wikidata-json-examples/Q53449_P569.json`");
        }    

        auto result = extract(Q53449_P569.unwrap, "P569"_sv);
        assert("+1374-02-18T00:00:00Z"_sv == result.unwrap);
    }    
    {
        auto Q53449_P570 = aids::read_file_as_string_view("./wikidata-json-examples/Q53449_P570.json");
        if (not Q53449_P570.has_value) {
            aids::panic("No such file `../wikidata-json-examples/Q53449_P570.json`");
        }    

        auto result = extract(Q53449_P570.unwrap, "P570"_sv);
        assert("+1399-07-17T00:00:00Z"_sv == result.unwrap);
    }    
    aids::println(stdout, "All good");
}

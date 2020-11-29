

void test() {
    using aids::operator""_sv;

    auto Q53449_P22 = aids::read_file_as_string_view("./wikidata-json-examples/Q53449_P22.json");
    if (not Q53449_P22.has_value) {
        aids::panic("No such file `../wikidata-json-examples/Q53449_P22.json`");
    }    

    auto result = extract(Q53449_P22.unwrap, "P22"_sv);
    assert("Q53998"_sv == result.unwrap);

    aids::println(stdout, "All good");
}

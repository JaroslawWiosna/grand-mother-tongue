

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
        if (not ("+1374-02-18T00:00:00Z"_sv == result.unwrap)) {
            aids::panic("+1374-02-18T00:00:00Z"_sv, " != ", result.unwrap);
        }
    }    
    {
        auto Q53449_P570 = aids::read_file_as_string_view("./wikidata-json-examples/Q53449_P570.json");
        if (not Q53449_P570.has_value) {
            aids::panic("No such file `../wikidata-json-examples/Q53449_P570.json`");
        }    

        auto result = extract(Q53449_P570.unwrap, "P570"_sv);
        assert("+1399-07-17T00:00:00Z"_sv == result.unwrap);
    }
    {
        auto foo = [](Shared_ptr<aids::Dynamic_Array<int>> sp) {
            auto cnt = sp.use_count();
            return cnt;
        };
        auto bar = [](Shared_ptr<aids::Dynamic_Array<int>> *sp) {
            auto cnt = sp->use_count();
            return cnt;
        };
        
        {
            aids::Dynamic_Array<int> da{};
            da.push(100);
            da.push(99);
            assert(2 == da.size);
            aids::destroy(&da);
            assert(0 == da.size);
        }
        aids::Dynamic_Array<int> da{};
        da.push(100);
        da.push(99);
        {
            auto sp1 = make_shared(&da);
            assert(1 == sp1.use_count());
            {
                auto sp2 = copy_shared_ptr(sp1);
                assert(2 == sp1.use_count());
                assert(2 == sp2.use_count());
                {
                    auto use_cnt_inside_lambda_by_value = foo(sp1);
                    assert((size_t)3 == use_cnt_inside_lambda_by_value);
                    assert(2 == sp1.use_count());
                    assert(2 == sp2.use_count());
                }
                {
                    auto use_cnt_inside_lambda_by_ptr = bar(&sp1);
                    assert((size_t)2 == use_cnt_inside_lambda_by_ptr);
                    assert(2 == sp1.use_count());
                    assert(2 == sp2.use_count());
                }
                sp1.get()->push(98);
                sp2.get()->push(97);
                assert(4 == sp1.get()->size);
                assert(4 == sp2.get()->size);
            }
            assert(1 == sp1.use_count());
            assert(4 == da.size);
        }
        assert(0 == da.size);
    }    
    aids::println(stdout, "All good");
}

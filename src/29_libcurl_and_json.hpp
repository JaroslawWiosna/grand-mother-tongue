#pragma once

aids::Maybe<aids::String_View> extract_p22(std::string content);
aids::Maybe<aids::String_View> extract_p25(std::string content);
aids::Maybe<PersonID> get_property(PersonID key, std::string property);
aids::Maybe<PersonID> get_father(PersonID key);
aids::Maybe<PersonID> get_mother(PersonID key);

aids::String_View url_of_get_father(PersonID key);
aids::String_View url_of_get_mother(PersonID key);
aids::String_View url_of_get_name(PersonID key);
aids::String_View url_of_get_native(PersonID key);

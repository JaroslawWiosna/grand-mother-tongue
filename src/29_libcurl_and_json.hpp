#pragma once

std::optional<std::string> extract_p22_or_p25(std::string content);
std::optional<PersonID> get_property(PersonID key, std::string property);
std::optional<PersonID> get_father(PersonID key);
std::optional<PersonID> get_mother(PersonID key);

std::string url_of_get_father(PersonID key);
std::string url_of_get_mother(PersonID key);
std::string url_of_get_name(PersonID key);
std::string url_of_get_native(PersonID key);

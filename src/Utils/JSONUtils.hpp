#ifndef JSONUTILS_HPP
#define JSONUTILS_HPP

#include <nlohmann/json.hpp>

// if the file does not exist, it returns an empty json
nlohmann::json LoadJSONFromFile(const std::string& filepath);

void SaveJSONToFile(const std::string& filepath, const nlohmann::json& data);

#endif
#include "JSONUtils.hpp"

#include <fstream>
#include <iostream>

nlohmann::json LoadJSONFromFile(const std::string& filepath)
{
	std::ifstream json_file(filepath);

	if (json_file.is_open() == false)
	{
		std::cerr << "Could not open the file \"" << filepath << "\"." << std::endl;
		return nlohmann::json {};
	}

	nlohmann::json json_data;
	try
	{
		json_file >> json_data;
	}
	catch (const nlohmann::json::parse_error& e)
	{
		std::cerr << "Could not parse JSON file \"" << filepath << "\": " << e.what() << std::endl;
		json_data = nlohmann::json {};
	}

	json_file.close();

	return json_data;
}

void SaveJSONToFile(const std::string& filepath, const nlohmann::json& data)
{
	std::ofstream output_file(filepath);

	if (output_file.is_open() == false)
	{
		std::cerr << "Could not open the file \"" << filepath << "\"." << std::endl;
		return;
	}

	try
	{
		output_file << data.dump(4);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Could not write JSON data to file \"" << filepath << "\": " << e.what() << std::endl;
	}

	output_file.close();
}
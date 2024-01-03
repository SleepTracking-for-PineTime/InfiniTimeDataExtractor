#include "csv_handler.hpp"

#include <fstream>

CSVHandler::CSVHandler()
{
}

CSVHandler::~CSVHandler()
{
}

void CSVHandler::AppendCSVLine(std::string file_name, std::vector<std::string> line_content)
{
    std::ofstream csv_file(file_name, std::ios::app);
    
    for (size_t i = 0; i < line_content.size(); i++)
    {
        csv_file << line_content[i];

        if (i != line_content.size()-1)
            csv_file << ",";
    }
    csv_file << std::endl;

    csv_file.close();
}

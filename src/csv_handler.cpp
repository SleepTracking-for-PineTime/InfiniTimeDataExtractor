#include "csv_handler.hpp"

CSVHandler::CSVHandler(std::string file_name) :
    csv_file(file_name, std::ios::app)
{
}

CSVHandler::~CSVHandler()
{
    csv_file.close();
}

void CSVHandler::AppendCSVLine(std::vector<std::string> line_content)
{
    for (size_t i = 0; i < line_content.size(); i++)
    {
        csv_file << line_content[i];

        if (i != line_content.size()-1)
            csv_file << ",";
    }
    csv_file << std::endl;
}

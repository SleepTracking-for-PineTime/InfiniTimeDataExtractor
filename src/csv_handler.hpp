#pragma once

#include <string>
#include <vector>
#include <fstream>

class CSVHandler
{
public:
    CSVHandler(std::string file_name);
    ~CSVHandler();

    void AppendCSVLine(std::vector<std::string> line_content);

private:
    std::ofstream csv_file;
};

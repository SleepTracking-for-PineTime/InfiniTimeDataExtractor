#pragma once

#include <string>
#include <vector>

class CSVHandler
{
public:
    CSVHandler();
    ~CSVHandler();

    void AppendCSVLine(std::string file_name, std::vector<std::string> line_content);
};
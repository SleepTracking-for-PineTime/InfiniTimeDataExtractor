#include "data_extractor.hpp"

int main(int argc, char** argv)
{
    std::string file_name = "data.csv";
    if (argc > 1)
        file_name = argv[1];

    int data_write_loop_delay = 1;
    if (argc > 2)
        data_write_loop_delay = argv[2][0] - '0';

    DataExtractor data_extractor(file_name, data_write_loop_delay);

    if (1 == data_extractor.Init())
        return 1;

    data_extractor.StartDataExtraction();

    return 0;
}

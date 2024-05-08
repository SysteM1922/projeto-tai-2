#include <iostream>
#include <string>
#include <getopt.h>
#include <stdio.h>
#include <cmath>
#include <filesystem>

#include "table.hpp"

using namespace std;
namespace fs = std::filesystem;

void read_file_to_memory(FILE *file, char *&targetData, int &fileSize)
{
    fseek(file, 0, SEEK_END);
    fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    targetData = (char *)malloc(fileSize * sizeof(char));
    if (targetData == NULL)
    {
        cerr << "Error allocating memory for file." << endl;
        exit(1);
    }
    if (static_cast<int>(fread(targetData, sizeof(char), fileSize, file)) != fileSize)
    {
        cerr << "Error reading input file." << endl;
        exit(1);
    }
    fclose(file);
}

double compress_from_table(Table table, size_t sequence_size, char *data, int fileSize)
{
    double total = 0;
    double prob = 0;

    string sequence(sequence_size, ' ');

    for (int i = 0; i < fileSize; i++)
    {
        if (sequence.size() < sequence_size)
        {
            sequence += data[i];
            continue;
        }
        table.calcProbability(sequence, data[i], prob);
        total -= log2(prob);
        for (size_t j = 0; j < sequence_size - 1; j++)
        {
            sequence[j] = sequence[j + 1];
        }
        sequence[sequence_size - 1] = data[i];
    }
    return total;
};

int main(int argc, char *argv[])
{
    size_t sequence_size = 1;
    FILE *humman_collection = NULL;
    FILE *chatted_collection = NULL;
    FILE *file_to_be_analyzed = NULL;
    int alpha = 0;
    bool interactive = false;
    string dir = "";

    if (argc < 10)
    {
        cerr << "Usage: " << argv[0] << " -a <alpha> -n <sequence_size> -h <human_collection> -c <chatted_collection> -f <file_to_be_analyzed> -d <directory> -i (iteractive mode)" << endl;
        return 1;
    }

    int opt;
    while ((opt = getopt(argc, argv, "a:n:h:c:f:d:i")) != -1)
    {
        switch (opt)
        {
        case 'a':
            alpha = atoi(optarg);
            break;
        case 'n':
            sequence_size = atoi(optarg);
            if (sequence_size < 1)
            {
                cerr << "Error sequence_size must be greater than 0" << endl;
                return 1;
            }

            break;
        case 'h':
            humman_collection = fopen(optarg, "r");
            if (humman_collection == NULL)
            {
                cerr << "Error opening humman collection file" << endl;
                return 1;
            }
            break;
        case 'c':
            chatted_collection = fopen(optarg, "r");
            if (chatted_collection == NULL)
            {
                cerr << "Error opening chatted collection file" << endl;
                return 1;
            }
            break;
        case 'f':
            if (interactive)
            {
                cerr << "Error: file mode is not compatible with interactive mode" << endl;
                return 1;
            }
            if (dir != "")
            {
                cerr << "Error: file mode is not compatible with directory mode" << endl;
                return 1;
            }
            file_to_be_analyzed = fopen(optarg, "r");
            if (file_to_be_analyzed == NULL)
            {
                cerr << "Error opening file to be analyzed" << endl;
                return 1;
            }
            break;
        case 'd':
            if (interactive)
            {
                cerr << "Error: directory mode is not compatible with interactive mode" << endl;
                return 1;
            }
            if (file_to_be_analyzed != NULL)
            {
                cerr << "Error: directory mode is not compatible with file mode" << endl;
                return 1;
            }
            dir = optarg;
            break;
        case 'i':
            if (dir != "")
            {
                cerr << "Error: interactive mode is not compatible with directory mode" << endl;
                return 1;
            }
            if (file_to_be_analyzed != NULL)
            {
                cerr << "Error: interactive mode is not compatible with file mode" << endl;
                return 1;
            }
            interactive = true;
            break;
        default:
            cerr << "Usage: " << argv[0] << " -a <alpha> -n <sequence_size> -h <human_collection> -c <chatted_collection> -f <file_to_be_analyzed> -d <directory> -i (iteractive mode)" << endl;
            return 1;
        }
    }

    Table human_compression_table = Table(alpha);
    Table chatted_compression_table = Table(alpha);

    read_for_table(humman_collection, human_compression_table, sequence_size, "human collection");
    read_for_table(chatted_collection, chatted_compression_table, sequence_size, "chatted collection");

    cout << "Human compression table memory usage: " << human_compression_table.memorySize() / 1024 << " KB" << endl;
    cout << "Chatted compression table memory usage: " << chatted_compression_table.memorySize() / 1024 << " KB" << endl
         << endl;

    cout << "Checking the nature of the file submitted for analysis ..." << endl;

    char *data;
    int fileSize = 0;
    double human_compression = 0;
    double chatted_compression = 0;
    if (file_to_be_analyzed != NULL)
    {
        read_file_to_memory(file_to_be_analyzed, data, fileSize);
        human_compression = compress_from_table(human_compression_table, sequence_size, data, fileSize);
        chatted_compression = compress_from_table(chatted_compression_table, sequence_size, data, fileSize);

        if (human_compression < chatted_compression)
        {
            cout << "The file submitted for analysis was Human written!" << endl;
        }
        else
        {
            cout << "The file submitted for analysis was generated by ChatGPT!" << endl;
        }

        free(data);
    }
    else if (dir != "")
    {
        cout << "Is GPT?\t| File Name" << endl;
        for (const auto &entry : fs::directory_iterator(dir))
        {
            FILE *file_to_be_analyzed = fopen(entry.path().c_str(), "r");
            if (file_to_be_analyzed == NULL)
            {
                cerr << "Error opening " << entry.path().c_str() << endl;
                return 1;
            }
            read_file_to_memory(file_to_be_analyzed, data, fileSize);

            human_compression = compress_from_table(human_compression_table, sequence_size, data, fileSize);
            chatted_compression = compress_from_table(chatted_compression_table, sequence_size, data, fileSize);

            if (human_compression > chatted_compression)
            {
                cout << "    1\t| " << entry.path().filename().c_str() << endl;
            }
            else
            {
                cout << "    0\t| " << entry.path().filename().c_str() << endl;
            }
            free(data);
        }

        human_compression_table.clear();
        chatted_compression_table.clear();
    }
    else if (interactive)
    {
        bool active = true;
        string file_name;
        while (active)
        {
            cout << endl
                 << "Enter the file name to be analyzed (or 'exit' to quit): ";
            cin >> file_name;
            if (file_name == "exit")
            {
                active = false;
                continue;
            }
            file_to_be_analyzed = fopen(file_name.c_str(), "r");
            if (file_to_be_analyzed == NULL)
            {
                cerr << "Error opening file to be analyzed" << endl;
                return 1;
            }
            read_file_to_memory(file_to_be_analyzed, data, fileSize);
            human_compression = compress_from_table(human_compression_table, sequence_size, data, fileSize);
            chatted_compression = compress_from_table(chatted_compression_table, sequence_size, data, fileSize);

            if (human_compression < chatted_compression)
            {
                cout << "The file submitted for analysis was Human written!" << endl;
            }
            else
            {
                cout << "The file submitted for analysis was generated by ChatGPT!" << endl;
            }
            free(data);
        }
    }

    human_compression_table.clear();
    chatted_compression_table.clear();

    return 0;
}
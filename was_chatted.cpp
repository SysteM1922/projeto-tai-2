#include <iostream>
#include <string>
#include <getopt.h>
#include <vector>
#include <sys/stat.h>

#include "table.h"

using namespace std;

bool directoryExists(const char *path)
{
    struct stat info;
    return stat(path, &info) == 0 && S_ISDIR(info.st_mode);
}

int main(int argc, char *argv[])
{
    size_t sequence_size = 1;
    FILE *humman_collection = NULL;
    FILE *chatted_collection = NULL;
    FILE *file_to_be_analyzed = NULL;

    if (argc < 9)
    {
        cerr << "Usage: " << argv[0] << " -n <sequence_size> -h <human_collection> -c <chatted_collection> -f <file_to_be_analyzed>" << endl;
        return 1;
    }

    int opt;
    while ((opt = getopt(argc, argv, "n:h:c:f:s:l:")) != -1)
    {
        switch (opt)
        {
        case 'n':
            sequence_size = atoi(optarg);
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
            file_to_be_analyzed = fopen(optarg, "r");
            if (file_to_be_analyzed == NULL)
            {
                cerr << "Error opening file to be analyzed" << endl;
                return 1;
            }
            break;
        default:
            cerr << "Usage: " << argv[0] << " -n <sequence_size> -h <human_collection> -c <chatted_collection> -f <file_to_be_analyzed>" << endl;
            return 1;
        }
    }

    Table human_compression_table;
    Table chatted_compression_table;

    read_for_table(humman_collection, human_compression_table, sequence_size + 1, "human collection");
    read_for_table(chatted_collection, chatted_compression_table, sequence_size + 1, "chatted collection");

    cout << "Human compression table memory usage: " << human_compression_table.memorySize() / 1024 << " KB" << endl;
    cout << "Chatted compression table memory usage: " << chatted_compression_table.memorySize() / 1024 << " KB" << endl;

    return 0;
}
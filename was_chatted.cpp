#include <iostream>
#include <fstream>
#include <string>
#include <getopt.h>
#include <vector>

#include <unordered_map>

#define BUFFER_SIZE 20 * 1024 * 1024

using namespace std;

void progressBar(int progress, int total) {
    float percent = static_cast<float>(progress) / total * 100;

    int barWidth = 50;
    int numChars = static_cast<int>(percent / 100 * barWidth);

    cout << "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < numChars)
            cout << "=";
        else
            cout << " ";
    }
    cout << "] " << static_cast<int>(percent) << "%\r";
    cout.flush();
}

void read_for_table(FILE *file, unordered_map<string, int> &table, size_t sequence_size, string label)
{
    size_t buffer_size;
    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size == 0)
    {
        cerr << "Error: file is empty" << endl;
        return;
    }

    if (file_size > BUFFER_SIZE)
    {
        buffer_size = BUFFER_SIZE;
    }
    else
    {
        buffer_size = file_size;
    }

    vector<char> buffer(buffer_size);
    string sequence(sequence_size, ' ');

    int percent = 0;
    cout << "Processing file " << label << "..." << endl;
    progressBar(percent, 100);

    while (fread(&buffer[0], sizeof(char), buffer_size, file) == buffer_size)
    {
        for (size_t i = 0; i < buffer_size; i++)
        {
            if (buffer[i] == '\0')
            {
                sequence = "";
                continue;
            }
            if (sequence.size() < sequence_size)
            {
                sequence += buffer[i];
                continue;
            }
            for (size_t j = 0; j < sequence_size - 1; j++)
            {
                sequence[j] = sequence[j + 1];
            }
            sequence[sequence_size - 1] = buffer[i];
            table[sequence]++;
        }
        percent += 100 * buffer_size / file_size;
        progressBar(percent, 100);
    }

    size_t remaining_bytes = file_size % buffer_size;
    if (remaining_bytes > 0)
    {
        for (size_t i = 0; i < remaining_bytes; i++)
        {
            if (buffer[i] == '\0')
            {
                sequence = "";
                continue;
            }
            if (sequence.size() < sequence_size)
            {
                sequence += buffer[i];
                continue;
            }
            sequence = sequence.substr(1) + buffer[i];
            table[sequence]++;
        }
    }
    progressBar(100, 100);
    cout << endl;
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
    while ((opt = getopt(argc, argv, "n:h:c:f:")) != -1)
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

    unordered_map<string, int> human_compression_table;
    unordered_map<string, int> chatted_compression_table;

    read_for_table(humman_collection, human_compression_table, sequence_size, "human collection");
    read_for_table(chatted_collection, chatted_compression_table, sequence_size, "chatted collection");

    // print tables memory usage
    cout << "Human compression table memory usage: " << human_compression_table.size() * sizeof(pair<string, int>) / 1024 << " KB" << endl;
    cout << "Chatted compression table memory usage: " << chatted_compression_table.size() * sizeof(pair<string, int>) / 1024 << " KB" << endl;

    return 0;
}
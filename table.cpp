#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include <fstream>

#include "table.h"
#include "progress_bar.h"

#define BUFFER_SIZE 20 * 1024 * 1024

using namespace std;
using namespace std::chrono;

void Table::addSequence(string sequence)
{
    sequence = sequence.substr(0, sequence.size() - 1);
    if (table.find(sequence) == table.end())
    {
        table[sequence] = unordered_map<char, int>();
    }
    else
    {
        table[sequence][sequence[sequence.size() - 1]]++;
    }
};

int Table::memorySize()
{
    int size = 0;
    for (auto &entry : table)
    {
        size += entry.first.size() + sizeof(entry.second);
        for (auto &sub_entry : entry.second)
        {
            size += sizeof(sub_entry.first) + sizeof(sub_entry.second);
        }
    }
    return size;
};

void read_for_table(FILE *file, Table &table, size_t sequence_size, string label)
{
    auto start = high_resolution_clock::now();
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
    progress_bar(percent, 100);

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
            table.addSequence(sequence);
        }
        percent += 100 * buffer_size / file_size;
        progress_bar(percent, 100);
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
            table.addSequence(sequence);
        }
    }
    progress_bar(100, 100);
    cout << endl;

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << "Time taken: " << duration.count() / 1000 << " seconds" << endl;
}


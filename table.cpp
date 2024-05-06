#include <unordered_map>
#include <string>
#include <iostream>
#include <chrono>
#include <fstream>

#include "table.h"
#include "progress_bar.h"

#define BUFFER_SIZE 20 * 1024 * 1024

using namespace std;
using namespace std::chrono;

Table::Table(int newAlpha){
    alpha = newAlpha;
}

void Table::addSequence(string sequence, char nextChar)
{
    if (table.find(sequence) == table.end())
    {
        table[sequence] = unordered_map<char, int>();
    }
    if (table[sequence].find(nextChar) == table[sequence].end())
    {
        table[sequence][nextChar] = 1;
    }
    else
    {
        table[sequence][nextChar]++;
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

void Table::calcProbability(string sequence, char nextChar, int &charValue, double &prob)
{
    double sum = 0;
    charValue = 0;
    for(auto &entry : table[sequence]){
        sum += entry.second;
        if(entry.first == nextChar){
            charValue = entry.second;
        }
    }
    prob = (charValue + alpha) / (sum + alpha);
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
    table.total = file_size;

    if (file_size > BUFFER_SIZE)
    {
        buffer_size = BUFFER_SIZE;
    }
    else
    {
        buffer_size = file_size;
    }

    char *buffer = (char *)malloc(buffer_size * sizeof(char));
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
                table.total--;
                continue;
            }
            if (sequence.size() < sequence_size)
            {
                sequence += buffer[i];
                table.total--;
                continue;
            }
            table.addSequence(sequence, buffer[i]);
            for (size_t j = 0; j < sequence_size - 1; j++)
            {
                sequence[j] = sequence[j + 1];
            }
            sequence[sequence_size - 1] = buffer[i];
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
                table.total--;
                continue;
            }
            if (sequence.size() < sequence_size)
            {
                sequence += buffer[i];
                table.total--;
                continue;
            }
            table.addSequence(sequence, buffer[i]);
            for (size_t j = 0; j < sequence_size - 1; j++)
            {
                sequence[j] = sequence[j + 1];
            }
        }
    }
    progress_bar(100, 100);
    cout << endl;
    free(buffer);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << "Time taken: " << duration.count() / 1000 << " seconds" << endl;
}


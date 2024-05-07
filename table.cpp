#include <unordered_map>
#include <string>
#include <iostream>
#include <chrono>
#include <fstream>
#include <functional>
#include <cmath>

#include <sparsehash/sparse_hash_map>
#include "tsl/hopscotch_map.h"

#include "table.hpp"
#include "progress_bar.hpp"

#define BUFFER_SIZE 20 * 1024 * 1024
#define ALPHABET_SIZE 1 << sizeof(char) * 8

using namespace std;
using namespace std::chrono;

hash <string> hasher;

Table::Table(int newAlpha){
    alpha = newAlpha;
}

void Table::addSequence(string sequence, char nextChar)
{
    table[hasher(sequence)][nextChar]++;
};

int Table::memorySize()
{
    //return sizeof(unordered_map<size_t, unordered_map<char, uint>>) + table.size() * (sizeof(size_t) + sizeof(unordered_map<char, uint>) + sizeof(char) + sizeof(uint));
    //return sizeof(google::sparse_hash_map<size_t, google::sparse_hash_map<char, uint>>) + table.size() * (sizeof(size_t) + sizeof(google::sparse_hash_map<char, uint>) + sizeof(char) + sizeof(uint));
    return sizeof(ska::flat_hash_map<size_t, ska::flat_hash_map<char, uint>>) + table.size() * (sizeof(size_t) + sizeof(ska::flat_hash_map<char, uint>) + sizeof(char) + sizeof(uint));
    //return sizeof(tsl::hopscotch_map<size_t, tsl::hopscotch_map<char, uint>>) + table.size() * (sizeof(size_t) + sizeof(tsl::hopscotch_map<char, uint>) + sizeof(char) + sizeof(uint));
};

void Table::calcProbability(string sequence, char nextChar, double &prob)
{
    uint total = 0;
    for(auto &entry : table[hasher(sequence)]){
        total += entry.second;
    }
    prob = (double)(table[hasher(sequence)][nextChar] + alpha) / (total + (alpha * ALPHABET_SIZE));
};

void Table::clear()
{
    table.clear();
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
                continue;
            }
            if (sequence.size() < sequence_size)
            {
                sequence += buffer[i];
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
                continue;
            }
            if (sequence.size() < sequence_size)
            {
                sequence += buffer[i];
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
    auto duration = duration_cast<nanoseconds>(stop - start);
    cout << "Time taken: " << duration.count() / 1000 << " nanoseconds" << endl;
}


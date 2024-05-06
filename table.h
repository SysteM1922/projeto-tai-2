#ifndef TABLE_H
#define TABLE_H

#include <unordered_map>
#include <string>
#include <fstream>

#define BUFFER_SIZE 20 * 1024 * 1024

using namespace std;

struct Table
{
    Table(int newAlpha);
    int alpha;
    int total = 0;
    unordered_map<string, unordered_map<char, int>> table;

    void addSequence(string sequence, char nextChar);
    int memorySize();
    void calcProbability(string sequence, char nextChar, int &charValue, double &prob);
};
void progress_bar(int progress, int total);
void read_for_table(FILE *file, Table &table, size_t sequence_size, string label);

#endif
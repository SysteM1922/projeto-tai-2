#ifndef TABLE_H
#define TABLE_H

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <fstream>

#define BUFFER_SIZE 20 * 1024 * 1024

using namespace std;

struct Table
{
    Table(int newAlpha);
    int alpha;
    int total = 0;
    int alphabetSize = 0;
    unordered_map<size_t, unordered_map<char, uint>> table;
    unordered_set<char> alphabet;

    void addSequence(string sequence, char nextChar);
    int memorySize();
    void calcProbability(string sequence, char nextChar, int &sum, double &prob);
    void addToAlphabet(char c);
    void clear();
};
void progress_bar(int progress, int total);
void read_for_table(FILE *file, Table &table, size_t sequence_size, string label);

#endif
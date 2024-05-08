#ifndef TABLE_H
#define TABLE_H

#include <unordered_map>
#include <string>
#include <fstream>

#include "tsl/hopscotch_map.h"
//#include <sparsehash/sparse_hash_map>
#include "flat_hash_map.hpp"

#define BUFFER_SIZE 20 * 1024 * 1024

using namespace std;

struct Table
{
    Table(int newAlpha);
    int alpha;
    //unordered_map<size_t, unordered_map<char, uint>> table;
    //google::sparse_hash_map<size_t, google::sparse_hash_map<char, uint>> table;
    ska::flat_hash_map<size_t, ska::flat_hash_map<char, uint>> table;
    //tsl::hopscotch_map<size_t, tsl::hopscotch_map<char, uint>> table;

    void addSequence(string sequence, char nextChar);
    int memorySize();
    void calcProbability(string sequence, char nextChar, double &prob);
    void clear();
};
void progress_bar(int progress, int total);
void read_for_table(FILE *file, Table &table, size_t sequence_size, string label);

#endif
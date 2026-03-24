#include "storage.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <vector>
#include <string>

using namespace std;

DiskStorage::DiskStorage() {
    // Constructor - nothing to initialize
}

DiskStorage::~DiskStorage() {
    // Destructor
}

string DiskStorage::getBucketFileName(int bucketIndex) {
    return "bucket_" + to_string(bucketIndex) + ".dat";
}

int DiskStorage::hashKey(const string& key) {
    // Simple hash function
    unsigned long hash = 5381;
    for (char c : key) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash % BUCKET_COUNT;
}

void DiskStorage::insert(const string& key, int value) {
    int bucketIndex = hashKey(key);
    string filename = getBucketFileName(bucketIndex);

    // Read existing entries
    vector<Entry> entries;
    ifstream inFile(filename, ios::binary);
    if (inFile) {
        Entry entry;
        while (inFile.read(reinterpret_cast<char*>(&entry), ENTRY_SIZE)) {
            entries.push_back(entry);
        }
        inFile.close();
    }

    // Check if entry already exists
    for (auto& entry : entries) {
        if (entry.active && strcmp(entry.key, key.c_str()) == 0 && entry.value == value) {
            // Entry already exists
            return;
        }
    }

    // Add new entry
    entries.push_back(Entry(key, value));

    // Write back to file
    ofstream outFile(filename, ios::binary | ios::trunc);
    for (const auto& entry : entries) {
        outFile.write(reinterpret_cast<const char*>(&entry), ENTRY_SIZE);
    }
    outFile.close();
}

void DiskStorage::remove(const string& key, int value) {
    int bucketIndex = hashKey(key);
    string filename = getBucketFileName(bucketIndex);

    // Read existing entries
    vector<Entry> entries;
    ifstream inFile(filename, ios::binary);
    if (inFile) {
        Entry entry;
        while (inFile.read(reinterpret_cast<char*>(&entry), ENTRY_SIZE)) {
            entries.push_back(entry);
        }
        inFile.close();
    }

    // Mark matching entry as inactive
    bool modified = false;
    for (auto& entry : entries) {
        if (entry.active && strcmp(entry.key, key.c_str()) == 0 && entry.value == value) {
            entry.active = false;
            modified = true;
            break; // Only delete one matching entry
        }
    }

    // Write back if modified
    if (modified) {
        ofstream outFile(filename, ios::binary | ios::trunc);
        for (const auto& entry : entries) {
            outFile.write(reinterpret_cast<const char*>(&entry), ENTRY_SIZE);
        }
        outFile.close();
    }
}

vector<int> DiskStorage::find(const string& key) {
    int bucketIndex = hashKey(key);
    string filename = getBucketFileName(bucketIndex);

    vector<int> values;
    ifstream inFile(filename, ios::binary);
    if (!inFile) {
        return values; // Empty vector
    }

    Entry entry;
    while (inFile.read(reinterpret_cast<char*>(&entry), ENTRY_SIZE)) {
        if (entry.active && strcmp(entry.key, key.c_str()) == 0) {
            values.push_back(entry.value);
        }
    }
    inFile.close();

    // Sort values
    sort(values.begin(), values.end());
    return values;
}

void DiskStorage::cleanup() {
    // Delete all bucket files
    for (int i = 0; i < BUCKET_COUNT; i++) {
        string filename = getBucketFileName(i);
        ::remove(filename.c_str()); // Use global remove function
    }
}
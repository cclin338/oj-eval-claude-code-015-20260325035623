#include "storage.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <vector>
#include <string>
#include <unordered_map>

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

    // Check net count for this key-value pair
    int netCount = 0;
    ifstream inFile(filename, ios::binary);
    if (inFile) {
        Entry entry;
        while (inFile.read(reinterpret_cast<char*>(&entry), ENTRY_SIZE)) {
            if (strcmp(entry.key, key.c_str()) == 0 && entry.value == value) {
                if (entry.active) {
                    netCount++;
                } else {
                    netCount--;
                }
            }
        }
        inFile.close();
    }

    // If value already exists (netCount > 0), don't insert
    if (netCount > 0) {
        return;
    }

    // Append new entry
    ofstream outFile(filename, ios::binary | ios::app);
    Entry newEntry(key, value);
    outFile.write(reinterpret_cast<const char*>(&newEntry), ENTRY_SIZE);
    outFile.close();
}

void DiskStorage::remove(const string& key, int value) {
    int bucketIndex = hashKey(key);
    string filename = getBucketFileName(bucketIndex);

    // Append a delete marker (inactive entry)
    ofstream outFile(filename, ios::binary | ios::app);
    Entry deleteMarker(key, value);
    deleteMarker.active = false;
    outFile.write(reinterpret_cast<const char*>(&deleteMarker), ENTRY_SIZE);
    outFile.close();
}

vector<int> DiskStorage::find(const string& key) {
    int bucketIndex = hashKey(key);
    string filename = getBucketFileName(bucketIndex);

    // Use a map to count: +1 for insert, -1 for delete
    unordered_map<int, int> countMap;

    ifstream inFile(filename, ios::binary);
    if (!inFile) {
        // Return empty vector
        vector<int> result;
        return result;
    }

    Entry entry;
    while (inFile.read(reinterpret_cast<char*>(&entry), ENTRY_SIZE)) {
        if (strcmp(entry.key, key.c_str()) == 0) {
            if (entry.active) {
                countMap[entry.value]++;
            } else {
                countMap[entry.value]--;
            }
        }
    }
    inFile.close();

    // Collect values with count > 0
    vector<int> values;
    for (const auto& pair : countMap) {
        if (pair.second > 0) {
            values.push_back(pair.first);
        }
    }

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
#ifndef STORAGE_H
#define STORAGE_H

#include <string>
#include <vector>
#include <cstring>

class DiskStorage {
private:
    static const int BUCKET_COUNT = 1009; // Prime number
    static const int MAX_KEY_LEN = 64;

    struct Entry {
        char key[MAX_KEY_LEN + 1]; // +1 for null terminator
        int value;
        bool active; // true if entry is valid

        Entry() : value(0), active(false) {
            key[0] = '\0';
        }

        Entry(const std::string& k, int v) : value(v), active(true) {
            strncpy(key, k.c_str(), MAX_KEY_LEN);
            key[MAX_KEY_LEN] = '\0';
        }
    };

    static const int ENTRY_SIZE = sizeof(Entry);

    std::string getBucketFileName(int bucketIndex);
    int hashKey(const std::string& key);

public:
    DiskStorage();
    ~DiskStorage();

    // Operations
    void insert(const std::string& key, int value);
    void remove(const std::string& key, int value);
    std::vector<int> find(const std::string& key);

    // Cleanup (called at end)
    void cleanup();
};

#endif // STORAGE_H
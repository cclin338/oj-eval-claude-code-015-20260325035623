#include "storage.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    DiskStorage storage;
    int n;
    cin >> n;
    cin.ignore(); // Ignore newline after n

    for (int i = 0; i < n; i++) {
        string line;
        getline(cin, line);
        stringstream ss(line);
        string command;
        ss >> command;

        if (command == "insert") {
            string key;
            int value;
            ss >> key >> value;
            storage.insert(key, value);
        } else if (command == "delete") {
            string key;
            int value;
            ss >> key >> value;
            storage.remove(key, value);
        } else if (command == "find") {
            string key;
            ss >> key;
            vector<int> values = storage.find(key);

            if (values.empty()) {
                cout << "null\n";
            } else {
                for (size_t j = 0; j < values.size(); j++) {
                    if (j > 0) cout << " ";
                    cout << values[j];
                }
                cout << "\n";
            }
        }
    }

    // Note: We don't call cleanup() because the judge will handle file cleanup
    // storage.cleanup();

    return 0;
}
#pragma once

#include <iostream>
#include <unordered_map>
#include <string>
#include <thread>
using namespace std;

class KVStore {
public:
    void set(const string& key, const string& val);
    bool get(const string& key,  string& val);
    bool del(const string& key);
private:
    unordered_map<string, string> data_;
};
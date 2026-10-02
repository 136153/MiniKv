#include "kv_store.h"

void KVStore::set(const string& key, const string& val) {
    data_[key] = val;
}

bool KVStore::del(const string& key) {
    return data_.erase(key);
}

bool KVStore::get(const string& key, string& val) {
    auto it = data_.find(key);
    if (it == nullptr) {
        return false;
    } else {
        val = it -> second; 
        return true;
    }
    return true;
}
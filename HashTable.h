#pragma once
#include <string>
#include <stdexcept>

// ============================================================
// Custom Hash Table — name → nodeId lookup
// Chaining collision resolution, auto-rehash at load > 0.7
// ============================================================

struct HTEntry {
    std::string key;
    int         value;
    HTEntry*    next;
    HTEntry(const std::string& k, int v) : key(k), value(v), next(nullptr) {}
};

class HashTable {
private:
    HTEntry** buckets;
    int       cap;
    int       count;

    static const int INIT_CAP = 64;

    int hash(const std::string& key) const {
        unsigned long h = 5381;
        for (char c : key) h = ((h << 5) + h) ^ (unsigned char)c;
        return (int)(h % (unsigned long)cap);
    }

    void rehash() {
        int       oldCap     = cap;
        HTEntry** oldBuckets = buckets;
        cap     = cap * 2 + 1;
        buckets = new HTEntry*[cap]();
        count   = 0;
        for (int i = 0; i < oldCap; i++) {
            HTEntry* e = oldBuckets[i];
            while (e) {
                insert(e->key, e->value);
                HTEntry* tmp = e->next;
                delete e;
                e = tmp;
            }
        }
        delete[] oldBuckets;
    }

    void freeAll() {
        for (int i = 0; i < cap; i++) {
            HTEntry* e = buckets[i];
            while (e) { HTEntry* tmp = e->next; delete e; e = tmp; }
            buckets[i] = nullptr;
        }
        count = 0;
    }

public:
    HashTable() : cap(INIT_CAP), count(0) {
        buckets = new HTEntry*[cap]();
    }

    // Deep copy constructor
    HashTable(const HashTable& o) : cap(o.cap), count(0) {
        buckets = new HTEntry*[cap]();
        for (int i = 0; i < o.cap; i++)
            for (HTEntry* e = o.buckets[i]; e; e = e->next)
                insert(e->key, e->value);
    }

    // Copy assignment
    HashTable& operator=(const HashTable& o) {
        if (this == &o) return *this;
        freeAll();
        delete[] buckets;
        cap     = o.cap;
        count   = 0;
        buckets = new HTEntry*[cap]();
        for (int i = 0; i < o.cap; i++)
            for (HTEntry* e = o.buckets[i]; e; e = e->next)
                insert(e->key, e->value);
        return *this;
    }

    ~HashTable() {
        freeAll();
        delete[] buckets;
    }

    void insert(const std::string& key, int value) {
        if ((double)count / cap > 0.7) rehash();
        int idx = hash(key);
        for (HTEntry* e = buckets[idx]; e; e = e->next)
            if (e->key == key) { e->value = value; return; }  // update
        HTEntry* entry = new HTEntry(key, value);
        entry->next  = buckets[idx];
        buckets[idx] = entry;
        count++;
    }

    int search(const std::string& key) const {
        int idx = hash(key);
        for (HTEntry* e = buckets[idx]; e; e = e->next)
            if (e->key == key) return e->value;
        return -1;  // not found
    }

    bool remove(const std::string& key) {
        int idx = hash(key);
        HTEntry* prev = nullptr;
        HTEntry* e    = buckets[idx];
        while (e) {
            if (e->key == key) {
                if (prev) prev->next = e->next;
                else       buckets[idx] = e->next;
                delete e;
                count--;
                return true;
            }
            prev = e; e = e->next;
        }
        return false;
    }

    int getCount() const { return count; }
};

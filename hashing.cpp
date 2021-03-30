//
// Created by abirm on 1/10/2019.
//
#include <bits/stdc++.h>

using namespace std;
using namespace std::chrono;


std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(1, INT_MAX);

#define alpha 3
#define getTime high_resolution_clock::now();
const unsigned long n = 10000;
const unsigned long m = n / alpha;

uint32_t hash1(const char *str) {
    uint32_t hash = 5381;
    int len = 7;
    int c;
    for (int i = 0; i < len; ++i) {
        c = *str++;
        hash = (hash << 5) + hash + c; /* hash * 33 + c */

    }

    return hash;
}

uint32_t hash2(const char *key) {
    const uint8_t *data = (const uint8_t *) key;
    int len = 7;
    uint32_t h = 97777;
    for (int i = 0; i < len; i++) {
        h ^= data[i];
        h *= 0x5bd1e995;
        h ^= h >> 15;
    }
    return h;
}

uint32_t auxHash(const char *str) {
    uint32_t hash = 101;
    int len = 7;
    int c;

    for (int i = 0; i < len; ++i) {
        c = *str++;
        hash = hash + c % hash;

    }

    return hash;
}

int get_rand() {
    return dis(gen);
}

void gen_random(char *s) {
    static const int len = 7;
    static const char alphanum[] = "abcdefghijklmnopqrstuvwxyz";
    //"ABCDEFGHIJKLMNOPQRSTUVWXYZ"

    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[get_rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
}

class Node {

public:
    Node(char *data) : data(data) {
        next = nullptr;
        previous = nullptr;
    }

    Node *next;
    Node *previous;

    char *data;
};

class HashTable_Chaining {
    Node **table;
    int size;
    int current_size;
    int collision;
    int hits;

    uint32_t (*hash_function)(const char *);

public:
    HashTable_Chaining(int size, uint32_t (*hash_function)(const char *)) :
            size(size), hash_function(hash_function), current_size(0),
            collision(0), hits(0) {
        table = new Node *[size];
    }

    int getCollision() const {
        return collision;
    }

    int getHits() const {
        return hits;
    }

    bool insert(char *data) {
        Node *node = new Node(data);
        uint32_t index = hash_function(data) % size;
        if (table[index] == nullptr) {
            table[index] = node;
            return true;
        } else {
            collision++;
            Node *node1 = table[index];
            if (node1 != nullptr) {
                while (node1->next != nullptr) {
                    if (strcmp(node1->data, data) == 0)
                        return false;
                    node1 = node1->next;
                }
                if (strcmp(node1->data, data) == 0)
                    return false;
                node1->next = node;
                node->previous = node1;
                return true;
            }
        }
    }

    bool search(char *data) {
        uint32_t index = hash_function(data) % size;
        if (table[index] == nullptr)
            return false;
        else {
            Node *node1 = table[index];
            while (node1 != nullptr) {
                hits++;
                if (strcmp(data, node1->data) == 0)
                    return true;
                node1 = node1->next;
            }
        }
    }

    bool deelet(const char *data) {
        uint32_t index = hash_function(data) % size;
        if (table[index] == nullptr)
            return false;
        else {
            Node *node1 = table[index];
            while (node1 != nullptr) {
                if (strcmp(data, node1->data) == 0) {
                    if (node1->previous != nullptr)
                        node1->previous->next = node1->next;
                    if (node1->next != nullptr) {
                        node1->next->previous = node1->previous;
                    }
                    node1 = nullptr;
                    return true;
                }
                node1 = node1->next;
            }
        }
    }
};

class HashTable_Probing {
    char **table;
    bool *is_deleted;
    int collitions;
    int hits;

    uint32_t (*hash_function)(const char *);

    uint32_t size;
    uint32_t current_size;

public:

    HashTable_Probing(uint32_t size, uint32_t (*hash_function)(const char *)) : size(size), current_size(0),
                                                                                collitions(0), hits(0),
                                                                                hash_function(hash_function) {
        table = new char *[size];
        is_deleted = new bool[size];
    }

    int getCollision() const {
        return collitions;
    }

    int getHits() const {
        return hits;
    }

    bool insert(char *data) {
        if (current_size == size)
            return false;
        uint32_t i = 0, index1 = hash_function(data), index2 = auxHash(data);
        uint32_t index = (index1 + i * index2) % size;
        uint32_t ind = index;
        while (table[index] != nullptr) {
            if (strcmp(table[index], data) == 0)
                return false;
            i++;
            collitions++;
            index = (index1 + i * index2) % size;
        }
        table[index] = data;
        is_deleted[index] = false;
        current_size++;
        return true;
    }

    bool search(const char *data) {
        uint32_t i = 0, index1 = hash_function(data), index2 = auxHash(data);
        uint32_t index = (index1 + i * index2) % size;
        hits++;
        uint32_t ind = index;
        if (table[index] == nullptr && !is_deleted[index])
            return false;
        if (strcmp(table[index], data) == 0)
            return true;
        while (table[index] != nullptr || is_deleted[index]) {
            i++;
            if (is_deleted[index]) {
                index = (index1 + i * index2) % size;
                continue;
            }
            if (strcmp(table[index], data) == 0)
                return true;
            index = (index1 + i * index2) % size;
            hits++;
            if (index == ind)
                return false;
        }
    }

    bool dilet(const char *data) {
        uint32_t i = 0, index1 = hash_function(data), index2 = auxHash(data);
        uint32_t index = (index1 + i * index2) % size;
        uint32_t ind = index;
        if (table[index] == nullptr && !is_deleted[index])
            return false;
        if (strcmp(table[index], data) == 0) {
            table[index] = nullptr;
            is_deleted[index] = true;
            return true;
        }
        while (table[index] != nullptr || is_deleted[index]) {
            if (is_deleted[index]) {
                index = (index1 + i * index2) % size;
                continue;
            }
            if (strcmp(table[index], data) == 0) {
                table[index] = nullptr;
                is_deleted[index] = true;
                return true;
            }
            i++;
            index = (index1 + i * index2) % size;
            if (index == ind)
                return false;
        }
    }

};

class HashTable_Custom_Probing {
    char **table;
    bool *is_deleted;
    uint32_t size;
    uint32_t current_size;
    int collosions;
    int hits;

    uint32_t (*hash_function)(const char *);

    const uint32_t C1 = 8389, C2 = 7177;

public:
    HashTable_Custom_Probing(uint32_t size, uint32_t (*hash_function)(const char *)) : size(size), current_size(0),
                                                                                       hits(0), collosions(0),
                                                                                       hash_function(hash_function) {
        table = new char *[size];
        is_deleted = new bool[size];
    }

    int getCollosion() const {
        return collosions;
    }

    int getHits() const {
        return hits;
    }

    bool insert(char *data) {
        if (current_size == size)
            return false;
        uint32_t i = 0, index1 = hash_function(data), index2 = auxHash(data);
        uint32_t index = (index1 + i * index2 * C1 + C2 * i * i) % size;
        uint32_t ind = index;
        while (table[index] != nullptr) {
            if (strcmp(table[index], data) == 0)
                return false;
            i++;
            collosions++;
            (index1 + i * index2 * C1 + C2 * i * i) % size;
        }
        table[index] = data;
        is_deleted[index] = false;
        current_size++;
        return true;
    }

    bool search(const char *data) {
        uint32_t i = 0, index1 = hash_function(data), index2 = auxHash(data);
        uint32_t index = (index1 + i * index2 * C1 + C2 * i * i) % size;
        hits++;
        uint32_t ind = index;
        if (table[index] == nullptr && !is_deleted[index])
            return false;
        if (strcmp(table[index], data) == 0)
            return true;
        while (table[index] != nullptr || is_deleted[index]) {
            if (is_deleted[index]) {
                (index1 + i * index2 * C1 + C2 * i * i) % size;
                continue;
            }
            if (strcmp(table[index], data) == 0)
                return true;
            i++;
            hits++;
            (index1 + i * index2 * C1 + C2 * i * i) % size;
            if (index == ind)
                return false;
        }
    }

    bool dilet(const char *data) {
        uint32_t i = 0, index1 = hash_function(data), index2 = auxHash(data);
        uint32_t index = (index1 + i * index2 * C1 + C2 * i * i) % size;
        uint32_t ind = index;
        if (table[index] == nullptr && !is_deleted[index])
            return false;
        if (strcmp(table[index], data) == 0) {
            table[index] = nullptr;
            is_deleted[index] = true;
            return true;
        }
        while (table[index] != nullptr || is_deleted[index]) {
            if (is_deleted[index]) {
                index = (index1 + i * index2 * C1 + C2 * i * i) % size;
                continue;
            }
            if (strcmp(table[index], data) == 0) {
                table[index] = nullptr;
                is_deleted[index] = true;
                return true;
            }
            i++;
            index = (index1 + i * index2 * C1 + C2 * i * i) % size;
            if (index == ind)
                return false;
        }
    }

};

int main() {
    HashTable_Chaining hashTable_chaining1(n, hash1);
    HashTable_Chaining hashTable_chaining2(n, hash2);
    HashTable_Probing hashTable_probing1(n * alpha, hash1);
    HashTable_Probing hashTable_probing2(n * alpha, hash2);
    HashTable_Custom_Probing hashTable_custom_probing1(n * alpha, hash1);
    HashTable_Custom_Probing hashTable_custom_probing2(n * alpha, hash2);

    char words[n][7];
    //words = new char *[n];
    for (int i = 0; i < n; ++i) {
        gen_random(words[i]);
        //cout<<words[i]<<endl;
        hashTable_chaining1.insert(words[i]);
        hashTable_chaining2.insert(words[i]);
        hashTable_probing1.insert(words[i]);
        hashTable_probing2.insert(words[i]);
        hashTable_custom_probing1.insert(words[i]);
        hashTable_custom_probing2.insert(words[i]);

    }
    for (int j = 0; j < n / 10; ++j) {
        uint32_t i=get_rand() % n;
        hashTable_chaining1.search(words[i]);
        hashTable_chaining1.search(words[i]);
        hashTable_chaining2.search(words[i]);
        hashTable_probing1.search(words[i]);
        hashTable_probing2.search(words[i]);
        hashTable_custom_probing1.search(words[i]);
        hashTable_custom_probing2.search(words[i]);
    }
    cout << "Chaining Hash1:  Collisions: " << hashTable_chaining1.getCollision() << "Average Hits " << hashTable_chaining1.getHits() / (n / 10.0) << endl;
    cout << "Chaining Hash2:  Collisions: " << hashTable_chaining2.getCollision() << "Average Hits " << hashTable_chaining2.getHits() / (n / 10.0) << endl<<endl;
    cout << "Double   Hash1:  Collisions: " << hashTable_probing1.getCollision() << "Average Hits " << hashTable_probing1.getHits() / (n / 10.0) << endl;
    cout << "Double   Hash2:  Collisions: " << hashTable_probing2.getCollision() << "Average Hits " << hashTable_probing2.getHits() / (n / 10.0) << endl<<endl;
    cout << "Custom   Hash1:  Collisions: " << hashTable_custom_probing1.getCollosion() << "Average Hits " << hashTable_custom_probing1.getHits() / (n / 10.0)
         << endl;
    cout << "Custom   Hash2:  Collisions: " << hashTable_custom_probing2.getCollosion() << "Average Hits " << hashTable_custom_probing2.getHits() / (n / 10.0)
         << endl;

    return 0;
}

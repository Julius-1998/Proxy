#ifndef __RAWCACHE_HPP__
#define __RAWCACHE_HPP__

#include <vector>
#include <unordered_map>
#include <mutex>
#include <optional>

template<typename K, typename V>
class RandomEvictCache {
private:
    std::unordered_map<K,V> mp;
    size_t max_capacity;
public:
    RandomEvictCache(int max_capacity) : mp(), max_capacity(max_capacity) { }

    bool containsKey(const K& key) { return mp.count(key); }

    void put(const K& key, const V& value) {
        if (mp.count(key) || mp.size() < max_capacity) {
            mp[key] = value;
        } else {
            mp.erase(mp.begin());
            mp[key] = value;
        }
    }

    V& get(const K& key) { return mp[key]; }

    void erase(const K& key) { mp.erase(key); }
};


template<typename K, typename V>
class ConcurrentRandomEvictCache {
private:
    std::vector<RandomEvictCache<K,V>> mps;
    std::vector<std::mutex> locks;
    std::hash<K> hash;
    int size;
public:
    ConcurrentRandomEvictCache(int max_capacity, int size) : mps(size, RandomEvictCache<K,V>(max_capacity)), locks(size), hash(), size(size) {}
    /*
    bool containsKey(const K& key) {
        int idx = hash(key) % size;
        std::scoped_lock<std::mutex> lk(locks[idx]);
        return mps[idx].containsKey(key);
    }
    */

    void put(const K& key, const V& value) {
        int idx = hash(key) % size;
        std::scoped_lock<std::mutex> lk(locks[idx]);
        mps[idx].put(key, value);
    }

    std::optional<V> get(const K& key) {
        int idx = hash(key) % size;
        std::scoped_lock<std::mutex> lk(locks[idx]);
        if (!mps[idx].containsKey(key))
            return std::optional<V>();
        return std::optional<V>(mps[idx].get(key));
    }

    void erase(const K& key) {
        int idx = hash(key) % size;
        std::scoped_lock<std::mutex> lk(locks[idx]);
        mps[idx].erase(key);
    }
};

#endif

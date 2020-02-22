#include <list>
#include <stdexcept>
#include <vector>

template<typename KeyType, typename ValueType, typename Hash = std::hash<KeyType> >
class HashMap {
public:
    using iterator = typename std::list<std::pair<const KeyType, ValueType>>::iterator;
    using const_iterator = typename std::list<std::pair<const KeyType, ValueType>>::const_iterator;
private:
    std::list<std::pair<const KeyType, ValueType>> list;
    std::vector<std::list<std::pair<KeyType, iterator>>> backets;
    Hash hasher;
    size_t list_size;
    
    size_t _index_by_key(const KeyType &key) const {
        return hasher(key) % backets.size();
    }

    void _reallocate(size_t upd_size) {
        backets.clear();
        backets.resize(upd_size);
        for (auto it = list.begin(); it != list.end(); ++it) {
            backets[_index_by_key(it->first)].emplace_back(it->first, it);
        }
    }

public:
    HashMap(const Hash &hasher = Hash()): backets(1), hasher(hasher), list_size(0) {}

    template<typename It>
    HashMap(It first, It second, const Hash &hasher = Hash()): HashMap(hasher) {
        for (auto it = first; it != second; ++it) {
            insert(*it);
        }
    }

    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> init_list): HashMap(init_list.begin(), init_list.end()) {} 

    Hash hash_function() const {
        return hasher;
    }

    size_t size() const {
        return list_size;
    }

    bool empty() const {
        return list_size == 0;
    }

    iterator begin() {
        return list.begin();
    }

    iterator end() {
        return list.end();
    }

    const_iterator begin() const {
        return list.begin();
    }

    const_iterator end() const {
        return list.end();
    }

    void insert(const std::pair<const KeyType, ValueType> &item) {
        auto ind = _index_by_key(item.first);
        for (auto pr : backets[ind]) {
            if (item.first == pr.first) {
                return;
            }
        }
        list.push_front(item);
        backets[ind].emplace_back(item.first, list.begin());
        ++list_size;
        if (list_size >= backets.size()) {
            _reallocate(2 * backets.size());
        }
    }

    void erase(const KeyType &key) {
        auto ind = _index_by_key(key);
        for (auto it = backets[ind].begin(); it != backets[ind].end(); ++it) {
            if (it->first == key) {
                list.erase(it->second);
                backets[ind].erase(it);
                --list_size;
                return;
            }
        }
    }

    const_iterator find(const KeyType &key) const {
        auto ind = _index_by_key(key);
        for (auto it = backets[ind].begin(); it != backets[ind].end(); ++it) {
            if (it->first == key) {
                return it->second;
            }
        }
        return end();
    }

    iterator find(const KeyType &key) {
        auto ind = _index_by_key(key);
        for (auto it = backets[ind].begin(); it != backets[ind].end(); ++it) {
            if (it->first == key) {
                return it->second;
            }
        }
        return end();
    }

    ValueType& operator[](const KeyType &key) {
        auto ind = _index_by_key(key);
        for (auto it = backets[ind].begin(); it != backets[ind].end(); ++it) {
            if (it->first == key) {
                return it->second->second;
            }
        }
        list.emplace_front(key, ValueType());
        backets[ind].emplace_back(key, list.begin());
        ++list_size;
        if (list_size >= backets.size()) {
            _reallocate(2 * backets.size());
        }
        return list.begin()->second;
    }
    
    const ValueType& at(const KeyType &key) const {
        auto ind = _index_by_key(key);
        for (auto it = backets[ind].begin(); it != backets[ind].end(); ++it) {
            if (it->first == key) {
                return it->second->second;
            }
        }
        throw std::out_of_range("There is no element with this key in hash map");
    }

    void clear() {
        list.clear();
        backets.clear();
        list_size = 0;
        backets.emplace_back();
    }

    HashMap& operator=(const HashMap &oth) {
        if (this == &oth) {
            return *this; 
        }
        clear();
        for (auto it = oth.begin(); it != oth.end(); ++it) {
            insert(*it);
        }
        return *this;
    }

    ~HashMap() {
        clear();
    }

};
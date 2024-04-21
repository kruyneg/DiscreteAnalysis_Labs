#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <cstring>
#include <sstream>
#define fast_io std::ios::sync_with_stdio(false); std::cin.tie(0); std::cout.tie(0)

enum colors {
    red = false,
    black = true
};

template <typename Key, typename T> class map;

template <typename T>
class rb_tree {
    struct __node {
        T value;
        __node* left;
        __node* right;
        __node* parent;
        colors color;

        __node(const T& val):
            value(val),
            left(nullptr),
            right(nullptr),
            parent(nullptr),
            color(red)
        {}
        ~__node() {
            if (left) {
                delete left;
            }
            if (right) {
                delete right;
            }
        }
    };
    
    void print(std::ostream& out, __node* cur, int space) {
        if (!cur) {
            return;
        }
        if (cur->right) {
            print(out, cur->right, space + 1);
        }
        int tmp = space;
        while (tmp--) {
            out << '\t';
        }
        // out << "\033[107m";
        if (cur->parent && cur->value < cur->parent->value) {
            out << '\\';
        } else {
            out << '/';
        }
        if (cur->color == red) {
            out << "\033[91m" << cur->value << "\033[39m" << std::endl;
        } else {
            out << "\033[30m" << cur->value << "\033[39m" << std::endl;
        }
        // out << "\033[49m";
        if (cur->left) {
            print(out, cur->left, space + 1);
        }
    }

public:
    // TODO: iterator
    rb_tree(): _m_head(nullptr), _m_size(0) {}
    rb_tree(const std::initializer_list<T>& l): _m_head(nullptr), _m_size(0) {
        for (const T& elem : l) {
            insert(elem);
        }
    }
    ~rb_tree() {
        clear();
    }

    __node* insert(const T& val) noexcept {
        // std::cout << "=================================================================" << std::endl;
        // print(std::cout, _m_head, 0);
        // std::cout << "=================================================================" << std::endl;
        // std::cout << "Insert " << val << std::endl;

        __node* parent = __find_parent(_m_head, val);
        if (_m_head && !(_m_head->value != val)) {
            parent = nullptr;
        }
        return __insert_node(parent, val);
    }

    void erase(const T& val) noexcept {
        // std::cout << "=================================================================" << std::endl;
        // print(std::cout, _m_head, 0);
        // std::cout << "=================================================================" << std::endl;
        // std::cout << "Erase " << val << std::endl;
        __node* target = __find(_m_head, val);
        if (!target) {
            std::cout << "NoSuchWord\n";
            return;
        }
        --_m_size;
        std::cout << "OK\n";
        return __delete_node(target);
    }

    bool contains(const T& val) const noexcept {
        return __find(_m_head, val);
    }

    // TODO: iterator find
    __node* find(const T& val) const noexcept {
        return __find(_m_head, val);
    }

    size_t size() const noexcept {
        return _m_size;
    }

    void clear() {
        delete _m_head;
        _m_head = nullptr;
        _m_size = 0;
    }
private:
    __node* __find(__node* current, const T& val) const noexcept {
        while (current && current->value != val) {
            if (val < current->value) {
                current = current->left;
            } else {
                current = current->right;
            }
        }
        return current;
    }
    __node* __find_parent(__node* current, const T& val) const noexcept {
        if (!current) {
            return current;
        }
        __node* next = 
            (val < current->value) ?
            current->left:
            current->right;
        while (next && next->value != val) {
            current = next;
            if (val < next->value) {
                next = next->left;
            } else {
                next = next->right;
            }
        }
        return current;
    }

    void __left_rotation(__node* x) noexcept {
        __node* y = x->right;
        y->parent = x->parent;
        x->parent = y;
        x->right = y->left;
        if (x->right)
            x->right->parent = x;
        y->left = x;
        if (_m_head == x) {
            _m_head = y;
        }
        if (y->parent) {
            if (y->value < y->parent->value) {
                y->parent->left = y;
            } else {
                y->parent->right = y;
            }
        }
    }
    void __right_rotation(__node* y) noexcept {
        __node* x = y->left;
        x->parent = y->parent;
        y->parent = x;
        y->left = x->right;
        if (y->left)
            y->left->parent = y;
        x->right = y;
        if (_m_head == y) {
            _m_head = x;
        }
        if (x->parent) {
            if (x->value < x->parent->value) {
                x->parent->left = x;
            } else {
                x->parent->right = x;
            }
        }
    }

    void __insert_balancing(__node* target) noexcept {
        while (target->parent && target->parent->color == red) {
            __node* parent = target->parent;
            __node* grandpa = parent->parent;
            if (parent == grandpa->left) {
                __node* uncle = grandpa->right;
                if (uncle && uncle->color == red) {
                    parent->color = black;
                    uncle->color = black;
                    grandpa->color = red;
                    target = grandpa;
                } else {
                    if (target == parent->right) {
                        __left_rotation(parent);
                        std::swap(target, parent);
                    }
                    parent->color = black;
                    grandpa->color = red;
                    __right_rotation(grandpa);
                }
            } else {
                __node* uncle = grandpa->left;
                if (uncle && uncle->color == red) {
                    parent->color = black;
                    uncle->color = black;
                    grandpa->color = red;
                    target = grandpa;
                } else {
                    if (target == parent->left) {
                        __right_rotation(parent);
                        std::swap(target, parent);
                    }
                    parent->color = black;
                    grandpa->color = red;
                    __left_rotation(grandpa);
                }
            }
        }
        _m_head->color = black;
    }

    __node* __insert_node(__node* parent, const T& val) noexcept {
        if (!parent) {
            if (!_m_head)
                _m_head = new __node(val);
            else 
                return _m_head;
            _m_head->color = black;
            _m_size = 1;
            return _m_head;
        } else {
            __node* &target = 
                (val < parent->value) ?
                parent->left :
                parent->right;
            if (target) {
                return target;
            }
            target = new __node(val);
            target->parent = parent;
            __insert_balancing(target);
            ++_m_size;
            return target;
        }
    }

    void __erase_balancing(__node* target) {
        while (target != _m_head && target->color == black) {
            __node* parent = target->parent;
            if (parent->left == target) {
                __node* brother = parent->right;
                if (brother && brother->color == red) {
                    brother->color = black;
                    parent->color = red;
                    __left_rotation(parent);
                    brother = parent->right;
                }
                if ((!brother->left || brother->left->color == black) && (!brother->right || brother->right->color == black)) {
                    brother->color = red;
                    target = parent;
                } else {
                    if (!brother->right || brother->right->color == black) {
                        brother->left->color = black;
                        brother->color = red;
                        __right_rotation(brother);
                        brother = parent->right;
                    }
                    brother->color = parent->color;
                    parent->color = black;
                    brother->right->color = black;
                    __left_rotation(parent);
                    target = _m_head;
                }
            } else {
                __node* brother = parent->left;
                if (brother && brother->color == red) {
                    brother->color = black;
                    parent->color = red;
                    __right_rotation(parent);
                    brother = parent->left;
                }
                if ((!brother->right || brother->right->color == black) && (!brother->left || brother->left->color == black)) {
                    brother->color = red;
                    target = parent;
                } else {
                    if (!brother->left || brother->left->color == black) {
                        brother->right->color = black;
                        brother->color = red;
                        __left_rotation(brother);
                        brother = parent->left;
                    }
                    brother->color = parent->color;
                    parent->color = black;
                    brother->left->color = black;
                    __right_rotation(parent);
                    target = _m_head;
                }
            }
        }
        target->color = black;
    }

    void __delete_node(__node* target) {
        if (target->left && target->right) {
            __node* victim = target->right;
            while (victim->left) {
                victim = victim->left;
            }
            target->value = victim->value;
            __delete_node(victim);
            return;
        }
        __node* parent = target->parent;
        __node* brother;
        __node*& target_edge =
            (!parent) ?
            _m_head :
            (parent->left == target) ?
            parent->left :
            parent->right;
        __erase_balancing(target);
        if (target->right) {
            target_edge = target->right;
            target->right->parent = parent;
        } else if (target->left) {
            target_edge = target->left;
            target->left->parent = parent;
        } else {
            target_edge = nullptr;
        }
        target->left = target->right = nullptr;
        delete target;
        if (_m_head)
            _m_head->color = black;
    }


    __node* _m_head;
    size_t _m_size;
    
    template<class K, class V> friend class map;
};

template <typename Key, typename T>
class map {
    struct __data {
        Key key;
        T value;
        bool operator<(const __data& other) const noexcept {
            return key < other.key;
        }
        bool operator!=(const __data& other) const noexcept {
            return key != other.key;
        }
        friend inline std::ostream& operator<<(std::ostream& out, const __data& other) {
            return out << other.key << '|' << other.value;
        }
    };
public:
    map(): __tree() {}
    map(const std::initializer_list<__data>& l): __tree(l) {}
    
    void insert(const Key& k, const T& val) noexcept {
        __data item = {k, val};
        __tree.insert(item)->value = item;
    }

    void erase(const Key& k) noexcept {
        __tree.erase({k, T()});
    }

    void clear() noexcept {
        __tree.clear();
    }

    bool contains(const Key& k) const noexcept {
        return __tree.contains({k, T()});
    }

    size_t size() const noexcept {
        return __tree.size();
    }

    bool empty() const noexcept {
        return !__tree.size();
    }

    T& operator[](const Key& key) noexcept {
        __tree.insert({key, T()});
        return __tree.find({key, T()})->value.value;
    }

    void save(const std::string&) = delete;
    void load(const std::string&) = delete;
private:
    struct save_item {
        Key key;
        T val;
        colors color;
        bool has_left;
        bool has_right;
    };

    void serialize(std::ofstream& out, typename rb_tree<map<Key, T>::__data>::__node* cur) {
        save_item record = {cur->value.key, cur->value.value, cur->color, static_cast<bool>(cur->left), static_cast<bool>(cur->right)};
        out.write(record.key.c_str(), 256);
        out.write(reinterpret_cast<char*>(&record.val), sizeof(unsigned long long));
        out.write(reinterpret_cast<char*>(&record.color), sizeof(colors));
        out.write(reinterpret_cast<char*>(&record.has_left), sizeof(bool));
        out.write(reinterpret_cast<char*>(&record.has_right), sizeof(bool));
        out.flush();
        if (cur->left)
            serialize(out, cur->left);
        if (cur->right)
            serialize(out, cur->right);
    }

    rb_tree<__data> __tree;
};


template<>
void map<std::string, unsigned long long>::save(const std::string& filename) {
    std::ofstream out(filename, std::ios::binary);
    out.write(reinterpret_cast<char*>(&__tree._m_size), sizeof(size_t));
    if (!empty()) {
        serialize(out, __tree._m_head);
    }
}

template<>
void map<std::string, unsigned long long>::load(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);

    __tree.clear();
    std::stack<rb_tree<__data>::__node*> q;
    size_t len;
    in.read(reinterpret_cast<char*>(&len), sizeof(size_t));
    if (in.eof() || !len) {
        return;
    }
    __tree._m_head = new rb_tree<__data>::__node({std::string(), 0});
    q.push(__tree._m_head);
    while (!q.empty() && !in.eof()) {
        save_item record;
        std::string key;
        key.resize(256);
        unsigned long long val;
        colors col;
        bool r, l;
        in.read((char*)key.c_str(), 256);
        in.read(reinterpret_cast<char*>(&val), sizeof(unsigned long long));
        in.read(reinterpret_cast<char*>(&col), sizeof(col));
        in.read(reinterpret_cast<char*>(&l), sizeof(l));
        in.read(reinterpret_cast<char*>(&r), sizeof(r));
        key.resize(std::strlen(key.c_str()));
        // std::cout << key << ' '
        //           << val << ' '
        //           << col << ' '
        //           << l << ' '
        //           << r << std::endl;

        rb_tree<__data>::__node* current = q.top();
        q.pop();
        current->value = {move(key), val};
        ++__tree._m_size;
        current->color = col;

        if (r) {
            current->right = new rb_tree<__data>::__node({std::string(), 0});
            current->right->parent = current;
            q.push(current->right);
        } else {
            current->right = nullptr;
        }
        if (l) {
            current->left = new rb_tree<__data>::__node({std::string(), 0});
            current->left->parent = current;
            q.push(current->left);
        } else {
            current->left = nullptr;
        }
    }
}

void tolow(std::string& str) {
    for (char& c : str) {
        c = tolower(c);
    }
}

int main() {
    fast_io;

    map<std::string, unsigned long long> m;
    std::string line;
    while(!std::cin.eof()) {
        std::string first;
        std::cin >> first;
        if (first == "+") {
            std::string word;
            unsigned long long n;
            std::cin >> word >> n;
            tolow(word);
            if (m.contains(word)) {
                std::cout << "Exist\n";
            } else {
                m[word] = n;
                std::cout << "OK\n";
            }
        } else if (first == "-") {
            std::string word;
            std::cin >> word;
            tolow(word);
            m.erase(word);
        } else if (first == "!") {
            std::string command, file;
            std::cin >> command >> file;
            if (command == "Save") {
                m.save(file);
                std::cout << "OK\n";
            } else {
                m.load(file);
                std::cout << "OK\n";
            }
        } else if (!first.empty()) {
            tolow(first);
            if (m.contains(first)) {
                std::cout << "OK: " << m[first] << '\n';
            } else {
                std::cout << "NoSuchWord\n";
            }
        }
    }    
}
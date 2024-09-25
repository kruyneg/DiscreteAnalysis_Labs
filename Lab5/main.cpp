#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>

class SuffTree {
    struct __node;
    using edge_map = std::map<char, __node*>;

    struct __node {
        int begin;
        int end;
        edge_map edges;
        __node* reference;

        std::vector<char> owners;

        ~__node() {
            for (auto [sym, edge] : edges) {
                delete edge;
            }
        }
    };
public:
    SuffTree(const std::string& text): __size(1) {
        __root = new __node{-1, -1, {}, nullptr};
        std::vector<char> vectext{text.begin(), text.end()};
        vectext.push_back('\0');
        __texts.push_back(std::move(vectext));
        __Ukkonen(0);
    }

    SuffTree(const std::initializer_list<std::string>& ilist) : __size(ilist.size()) {
        __root = new __node{-1, -1, {}, nullptr};
        char i = 0;
        for (const auto& text : ilist) {
            std::vector<char> vectext{text.begin(), text.end()};
            vectext.push_back(i);
            __texts.push_back(std::move(vectext));
            __Ukkonen(i);
            ++i;
        }
        __check_owners(__root);
    }

    ~SuffTree() {
        delete __root;
    }

    /// @brief Добавляет новую строку в обобщённое суффиксное дерево
    void add(const std::string& text) {
        char new_ind = __size;
        ++__size;
        std::vector<char> vectext{text.begin(), text.end()};
        vectext.push_back(new_ind);

        __texts.push_back(std::move(vectext));
        __Ukkonen(new_ind);
        __check_owners(__root);
    }

    /// @brief Ищет максимальную общую подстроку для строк, имеющихся в дереве
    /// @return Массив всех общих подстрок максимальной длины
    std::vector<std::string> max_com_substr() const noexcept {
        std::vector<std::string> result;
        size_t maxlen = 0;
        std::function<void(const __node*, std::string&)> dfs = 
            [&result, &maxlen, &dfs, this] (const __node* current, std::string& str) -> void {
                if (current->owners.size() == __size) {
                    str.append(std::string{
                        std::next(__texts[current->owners.front()].begin(), current->begin),
                        std::next(__texts[current->owners.front()].begin(), current->end + 1)
                    });
                    if (str.size() > maxlen) {
                        maxlen = str.size();
                        result.clear();
                    }
                    if (str.size() == maxlen) {
                        result.push_back(str);
                    }
                } else if (current != __root) return;
                
                for (const auto [c,node] : current->edges) {
                    dfs(node, str);
                }
                if (current != __root) {
                    str.resize(str.size() - (current->end - current->begin + 1));
                }
            };
        std::string buffer;
        dfs(__root, buffer);
        return result;
    }

    void Print() {
        std::function<void(const __node*, int)> print = [this, &print](const __node* cur, int depth) {
            if (cur != __root) {
                std::cout << std::string(static_cast<size_t>(depth), '\t');
                std::cout << std::string{std::next(__texts[cur->owners.front()].begin(), cur->begin), 
                                        std::next(__texts[cur->owners.front()].begin(), cur->end + 1)};
                for (const auto i : cur->owners) {
                    std::cout << ' ' << static_cast<int>(i);
                }
            }
            std::cout << std::endl;
            for (auto [c, node] : cur->edges) {
                if (c >= 32)
                    print(node, depth + 1);
            }
        };
        return print(__root, -1);
    }

private:
    struct __builder_ptr {
        __node* cur;
        char edge;
        int len;
    };

    __node* __Ukkonen(char owner_ind) {
        const auto& root = __root;
        const auto& text = __texts[owner_ind];

        __builder_ptr ptr = {root, '\0', 0};

        int i;
        int delay = 0;
        for (i = 0; i < text.size(); ++i) {
            ++delay;
            __node* prev = nullptr; // используется для создания суффиксной ссылки
            for (int j = 0, delayed = delay; j < delayed; ++j) {
                if (ptr.cur != root && ptr.cur->owners.back() != owner_ind) {
                    ptr.cur->owners.push_back(owner_ind);
                }
                if (ptr.len == 0 && !ptr.cur->edges.contains(text[i])) {    // просто добавляем новую ветвь
                    ptr.cur->edges[text[i]] = new __node{i, int(text.size() - 1), {}, nullptr, {owner_ind}};
                    --delay;

                    if (prev && ptr.cur != root) {
                        prev->reference = ptr.cur;
                    }
                    prev = nullptr;

                    if (ptr.cur->reference)
                        ptr.cur = ptr.cur->reference;
                    else
                        ptr.cur = root;
                } else if (ptr.len == 0) {                                  // начинаем движение по одной из ветвей указателем
                    ++ptr.len;
                    ptr.edge = text[i];
                    if (prev && ptr.cur != root) {
                        prev->reference = ptr.cur;
                    }
                    break;
                } else {                                                    // разделяем ветвь посередине
                    int ind = ptr.cur->edges[ptr.edge]->begin + ptr.len;

                    if (ind > ptr.cur->edges[ptr.edge]->end) {
                        // индекс вышел за пределы текущей ветви, продвигаемся дальше вглубь
                        
                        ptr.cur = ptr.cur->edges[ptr.edge];
                        if (ptr.cur != root && ptr.cur->owners.back() != owner_ind) {
                            ptr.cur->owners.push_back(owner_ind);
                        }
                        ptr.len = ind - ptr.cur->end - 1;
                        ptr.edge = text[i - ptr.len];

                        --j;
                        continue;
                    }
                    // подставляем символы первой строки, при которой создана эта вершина --------
                    if (__texts[ptr.cur->edges[ptr.edge]->owners.front()][ind] == text[i]) { // <-l
                        // продолжаем движение по ветви указателем
                        ++ptr.len;
                        if (prev && ptr.cur != root) {
                            prev->reference = ptr.cur;
                        }
                        break;
                    } else {
                        // разделяем ветвь посередине
                        auto next = ptr.cur->edges[ptr.edge];
                        auto old = new __node{next->begin, ind - 1, {}, nullptr, next->owners};
                        next->begin = ind;
                        old->edges = edge_map{};
                        old->edges[__texts[old->owners.front()][ind]] = next;
                        old->edges[text[i]] = new __node{i, int(text.size() - 1), {}, nullptr, {owner_ind}};
                        if (old->owners.back() != owner_ind) {
                            old->owners.push_back(owner_ind);
                        }
                        ptr.cur->edges[ptr.edge] = old;

                        --delay;

                        // создание суффиксной ссылки
                        if (prev) {
                            prev->reference = old;
                        }
                        prev = old;

                        // изменение указателя для дальнейшей вставки
                        if (ptr.cur != root) {
                            if (ptr.cur->reference)
                                ptr.cur = ptr.cur->reference;
                            else 
                                ptr.cur = root;
                        } else {
                            ptr.edge = __texts
                                            [ptr.cur->edges[ptr.edge]->owners.front()]
                                            [ptr.cur->edges[ptr.edge]->begin + 1];
                            --ptr.len;
                        }
                    }
                }
            }
        }
        --i;
        return root;
    }

    void __check_owners(__node* const cur) {
        if (cur->edges.empty()) {
            return;
        } if (cur == __root) {
            for (const auto [edge, node] : cur->edges) {
                __check_owners(node);
            }
            return;
        }
        std::vector<char> own(__size, false);
        for (const auto owner : cur->owners) {
            own[owner] = true;
        }
        for (const auto [edge, node] : cur->edges) {
            __check_owners(node);
            for (const auto owner : node->owners) {
                own[owner] = true;
            }
        }
        std::vector<char> result;
        for (int i = 0; i < __size; ++i) {
            if (own[i]) {
                result.push_back(i);
            }
        } 
        cur->owners = std::move(result);
    }

    size_t __size;
    std::vector<std::vector<char>> __texts;
    __node* __root;
};

int main() {
    std::string word1, word2;
    std::getline(std::cin, word1);
    std::getline(std::cin, word2);
    SuffTree tree{word1, word2};
    // tree.Print();
    const auto answer = tree.max_com_substr();
    if (answer.empty()) {
        std::cout << 0 << std::endl;
    } else {
        std::cout << answer.front().size() << std::endl;
        for (const auto& str : answer) {
            std::cout << str << std::endl;
        }
    }
}
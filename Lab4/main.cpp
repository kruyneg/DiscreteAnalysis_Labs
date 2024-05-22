#include<iostream>
#include<vector>
#include<unordered_map>
#include<algorithm>
#include<fstream>
#include<functional>

#define fast_io std::ios::sync_with_stdio(false); std::cin.tie(0); std::cout.tie(0);

std::ostream& operator<<(std::ostream& out, const std::vector<int64_t>& vec) {
    for (auto& elem : vec) {
        out << elem << ' ';
    }
    return out;
}

template<class Iterator>
std::vector<int64_t> __Z (Iterator first, Iterator last) {
    if (first == last) {
        return {};
    }
    int64_t size = last - first;
    std::vector<int64_t> z(size);
    z[0] = 0;
    Iterator l = first, r = first;
    for (int64_t i = 1; i < size; ++i) {
        if (std::next(first, i) <= r) {
            z[i] = std::min(z[i - (l - first)], (r - first) - i + 1);
        }
        while (i + z[i] < size && *std::next(first, i + z[i]) == *std::next(first, z[i])) {
            ++z[i];
        }
        if (i + z[i] - 1 > r - first) {
            l = std::next(first, i);
            r = std::next(first, i + z[i] - 1);
        }
    }
    return z;

}

std::unordered_map<char, std::vector<int64_t>> __bad_symbol(const std::string& str) {
    std::unordered_map<char, std::vector<int64_t>> result;
    for (int64_t i = 0; i < str.size(); ++i) {
        result[str[i]].push_back(i);
    }
    return result;
}

std::vector<int64_t> __good_suf(const std::string& str) {
    std::vector<int64_t> n = __Z(str.rbegin(), str.rend());
    std::reverse(n.begin(), n.end());
    std::vector<int64_t> L(str.size(), -1);
    for (int64_t i = 0; i < n.size(); ++i) {
        if (n[i] != 0)
            L[L.size() - n[i]] = i;
    }
    std::vector<int64_t>& l = n;
    l = __Z(str.begin(), str.end());
    l[0] = l.size();
    for (int64_t i = l.size() - 2; i >= 0; --i) {
        if (l[i] < l[i + 1] || l[i] != l.size() - i) {
            l[i] = l[i + 1];
        }
    }
    for (int64_t i = 0; i < l.size(); ++i) {
        if (L[i] == -1) {
            L[i] = l[i];
        }
    }
    return L;
}

int64_t __binsearch(int64_t l, int64_t r, std::function<bool(int64_t)> check) {
    while(l != r) {
        int64_t m = (l + r) / 2;
        if (check(m)) {
            r = m;
        } else {
            l = m + 1;
        }
    }
    return l;
}

int64_t __get_offset_bs(const std::unordered_map<char, std::vector<int64_t>>& table, char symbol, int64_t index) {
    if (!table.contains(symbol)) {
        return index + 1;
    }
    const std::vector<int64_t>& vals = table.at(symbol);
    auto result_ind = __binsearch(0, vals.size(), [&vals, index](int64_t ind) -> bool {
        return vals[ind] >= index;
    });
    --result_ind;
    if (result_ind == -1 || vals[result_ind] >= index) {
        return index + 1;
    }
    return index - vals[result_ind];
}

int64_t __get_offset_gs(const std::vector<int64_t>& vals, int64_t index) {
    if (index == vals.size() - 1) {
        return 1;
    }
    ++index;
    return vals.size() - vals[index] - 1;
}

std::vector<int64_t> boyer_moore(const std::string& pattern, const std::string & text) {
    if (pattern.size() > text.size()) {
        return {};
    }
    std::vector<int64_t> result;
    std::unordered_map<char, std::vector<int64_t>> bsr = __bad_symbol(pattern);
    std::vector<int64_t> gsr = __good_suf(pattern);
    int64_t t_edge = pattern.size() - 1;
    int64_t p_edge = t_edge;
    while (t_edge < text.size()) {
        int64_t t_ind = t_edge, p_ind = p_edge;
        for (;p_ind >= 0 && text[t_ind] == pattern[p_ind]; --t_ind, --p_ind);
        if (p_ind == -1) {
            result.push_back(t_ind + 1);
        }
        t_edge += std::max({1L, __get_offset_bs(bsr, text[t_ind], p_ind), __get_offset_gs(gsr, p_ind)});
        p_edge = pattern.size() - 1;
    }
    return result;
}

int main() {
    fast_io;

    // std::fstream fin("/home/kruyneg/Programming/DiscreteAnalysis_Labs/Lab4/500.txt");

    std::string pattern, text;
    pattern.push_back(' ');
    char c;
    while(c != '\n') {
        std::cin.get(c);
        if (c == ' ' || c == '\t') {
            if (pattern.back() != ' ') {
                pattern.push_back(' ');
            }
        } else if (c == '\n') {
            if (pattern.back() != ' ') {
                pattern.push_back(' ');
            }
            continue;
        } else {
            pattern.push_back(tolower(c));
        }
    }
    int64_t curline = 1;
    int64_t curword = 1;
    std::unordered_map<int64_t, std::pair<int64_t, int64_t>> indexes;
    text.push_back(' ');
    while(std::cin.get(c)) {
        if (c == ' ' || c == '\t') {
            if (text.back() != ' ') {
                ++curword;
                text.push_back(' ');
            }
        } else if (c == '\n') {
            ++curline;
            curword = 1;
            if (text.back() != ' ') {
                text.push_back(' ');
            }
        } else {
            if (text.back() == ' ') {
                indexes[text.size()] = {curline, curword};
            }
            text.push_back(tolower(c));
        }
    }
    text.push_back(' ');
    std::vector<int64_t> answer = boyer_moore(pattern, text);
    for (const auto& index : answer) {
        std::cout << indexes[index + 1].first << ", " << indexes[index + 1].second << '\n';
    }
}
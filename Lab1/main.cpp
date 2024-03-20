#include <iostream>
#include <vector>

void __count_sort(std::vector<std::pair<std::string, std::string>>& arr, uint index) {
    std::vector<unsigned int> counter('Z' - '0' + 1, 0);
    for (auto& [key, val] : arr) {
        ++counter[key[index] - '0'];
    }
    for (size_t i = 1; i < counter.size(); ++i) {
        counter[i] += counter[i - 1];
    }
    std::vector<std::pair<std::string, std::string>> result(arr.size());
    for (int i = arr.size() - 1; i >= 0; --i) {
        int sym = arr[i].first[index] - '0';
        result[counter[sym] - 1] = std::move(arr[i]);
        --counter[sym];
    }
    arr = std::move(result);
}

void radix_sort(std::vector<std::pair<std::string, std::string>>& arr) {
    if (arr.empty()) {
        return;
    }
    for (long i = 7; i >= 0; --i) {
        if (arr.front().first[i] == ' ') {
            continue;
        }
        __count_sort(arr, i);
    }
}

std::pair<std::string, std::string> parse(const std::string& str) {
    std::string key = str.substr(0, 8);
    std::string val = str;
    return {key, val};
}

int main() {
    std::ios::sync_with_stdio(false);
	std::cin.tie(0);
	std::cout.tie(0);

    std::vector<std::pair<std::string, std::string>> array;
    std::string line;
    while (getline(std::cin, line)) {
        if (line.size() < 8) {
            continue;
        }
        array.push_back(parse(line));
    }
    radix_sort(array);
    for (auto& [key, val] : array) {
        std::cout << val << '\n';
    }
}
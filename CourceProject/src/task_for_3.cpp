#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <sstream>
#include <unordered_map>
#include <vector>

const double EPS = std::numeric_limits<double>::epsilon();

class NaiveBayes {
   public:
    NaiveBayes(int number_of_classes = 2)
        : _m_conditional_probability(number_of_classes),
          _m_class_probability(number_of_classes) {}

    void learn(const std::vector<std::string>& document,
               const std::vector<int>& classes) {
        auto doc = __prepare_doc(document);
        auto classes_num = _m_class_probability.size();

        std::unordered_map<std::string, size_t> word_counter;

        std::vector<std::unordered_map<std::string, size_t>> class_counter(
            classes_num, std::unordered_map<std::string, size_t>{});

        std::vector<size_t> class_amount(classes_num, 0);

        for (size_t i = 0; i < doc.size(); ++i) {
            class_amount[classes[i]] += doc[i].size();
            for (const auto& word : doc[i]) {
                ++class_counter[classes[i]][word];
                ++word_counter[word];
            }
        }

        for (int c = 0; c < classes_num; ++c) {
            double nc = std::count_if(classes.begin(), classes.end(),
                                      [c](int elem) { return elem == c; });
            _m_class_probability[c] = nc / classes.size();
        }

        for (int8_t c = 0; c < classes_num; ++c) {
            for (const auto& [word, cnt] : word_counter) {
                _m_conditional_probability[c][word] =
                    static_cast<double>(class_counter[c][word] + 1) /
                    (class_amount[c] + word_counter.size());
            }
        }
    }

    std::vector<int8_t> predict(
        const std::vector<std::string>& document) const {
        std::vector<int8_t> classes(document.size());
        auto doc = __prepare_doc(document);
        int classes_num = _m_class_probability.size();

        for (size_t i = 0; i < doc.size(); ++i) {
            double max_score = -std::numeric_limits<double>::max();
            int8_t best_class;
            for (int8_t c = 0; c < classes_num; ++c) {
                double score = std::log(_m_class_probability[c]);
                for (const auto& word : doc[i]) {
                    if (!_m_conditional_probability[c].count(word)) {
                        continue;
                    } else {
                        score +=
                            std::log(_m_conditional_probability[c].at(word));
                    }
                }
                // std::cout << (int)c << " log: " << score << std::endl;
                if (score > max_score) {
                    max_score = score;
                    best_class = c;
                }
            }
            classes[i] = best_class;
        }
        return classes;
    }

   private:
    std::vector<std::vector<std::string>> __prepare_doc(
        const std::vector<std::string>& doc) const {

        std::vector<std::vector<std::string>> result(doc.size());
        for (size_t i = 0; i < doc.size(); ++i) {
            result[i] = __split_and_clean(doc[i]);
        }
        return result;
    }

    std::vector<std::string> __split_and_clean(std::string s) const {
        std::transform(s.begin(), s.end(), s.begin(), [](char c) {
            if (ispunct(c))
                return ' ';
            else
                return c;
        });
        std::stringstream ss{s};
        std::vector<std::string> result;
        while (!ss.eof()) {
            std::string word;
            ss >> word;
            std::transform(word.begin(), word.end(), word.begin(), tolower);

            if (word != "") {
                result.push_back(std::move(word));
            }
        }
        return result;
    }

    std::vector<std::unordered_map<std::string, double>>
        _m_conditional_probability;
    std::vector<double> _m_class_probability;
};

int main() {
    NaiveBayes classifier;

    int train, test;
    std::cin >> train >> test;
    std::vector<std::string> train_data(train);
    std::vector<int> train_classes(train);
    for (size_t i = 0; i < train; ++i) {
        int n;
        std::cin >> n;
        train_classes[i] = n;
        std::cin.ignore(1);
        std::getline(std::cin, train_data[i]);
    }

    classifier.learn(train_data, train_classes);

    while (test--) {
        std::string text;
        std::getline(std::cin, text);

        auto res = classifier.predict(std::vector<std::string>{text});
        std::cout << (int)res.front() << std::endl;
    }
}
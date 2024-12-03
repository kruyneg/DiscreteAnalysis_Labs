#include "NaiveBayes.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>

std::vector<std::string> __split_and_clean(std::string s) {
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

std::vector<std::vector<std::string>> __prepare_doc(
    const std::vector<std::string>& doc) {

    std::vector<std::vector<std::string>> result(doc.size());
    for (size_t i = 0; i < doc.size(); ++i) {
        result[i] = __split_and_clean(doc[i]);
    }
    return result;
}

void NaiveBayes::learn(const std::vector<std::string>& document,
                       const std::vector<size_t>& classes) {
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

    for (size_t c = 0; c < classes_num; ++c) {
        double nc = std::count_if(classes.begin(), classes.end(),
                                  [c](size_t elem) { return elem == c; });
        _m_class_probability[c] = nc / classes.size();
    }

    for (size_t c = 0; c < classes_num; ++c) {
        for (const auto& [word, cnt] : word_counter) {
            _m_conditional_probability[c][word] =
                static_cast<double>(class_counter[c][word] + 1) /
                (class_amount[c] + word_counter.size());
        }
    }
}

std::vector<size_t> NaiveBayes::predict(
    const std::vector<std::string>& document) const {
    std::vector<size_t> classes(document.size());
    auto doc = __prepare_doc(document);
    size_t classes_num = _m_class_probability.size();

    for (size_t i = 0; i < doc.size(); ++i) {
        double max_score = -std::numeric_limits<double>::max();
        int8_t best_class;
        for (int8_t c = 0; c < classes_num; ++c) {
            double score = std::log(_m_class_probability[c]);
            for (const auto& word : doc[i]) {
                if (!_m_conditional_probability[c].count(word)) {
                    continue;
                } else {
                    score += std::log(_m_conditional_probability[c].at(word));
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

void NaiveBayes::save(std::ostream& stats_file) const {
    stats_file << _m_class_probability.size() << ' '
               << _m_conditional_probability.front().size() << '\n';
    stats_file << std::setprecision(17);
    for (const auto& map : _m_conditional_probability) {
        for (const auto& [word, prob] : map) {
            stats_file << word << ' ' << prob << '\n';
        }
    }
    for (const auto& prob : _m_class_probability) {
        stats_file << prob << '\n';
    }
}

void NaiveBayes::load(std::istream& stats_file) {
    size_t num_classes, vocab_size;
    stats_file >> num_classes >> vocab_size;

    _m_conditional_probability.clear();
    _m_conditional_probability.resize(num_classes);

    std::string word;
    double prob;

    for (size_t i = 0; i < num_classes; ++i) {
        for (size_t j = 0; j < vocab_size; ++j) {
            stats_file >> word >> prob;
            _m_conditional_probability[i][word] = prob;
        }
    }

    _m_class_probability.clear();
    _m_class_probability.resize(num_classes);

    for (size_t i = 0; i < num_classes; ++i) {
        stats_file >> prob;
        _m_class_probability[i] = prob;
    }
}

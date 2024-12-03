#pragma once

#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

class NaiveBayes {
   public:
    NaiveBayes(int number_of_classes = 2)
        : _m_conditional_probability(number_of_classes),
          _m_class_probability(number_of_classes) {}

    void learn(const std::vector<std::string>& document,
               const std::vector<size_t>& classes);

    std::vector<size_t> predict(
        const std::vector<std::string>& document) const;

    void save(std::ostream&) const;
    void load(std::istream&);

   private:

    std::vector<std::unordered_map<std::string, double>>
        _m_conditional_probability;
    std::vector<double> _m_class_probability;
};

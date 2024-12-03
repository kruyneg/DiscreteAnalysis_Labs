#include <boost/program_options.hpp>
#include <iostream>
#include "naive_bayes/NaiveBayes.hpp"
#include "tools/parser.hpp"

namespace po = boost::program_options;

int main(int argc, char** argv) {
    tools::Parser parser;
    tools::Options command_args;
    const char* args[] = {
        "prog", "learn",
        "--input=/home/kruyneg/Programming/DiscreteAnalysis_Labs/CourceProject/"
        "tests/input.txt",
        "--output=/home/kruyneg/Programming/DiscreteAnalysis_Labs/"
        "CourceProject/tests/output.txt"};
    char** ptr = const_cast<char**>(args);
    try {
        command_args = parser.parse(4, ptr);
    } catch (po::error err) {
        std::cerr << err.what() << std::endl;
    }

    if (command_args.command == "learn") {
        size_t length;
        *command_args.input >> length;
        command_args.input->ignore(1, '\n');
        // auto tags = parseTags(*command_args.input);
        std::vector<size_t> tags;
        std::unordered_map<std::string, size_t> tag_names;
        {
            std::string tag;
            size_t cur_id = 0;
            // Разделяем строку по запятым
            std::string line;
            std::getline(*command_args.input, line);
            std::stringstream ss{line};
            while (std::getline(ss, tag, ',')) {
                // Убираем возможные пробелы с начала и конца тега
                tag.erase(0, tag.find_first_not_of(
                                 " \t"));  // удаление пробелов с начала
                tag.erase(tag.find_last_not_of(" \t") +
                          1);  // удаление пробелов с конца

                if (!tag_names.count(tag)) {
                    tag_names[tag] = cur_id++;
                }
                tags.push_back(tag_names[tag]);
            }
        }

        std::vector<std::string> queries(tags.size());
        for (size_t i = 0; i < queries.size(); ++i) {
            for (size_t j = 0; j < length; ++j) {
                std::string line;
                std::getline(*command_args.input, line);
                queries[i] += line + '\n';
            }
        }

        NaiveBayes classifier(tag_names.size());
        classifier.learn(queries, tags);

        classifier.save(*command_args.output);
    } else if (command_args.command == "predict") {
        
    }
}
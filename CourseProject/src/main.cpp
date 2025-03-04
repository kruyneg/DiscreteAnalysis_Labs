#include <boost/program_options.hpp>
#include <iostream>
#include "naive_bayes/NaiveBayes.hpp"
#include "tools/parser.hpp"

namespace po = boost::program_options;

int main(int argc, char** argv) {
    tools::Parser parser;
    tools::Options command_args;
    // const char* args[] = {
    //     "prog", "learn",
    //     "--input=/home/kruyneg/Programming/DiscreteAnalysis_Labs/CourseProject/"
    //     "tests/input.txt",
    //     "--output=/home/kruyneg/Programming/DiscreteAnalysis_Labs/"
    //     "CourseProject/tests/output.txt"};
    // const char* args[] = {
    //     "prog", "predict",
    //     "--input=/home/kruyneg/Programming/DiscreteAnalysis_Labs/CourseProject/tests/data_generation/input_predict.txt",
    //     "--stats=/home/kruyneg/Programming/DiscreteAnalysis_Labs/CourseProject/tests/data_generation/stats_spam.txt",
    //     "--output=/home/kruyneg/Programming/DiscreteAnalysis_Labs/CourseProject/tests/data_generation/prediction_spam.txt"};
    // char** ptr = const_cast<char**>(args);
    try {
        // command_args = parser.parse(5, ptr);
        command_args = parser.parse(argc, argv);
    } catch (po::error err) {
        std::cerr << err.what() << std::endl;
    }

    if (command_args.command == "learn") {

        std::vector<std::string> queries;
        std::vector<std::vector<size_t>> tags;
        std::unordered_map<std::string, size_t> tag_names;
        size_t cur_id = 0;
        while (!command_args.input->eof()) {
            size_t length;
            *command_args.input >> length;
            command_args.input->ignore(1, '\n');
            // auto tags = parseTags(*command_args.input);
            tags.emplace_back();

            std::string tag;
            // Разделяем строку по запятым
            std::string line;
            std::getline(*command_args.input, line);
            std::stringstream ss{line};
            while (std::getline(ss, tag, ',')) {
                // Убираем возможные пробелы с начала и конца тега
                tag.erase(0, tag.find_first_not_of(" \t"));  // удаление пробелов с начала
                tag.erase(tag.find_last_not_of(" \t") + 1);  // удаление пробелов с конца

                if (!tag_names.count(tag)) {
                    tag_names[tag] = cur_id++;
                }
                tags.back().push_back(tag_names[tag]);
            }

            queries.emplace_back();
            for (size_t j = 0; j < length; ++j) {
                std::string line;
                std::getline(*command_args.input, line);
                queries.back() += line + '\n';
            }
        }

        NaiveBayes classifier(tag_names.size());
        classifier.learn(queries, tags);

        std::vector<std::string> tag_names_vec(tag_names.size());
        for (auto& [name, i] : tag_names) {
            tag_names_vec[i] = std::move(name);
        }
        for (auto& name : tag_names_vec) {
            *command_args.output << name << ';';
        }
        *command_args.output << '\n';
        classifier.save(*command_args.output);
    } else if (command_args.command == "predict") {
        std::string line;
        std::getline(*command_args.stats, line);
        std::vector<std::string> tag_names;
        // std::unordered_map<std::string, size_t> tag_names;
        {
            std::stringstream ss(line);
            while (!ss.eof()) {
                std::string tag;
                std::getline(ss, tag, ';');
                if (!tag.empty()) {
                    tag_names.push_back(tag);
                }
            }
        }

        NaiveBayes classifier(tag_names.size());
        classifier.load(*command_args.stats);

        std::vector<std::string> queries;
        size_t length;
        while (!command_args.input->eof()) {
            *command_args.input >> length;
            command_args.input->ignore(1, '\n');
            if (command_args.input->eof()) {
                break;
            }
            queries.push_back("");
            for (int i = 0; i < length; ++i) {
                if (command_args.input->eof()) {
                    queries.pop_back();
                    break;
                }
                std::getline(*command_args.input, line);
                queries.back().insert(queries.back().end(), line.begin(),
                                      line.end());
            }
        }

        auto pred = classifier.predict(queries);
        for (auto classes : pred) {
            if (classes.empty()) {
                *command_args.output << '\n';
                continue;
            }
            for (size_t i = 0; i < classes.size() - 1; ++i) {
                *command_args.output << tag_names[classes[i]] << ',';
            }
            *command_args.output << tag_names[classes.back()] << '\n';
        }
    }
}
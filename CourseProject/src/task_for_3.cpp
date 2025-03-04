#include <algorithm>
#include <boost/program_options.hpp>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
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
               const std::vector<std::vector<size_t>>& classes);

    std::vector<std::vector<size_t>> predict(
        const std::vector<std::string>& document) const;

    void save(std::ostream&) const;
    void load(std::istream&);

   private:
    std::vector<std::unordered_map<std::string, double>>
        _m_conditional_probability;
    std::vector<double> _m_class_probability;
};

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
                       const std::vector<std::vector<size_t>>& classes) {
    auto doc = __prepare_doc(document);
    auto classes_num = _m_class_probability.size();

    std::unordered_map<std::string, size_t> word_counter;

    std::vector<std::unordered_map<std::string, size_t>> class_counter(
        classes_num, std::unordered_map<std::string, size_t>{});

    std::vector<size_t> class_amount(classes_num, 0);

    for (size_t i = 0; i < doc.size(); ++i) {
        for (const auto& cls : classes[i]) {
            class_amount[cls] += doc[i].size();
            for (const auto& word : doc[i]) {
                ++class_counter[cls][word];
                ++word_counter[word];
            }
        }
    }

    for (size_t c = 0; c < classes_num; ++c) {
        double nc = std::count_if(classes.begin(), classes.end(),
                                  [c](const std::vector<size_t>& elem) {
                                      return std::find(elem.begin(), elem.end(),
                                                       c) != elem.end();
                                  });
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

std::vector<std::vector<size_t>> NaiveBayes::predict(
    const std::vector<std::string>& document) const {
    std::vector<std::vector<size_t>> classes(document.size());
    auto doc = __prepare_doc(document);
    size_t classes_num = _m_class_probability.size();

    for (size_t i = 0; i < doc.size(); ++i) {
        double max_score = -std::numeric_limits<double>::max();
        double sum_pred = 0;
        std::vector<double> scores;
        for (int8_t c = 0; c < classes_num; ++c) {
            double score = std::log(_m_class_probability[c]);
            for (const auto& word : doc[i]) {
                if (!_m_conditional_probability[c].count(word)) {
                    continue;
                } else {
                    score += std::log(_m_conditional_probability[c].at(word));
                }
            }
            sum_pred += score;
            scores.push_back(score);
        }

        double avg_score = sum_pred / classes_num;
        for (int c = 0; c < classes_num; ++c) {
            if (scores[c] > avg_score) {
                classes[i].push_back(c);
            }
        }
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

using namespace boost::program_options;

namespace tools {

struct Options {
    std::string command;
    std::shared_ptr<std::istream> input;
    std::shared_ptr<std::ostream> output;
    std::shared_ptr<std::istream> stats;
};

class Parser {
   public:
    Parser()
        : main_desc(
              "Document Classifier\n Usage: classifier [ learn | predict ] [ "
              "options ]") {

        main_desc.add_options()("help,h", "message with usage")(
            "input,i", value<std::string>()->value_name("file.txt"),
            "path to input file")("output,o",
                                  value<std::string>()->value_name("file.txt"),
                                  "path to output file")(
            "stats,s", value<std::string>()->value_name("file.txt"),
            "path to stats file, that was created during training");
        main_desc.add_options()(
            "command", value<std::string>()->value_name("learn | predict"),
            "Subcommand: learn or predict.");

        command_desc.add("command", 1);
    }

    Options parse(const int argc, char** argv) const {
        if (argc < 2) {
            main_desc.print(std::cout);
            std::exit(0);
        }
        variables_map varmap;
        store(command_line_parser(argc, argv)
                  .options(main_desc)
                  .positional(command_desc)
                  .run(),
              varmap);
        notify(varmap);

        bool has_help = varmap.count("help");
        bool has_command = varmap.count("command");
        if (has_help && has_command) {
            throw error(
                "Specify either --help or a command (learn or predict), not "
                "both.");
        } else if (!has_help && !has_command) {
            throw error(
                "Specify either --help or a command (learn or predict).");
        }

        if (has_help) {
            main_desc.print(std::cout);
            std::exit(0);
        }

        auto command = varmap["command"].as<std::string>();
        if (command == "learn") {
            std::cout << "Learning!\n";
        } else if (command == "predict") {
            std::cout << "Predicting!\n";
        } else {
            throw(error("Undefined command"));
        }

        std::shared_ptr<std::istream> input{&std::cin, [](auto) {}};
        if (varmap.count("input")) {
            auto input_path = varmap["input"].as<std::string>();
            std::cout << "Input file: " << input_path << '\n';
            auto file = new std::ifstream(input_path);
            if (!file->is_open()) {
                delete file;
                throw std::runtime_error("Failed to open input file: " +
                                         input_path);
            }
            input.reset(file, [](std::istream* ptr) { delete ptr; });
        }

        std::shared_ptr<std::ostream> output{&std::cout, [](auto) {}};
        if (varmap.count("output")) {
            auto output_path = varmap["output"].as<std::string>();
            std::cout << "Output file: " << output_path << '\n';
            auto file = new std::ofstream(output_path);
            if (!file->is_open()) {
                delete file;
                throw std::runtime_error("Failed to open output file: " +
                                         output_path);
            }
            output.reset(file, [](std::ostream* ptr) { delete ptr; });
        }

        std::shared_ptr<std::istream> stats{&std::cin, [](auto) {}};
        if (varmap.count("stats")) {
            auto stats_path = varmap["stats"].as<std::string>();
            std::cout << "Stats file: " << stats_path << '\n';
            auto file = new std::ifstream(stats_path);
            if (!file->is_open()) {
                delete file;
                throw std::runtime_error("Failed to open stats file: " +
                                         stats_path);
            }
            stats.reset(file, [](std::istream* ptr) { delete ptr; });
        }

        Options res{command, input, output, stats};
        return res;
    }

   private:
    options_description main_desc;
    positional_options_description command_desc;
};

}  // namespace tools

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
                tag.erase(0, tag.find_first_not_of(
                                 " \t"));  // удаление пробелов с начала
                tag.erase(tag.find_last_not_of(" \t") +
                          1);  // удаление пробелов с конца

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
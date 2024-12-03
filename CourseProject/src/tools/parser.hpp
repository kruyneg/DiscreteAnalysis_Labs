#pragma once

#include <boost/program_options.hpp>
#include <fstream>
#include <iostream>
#include <memory>

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
            input.reset(new std::ifstream(varmap["input"].as<std::string>()),
                        [](std::istream* ptr) { delete ptr; });
        }
        std::shared_ptr<std::ostream> output{&std::cout, [](auto) {}};
        if (varmap.count("output")) {
            output.reset(new std::ofstream(varmap["output"].as<std::string>()),
                         [](std::ostream* ptr) { delete ptr; });
        }
        std::shared_ptr<std::istream> stats{&std::cin, [](auto) {}};
        if (varmap.count("stats")) {
            stats.reset(new std::ifstream(varmap["stats"].as<std::string>()),
                        [](std::istream* ptr) { delete ptr; });
        }

        Options res{command, input, output, stats};
        return res;
    }

   private:
    options_description main_desc;
    positional_options_description command_desc;
};

}  // namespace tools

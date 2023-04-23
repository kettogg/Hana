#include "hana.h"

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <cassert>
#include "config.h"
#include "CodeGen/CodeGenContext.h"
#include "Ast/AstNode.h"
#include "Utils/GetOpt.h"

extern int yyparse();

extern int yylex_destroy();

extern FILE *yyin;
extern hana::Block *programBlock;
extern std::stack<std::string> fileNames;
extern std::vector<std::string> libPaths;
extern int parsing_error;

void usage();

// Works with path since checking from behind
bool ExtCheck(std::string &str) {
    int i = str.length()-1;
    while (str[i] != '.' && i >= 0) {
        i--;
    }
    std::string temp = "";
    for (int j = i + 1; j < str.length(); j++) {
        temp += str[j];
    }
    if (temp == "hana")
        return 1;
    return 0;
}

int main(int argc, char **argv) {
    std::string fileName;
    if (argc == 1) {
        // If no argument passed
        std::cout << "Hana version " << MAJOR_VER << "." << MINOR_VER << "." << REVISION_VER << "\n";
        std::cout << "Please enter the file name or path to execute, Use 'hana -h' for more information.\n";
        exit(0);
    }

    libPaths.push_back("./"); // current path
    bool verbose = false;
    bool quiet = false;
    bool debug = false;
    GetOpt getopt(argc, argv, "hvqdr:");
    for (auto opt: getopt) {
        switch (opt) {
            case 'h':
                usage();
                return 1;
            case 'v':
                verbose = true;
                break;
            case 'q':
                quiet = true;
                break;
            case 'd':
                debug = true;
                break;
            case 'r': {
                std::stringstream ss(getopt.get());
                std::string item;
                while (std::getline(ss, item, '|')) {
                    std::replace(std::begin(item), std::end(item), '\\', '/');
                    if (item[item.size()] != '/') {
                        item += '/';
                    }
                    libPaths.push_back(item);
                }
            }
                break;
            case EOF:
                break;
            default:
                std::cout << getopt.error() << "\n";
                usage();
                return 1;
        }
    }

    if (!quiet) {
        std::cout << "\n========================\n";
        std::cout << "   Hana version " << MAJOR_VER << "." << MINOR_VER << "." << REVISION_VER << "\n";
        std::cout << "========================\n";
    }
    auto files = getopt.getRemainingArguments();
    assert(files.size() == 1);
    fileName = files[0]; // Currently only one file is supported.

    bool flag = ExtCheck(fileName);
    if (!flag) {
        std::cout << "** Invalid File Extension, Hana files must have a format of <filename>.hana\n\n";
        exit(0);
    }

    yyin = fopen(fileName.c_str(), "r+");
    if (yyin == nullptr) {
        std::cout << "File " << fileName << "not found. Abort" << std::endl;
        return -1;
    }

    fileNames.push("");       // Add the empty file name after last EOF.
    fileNames.push(fileName); // Add the top level file name.
    if (yyparse() || parsing_error) {
        yylex_destroy();
        return 1;
    }

    if (programBlock == nullptr) {
        std::cout << "Parsing " << fileName << "failed. Abort" << std::endl;
    } else {
        std::ostringstream devNull;
        hana::CodeGenContext context(quiet ? devNull : std::cout);
        context.verbose = verbose;
        context.debug = debug;
        if (verbose)
            context.printCodeGeneration(*programBlock, std::cout);
        if (context.preProcessing(*programBlock)) {
            if (context.generateCode(*programBlock)) {
                context.runCode();
            }
        }
    }

    if (yyin != nullptr)
        fclose(yyin);
    delete programBlock;
    yylex_destroy();
    return 0;
}

void usage() {
    std::cout << "Hana version " << MAJOR_VER << "." << MINOR_VER << "." << REVISION_VER << "\n";
    std::cout << "Usage:\n";
    std::cout << "hana <filename> -h -q -v -d -r Path1|Path2|...\n";
    std::cout << "\t-h => Help menu.\n";
    std::cout << "\t-q => Quiet Mode, displays no output.\n";
    std::cout << "\t-v => Verbose mode, displays more information.\n";
    std::cout << "\t-d => Debug mode, basically skips code optimization step.\n";
    std::cout << "\t-r => Require other Hana files, seperated by the pipe '|'.\n";
}

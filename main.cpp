#include "Parser.hpp"

void printHelp();
std::string getOutputName(std::string inputName);

int main(int argc, char ** argv)
{
    if (argc < 2) {
        printHelp();
        return 1;
    }
    std::ifstream input(argv[1]);
    if (input) {
        std::string outputName = getOutputName(argv[1]);
        std::cout << "Generating to: " << outputName << std::endl;
        std::ofstream output(outputName.c_str());

        Parser parser(input, output);
        parser.parse();

        return 0;
    } else {
        std::cerr << "File " << argv[1] << " not found" << std::endl;
        return 2;
    }
}

void printHelp()
{
    std::cout << "Call 'MilanCompiler.exe <input_file.mil>'" << std::endl;
}

std::string getOutputName(std::string inputName)
{
    std::string outputName("out_");
    outputName.append(inputName);
    outputName.erase(outputName.length() - 4, outputName.length());
    outputName.append(".out");
    return outputName;
}

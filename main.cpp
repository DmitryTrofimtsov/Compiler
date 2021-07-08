#include "LEXER.h"
#include "PARSER.h"
#include "GENCODE.h"
#include "TREE.h"

int main(int argc, char* argv[])
{
    
    if (argc < 2) {                                 //if argument does not exist
        std::cout << "ERROR: File is missing";
        return -1;
    }
    
    LEXER lexer;

    try {
        lexer.Operation(std::string(argv[1]));
    }
    catch (const std::string& error) {
        std::cout << error;
        return -1;
    }

    if (!lexer.GetTableErrors()->empty()) {
        std::ofstream m_generated_file(std::string(argv[1]) + "/generated.txt");
        
        for (const auto& error : *lexer.GetTableErrors()) {
            m_generated_file << error << std::endl;
        }
        
        m_generated_file.close();
        return -1;
    }

    PARSER parser(lexer.GetTableTokens()); 
    
    try {
        parser.Operation(std::string(argv[1]));
    }
    catch (const std::string& error) {
        std::ofstream m_generated_file(std::string(argv[1]) + "/generated.txt");
        m_generated_file << error;
        m_generated_file.close();
        return -1;
    }

    GENCODE genCode(parser.GetTree(), std::string(argv[1]));

    return 0;
}

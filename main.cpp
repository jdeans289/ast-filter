#include <iostream>
#include <fstream>

#include "ast_filter.hpp"

int main (int argc, char ** argv) {
    
    if (argc != 2) {
        std::cerr << "Usage: ast-filter <filename>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];

    json ast = ASTFilter::generateFilteredAST(filename);

    std::ofstream out("ast_filtered.json");
    int indent = 4;
    out << ast.dump(indent) << std::endl;
}
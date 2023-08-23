#include <regex>
#include <iostream>
#include <set>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace ASTFilter {

    class SourceManager {
        public:
        SourceManager(std::string file);

        bool isSystem(std::string file);
        void print();

        private:

        void init (std::string file);

        std::set<std::string> system_includes;
    };

    json generateFullAST (std::string file);
    json generateFilteredAST (std::string file);

    void filter_ast(json& full_ast, SourceManager& source_manager);
}


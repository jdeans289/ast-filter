#include "ast_filter.hpp"

namespace ASTFilter {
    SourceManager::SourceManager(std::string file) {
        init(file);
    }

    bool SourceManager::isSystem(std::string file) {        
        // path is a full file path

        for (auto system_path : system_includes) {
            if (file.rfind(system_path, 0) == 0) {
                return true;
            }
        }

        return false;
    } 

    void SourceManager::print() {
        std::cout << "System paths: " << std::endl;
        for (auto path : system_includes) {
            std::cout << "\t" << path << std::endl;
        }
    }

    void SourceManager::init (std::string file) {
        FILE *fp;
        char path[20480];
        std::stringstream ss;

        // Open the command for reading
        std::string clang_ast_dump_cmd = "clang -### " + file + " 2>&1";
        fp = popen(clang_ast_dump_cmd.c_str(), "r");
        if (fp == NULL) {
            std::cerr << "Could not run command." << std::endl;
        }

        while (fgets(path, sizeof(path), fp) != NULL) {
            ss << path;
        }

        pclose(fp);

        std::string clang_output = ss.str();
        std::regex internal_isystem_match("\"-internal-isystem\" \"(\\S*)\"");
        std::smatch match;

        while (std::regex_search(clang_output, match, internal_isystem_match)) {
            // Capturing group is at index 1
            system_includes.insert(match.str(1));

            // suffix to find the rest of the string.
            clang_output = match.suffix().str();
        }

        
        clang_output = ss.str();
        std::regex internal_externc_match("\"-internal-externc-isystem\" \"(\\S*)\"");

        while (std::regex_search(clang_output, match, internal_externc_match)) {
            // Capturing group is at index 1
            system_includes.insert(match.str(1));

            // suffix to find the rest of the string.
            clang_output = match.suffix().str();
        }
    }

    json generateFilteredAST (std::string filename) {
        SourceManager s(filename);
        json ast = generateFullAST(filename);
        filter_ast(ast, s);

        return ast;
    }

    json generateFullAST (std::string file) {

        FILE *fp;
        char path[1035];
        std::stringstream ss;

        // Open the command for reading
        std::string clang_ast_dump_cmd = "clang -Xclang -ast-dump=json " + file;
        fp = popen(clang_ast_dump_cmd.c_str(), "r");
        if (fp == NULL) {
            std::cout << "Could not generate AST for file " << file << std::endl;
            return std::move(json());
        }

        while (fgets(path, sizeof(path), fp) != NULL) {
            ss << path;
        }

        pclose(fp);
        
        return json::parse(ss);
    }

    void filter_ast(json& full_ast, SourceManager& source_manager) {
        json new_inner;

        bool previous_included = false;

        for (auto entry : full_ast["inner"]) {
            if (entry.contains("isImplicit") && entry["isImplicit"] == true) {
                continue;
            }

            if (entry.contains("loc")) {
                std::string filename = "";
                json loc = entry["loc"];

                if (loc.contains("file")) {
                    filename = loc["file"];
                }

                if (loc.contains("expansionLoc")) {
                    if (loc["expansionLoc"].contains("file")) {
                        filename = loc["expansionLoc"]["file"];
                    }
                }

                if (filename == "") {
                    // If no location was found, then we are still in the last entry's location
                    if (previous_included) {
                        new_inner.push_back(entry);
                    }
                    continue;
                }

                if (!source_manager.isSystem(filename)) {
                    new_inner.push_back(entry);
                    previous_included = true;
                } else {
                    previous_included = false;
                }   
            }
        }
        full_ast["inner"] = new_inner;
    }
}
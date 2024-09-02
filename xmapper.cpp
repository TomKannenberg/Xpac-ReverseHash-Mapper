#include <iostream>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <cctype>
#include <sstream>
#include <conio.h>
#include <functional>

unsigned int sumohash(const char* pString) {
    int dwHash = 0;
    char pStringByte;
    for (unsigned int i = strlen(pString) - 1; (i & 0x80000000u) == 0; --i) {
        pStringByte = toupper(pString[i]);
        if (pStringByte == 47)
            pStringByte = 92;
        dwHash = pStringByte + 131 * dwHash;
    }
    return dwHash;
}

void saveMappingToFile() {
    std::unordered_map<unsigned int, std::string> fileMapping;

    // Open FileNames.list for reading
    std::ifstream fileNamesList("FileNames.list");
    if (!fileNamesList.is_open()) {
        std::cerr << "Error opening FileNames.list" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(fileNamesList, line)) {
        std::string opath = line;
        opath[0] = std::toupper(opath.at(0));
        std::string path = ".\\";
        path += opath;
        unsigned int fileHash = sumohash(path.c_str());

        // Check for duplicates
        if (fileMapping.find(fileHash) == fileMapping.end()) {
            fileMapping[fileHash] = path;
        } else {
            std::cout << "Duplicate found for hash " << fileHash << ": " << line << std::endl;
            // Handle duplicates as needed
        }
    }

    fileNamesList.close(); // Close FileNames.list

    std::string output = "";

    for (const auto& entry : fileMapping) {
        unsigned int hash = entry.first;
        const std::string& file = entry.second;
        output += std::to_string(hash) + ":" + file + ";\n";
    }

    std::string output2 = "";

    output2 += "#include <vector>\n#include <unordered_map>\n\n\tstd::vector<char> u = {\n\t'";
    output2 += output[0];

    bool is20 = false;
    int offset = 0;
    int formati = 60;

    for (int i = 1; i < output.length(); i++) {

        if (i == 59) {
            std::cout << "HELP!!" << std::endl;
        }

        char temp = output[i];
        if (temp == '\n') {
            if ((i - ++offset) % formati == 0) {
                output2 += "',";
                is20 = true;
            }
        } else {

            if (temp == '\\') {
                if (!is20) {
                    output2 += "',";
                } else {
                    is20 = false;
                }
                if ((i - offset) % formati == 0) {
                    output2 += '\n';
                    output2 += '\t';
                } else if (is20) {
                    is20 = false;
                }
                output2 += "'\\\\";
            } else {
                if (!is20) {
                    output2 += "',";
                }
                if ((i - offset) % formati == 0) {
                    output2 += '\n';
                    output2 += '\t';
                } else if (is20) {
                    is20 = false;
                }
                output2 += '\'';
                output2 += temp;
            }
        }
        
    }

    output2 += "'\n\t};\nstd::unordered_map<std::string, std::string> loadUnHasher() {\n\tstd::string f = \"\", s = \"\";\n\n\tstd::string* c = &f;\n\n\tstd::unordered_map<std::string, std::string> unhashmap =\n\tstd::unordered_map<std::string, std::string>();\n\n\n\tfor (auto it = u.begin(); it < u.end(); it++) {\n\t\tif (*it == ':') {\n\t\t\tc = &s;\n\t\t} else if (*it == ';') {\n\t\t\tunhashmap[f] = s;\n\t\t\tf = \"\", s = \"\";\n\t\t\tc = &f;\n\t\t} else {\n\t\t\tc->push_back(*it);\n\t\t}\n\t}\n\treturn unhashmap;\n}";

    //std::cout << output2;

    std::ofstream out("gcmapping.hpp", std::ios::binary);

    out.write(output2.c_str(), output2.size());

    out.close();

    std::ofstream out2("mgcmapping.txt", std::ios::binary);

    out2.write(output.c_str(), output.size());

    out2.close();

    std::cout << "Done" << std::endl;
}


int main() {

    saveMappingToFile();

    _getch();

}
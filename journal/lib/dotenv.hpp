#ifndef DOTENV_HPP
#define DOTENV_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace std;

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, last - first + 1);
}

void load_env(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Unable to open .env file: " << filename << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        // Пропустить пустые строки и комментарии
        if (line.empty() || line[0] == '#') continue;

        size_t pos = line.find('=');
        if (pos == string::npos) continue;

        string key = trim(line.substr(0, pos));
        string value = trim(line.substr(pos + 1));

        // Удалить комментарии внутри значения
        size_t comment_pos = value.find('#');
        if (comment_pos != string::npos) {
            value = value.substr(0, comment_pos);
            value = trim(value);
        }

        // Установить переменную среды
        setenv(key.c_str(), value.c_str(), 1);
    }
}

#endif

#include <fstream>
#include <sstream>
#include <map>
#include <stdexcept>

using namespace std;

class SQLLoader {
private:
    map<string, string> queries;
public:
    SQLLoader(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            throw runtime_error("Unable to open SQL file: " + filename);
        }

        string line, key, sql;
        while (getline(file, line)) {
            if (line.rfind("--", 0) == 0) { // Начало нового запроса
                if (!key.empty() && !sql.empty()) {
                    queries[key] = sql;
                    sql.clear();
                }
                key = line.substr(2); // Убираем "--"
                key.erase(0, key.find_first_not_of(" \t")); // Удаляем начальные пробелы
            } else if (!line.empty()) {
                sql += line + "\n";
            }
        }
        if (!key.empty() && !sql.empty()) { // Последний запрос
            queries[key] = sql;
        }
    }

    const string& getQuery(const string& key) const {
        auto it = queries.find(key);
        if (it == queries.end()) {
            throw runtime_error("Query not found: " + key);
        }
        return it->second;
    }
};

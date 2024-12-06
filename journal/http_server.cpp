#include <iostream>
#include <pqxx/pqxx>

#include "src/config/config.hpp"
#include "src//storage/storage.hpp"
#include "lib/crow_all.h"
#include "src/handlers/handlers.hpp"

#define CROW_MAIN

void createTxtFile(const string& fileName) {
    if (!filesystem::exists(fileName)) {
        ofstream outFile(fileName);
    } else {
        cerr << "File exists" << endl;
    }
}

int main() {
    config cfg = loadConfig();

    createTxtFile("group_name.txt");

    Database db(cfg.dbConfig);
    db.createTables();
    
    crow::SimpleApp app;
    setupRoutes(app, db);
    app.port(cfg.appConfig.port).bindaddr(cfg.appConfig.host).multithreaded().run();
}

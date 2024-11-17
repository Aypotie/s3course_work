#include <iostream>
#include <pqxx/pqxx>

#include "src/config/config.hpp"
#include "src//storage/storage.hpp"
#include "lib/crow_all.h"
#include "src/handlers/handlers.hpp"

#define CROW_MAIN

int main() {
    config cfg = loadConfig();

    Database db(cfg.dbConfig);
    db.createTables();
    
    crow::SimpleApp app;
    setupRoutes(app, db);
    app.port(cfg.appConfig.port).bindaddr(cfg.appConfig.host).multithreaded().run();
}

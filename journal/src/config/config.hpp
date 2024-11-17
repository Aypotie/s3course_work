#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <string>
#include "../../lib/dotenv.hpp"

using namespace std;

struct DBConfig {
    string host;
    int port;
    string password;
    string username;
    string dbName;
};

struct AppConfig {
    string host;
    int port;
};

struct config {
    DBConfig dbConfig;
    AppConfig appConfig;
};

config loadConfig() {
    load_env(".env");

    //DBConfig config

    config cfg;

    DBConfig dbcfg;
    dbcfg.host = getenv("DB_HOST");
    dbcfg.port = atoi(getenv("DB_PORT"));
    dbcfg.username = getenv("DB_USER");
    dbcfg.password = getenv("DB_PASSWORD");
    dbcfg.dbName = getenv("DB_NAME");
    cfg.dbConfig = dbcfg;

    AppConfig appCfg;
    appCfg.host = getenv("APP_HOST");
    appCfg.port = atoi(getenv("APP_PORT"));
    cfg.appConfig = appCfg;

    return cfg;
}

#endif
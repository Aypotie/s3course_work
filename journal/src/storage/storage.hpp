#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <format>
#include <stdio.h>
#include <vector>
#include <map>

#include "../config/config.hpp"
#include "../../lib/crow_all.h"

class Database {
private:
    pqxx::connection conn;
public:
    Database(DBConfig& cfg) : conn("dbname="+cfg.dbName+" user="+cfg.username+" password="+cfg.password+" hostaddr="+cfg.host+" port="+to_string(cfg.port)) {
        if (conn.is_open()) {
            cout << "Connected to database: " << conn.dbname() << endl;
        } else {
            throw runtime_error("Failed to connect to database.");
        }
    }
    void createTables() {
        try {
            pqxx::work txn(conn);
            txn.exec(
                "CREATE TABLE IF NOT EXISTS student_group ("
                "id SERIAL PRIMARY KEY, "
                "name VARCHAR(255) NOT NULL, "
                "surname VARCHAR(255) NOT NULL, "
                "lastname VARCHAR(255) NOT NULL);"
            ); 

            txn.exec (
                "CREATE TABLE IF NOT EXISTS checkpoints ("
                "id SERIAL PRIMARY KEY, "
                "name VARCHAR(255), "
                "max_score INT, "
                "score INT, " //или float
                "date DATE, "
                "description TEXT);"
            );

            // txn.exec("INSERT INTO student_group (name, surname, lastname) VALUES ('Ayslana', 'Potapova', 'Vasilievna');");

            txn.commit();
            cout << "Tables are create" << endl;
        } catch (exception &e) {
            throw runtime_error(e.what());
        }
    }

    void addUser(string name, string surname, string lastname) {
        try {
            pqxx::work txn(conn);
            txn.exec0(
                "INSERT INTO student_group (name, surname, lastname) VALUES (" +
                txn.quote(name) + ", " +
                txn.quote(surname) + ", " +
                txn.quote(lastname) + ")"
            );

            txn.commit();
        } catch (exception &e) {
            cerr << "Error: " << e.what() << endl;
            throw runtime_error(e.what());
        }
    }

    vector<map<string, crow::json::wvalue>> selectUsers() {
        vector<map<string, crow::json::wvalue>> result;
        try{
            pqxx::work txn(conn);
            pqxx::result res = txn.exec("SELECT id, name, surname, lastname FROM student_group");

            for (auto row : res) {
                map<string, crow::json::wvalue> student;
                student["id"] = row["id"].as<int>();
                student["name"] = row["name"].as<string>();
                student["surname"] = row["surname"].as<string>();
                student["lastname"] = row["lastname"].as<string>();
                result.push_back(student);
            }
        } catch (exception &e) {
            cerr << "Error" << e.what() << endl;
            throw runtime_error(e.what());
        }
        return result;
    }
};


#endif
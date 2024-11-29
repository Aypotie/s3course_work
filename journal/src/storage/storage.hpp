#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <format>
#include <stdio.h>
#include <vector>
#include <map>
#include <regex>

#include "errors.hpp"
#include "../config/config.hpp"
#include "../../lib/crow_all.h"
#include "query.hpp"

class Database {
private:
    pqxx::connection conn;
    SQLLoader sqlLoader;
public:
    bool isValidDate(const string& date) {
        regex dateRegex("^(0[1-9]|[12][0-9]|3[01])\\.(0[1-9]|1[0-2])\\.(\\d{4})$");
        return regex_match(date, dateRegex);
    }
    Database(DBConfig& cfg) : conn("dbname="+cfg.dbName+" user="+cfg.username+" password="+cfg.password+" hostaddr="+cfg.host+" port="+to_string(cfg.port)), sqlLoader("sql_query.sql") {
        if (conn.is_open()) {
            cout << "Connected to database: " << conn.dbname() << endl;
        } else {
            throw runtime_error("Failed to connect to database.");
        }
    }
    void createTables() {
        try {
            pqxx::work txn(conn);
            txn.exec(sqlLoader.getQuery("Create student_group table"));
            txn.exec(sqlLoader.getQuery("Create checkpoints table"));
            txn.exec(sqlLoader.getQuery("Create results table"));

            txn.commit();
            //cout << "Tables are create" << endl;
        } catch (exception &e) {
            throw runtime_error(e.what());
        }
    }

    void addUser(string name, string surname, string lastname) {
        try {
            pqxx::work txn(conn);
            txn.exec_params(
                sqlLoader.getQuery("Insert user"),
                name, surname, lastname
            );

            txn.commit();
        } catch (exception &e) {
            cerr << "Error: " << e.what() << endl;
            throw runtime_error(e.what());
        }
    }

    void addCheckpoint(string name, int max_score, string date, string descript) {
        try {
            pqxx::work txn(conn);
            
            txn.exec_params(
                sqlLoader.getQuery("Insert checkpoint"),
                name, max_score, date, descript
            );

            if (!isValidDate(date)) {
                throw ErrorDate("Неправильная запись даты");
            }

            txn.commit();
        }  catch (exception &e) {
            cerr << "Error: " << e.what() << endl;
            throw;
        }
    }

    void addResults(int student_id, int checkpoint_id, int score) {
        try {
            pqxx::work txn(conn);
            txn.exec_params(
                sqlLoader.getQuery("Insert result"),
                student_id, checkpoint_id, score
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
            pqxx::result res = txn.exec(sqlLoader.getQuery("Select users"));

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

    void delUser(int id) {
        try {
            pqxx::work txn(conn);
            
            pqxx::result res = txn.exec_params(sqlLoader.getQuery("Delete user"), id);
            
            if (res.affected_rows() == 0) {
                throw ErrorStudentNotFound("Студента не существует");
            }
        
            txn.commit();
        } catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
            throw;
        }
    }

    void delCheckpoint(int id) {
        try {
            pqxx::work txn(conn);
            pqxx::result res = txn.exec_params(sqlLoader.getQuery("Delete checkpoint"), id);

            if (res.affected_rows() == 0) {
                throw ErrorCheckpointNotFound("Контрольной не существует");
            }

            txn.commit();
        } catch (const std::exception& e) {
            cerr << "Error: " << e.what() << endl;
            throw;
        }
    }


    void delResult(int id)  {
        try {
            pqxx::work txn(conn);
            txn.exec_params(sqlLoader.getQuery("Delete result"), id);
            txn.commit();
        } catch (exception &e) {
            cerr << "Error: "   << e.what() << endl;
            throw runtime_error(e.what());
        }
    }

    vector<map<string, crow::json::wvalue>> selectCheckpoints() {
        vector<map<string, crow::json::wvalue>> result;
        try{
            pqxx::work txn(conn);
            pqxx::result res = txn.exec(sqlLoader.getQuery("Select checkpoints"));

            for (auto row : res) {
                map<string, crow::json::wvalue> checkpoint;
                checkpoint["id"] = row["id"].as<int>();
                checkpoint["name"] = row["name"].as<string>();
                checkpoint["max_score"] = row["max_score"].as<int>();
                checkpoint["date"] = row["date"].as<string>();
                checkpoint["description"] = row["description"].as<string>();
                result.push_back(checkpoint);
            }
        } catch (exception &e) {
            cerr << "Error" << e.what() << endl;
            throw runtime_error(e.what());
        }
        return result;
    }
    
    vector<std::map<std::string, crow::json::wvalue>> selectResults() {
        std::vector<std::map<std::string, crow::json::wvalue>> result;
        try {
            pqxx::work txn(conn);
            pqxx::result res = txn.exec(sqlLoader.getQuery("Select results"));

            for (const auto& row : res) {
                map<string, crow::json::wvalue> result_entry;
                result_entry["id"] = row["id"].as<int>();
                result_entry["student_name"] = row["surname"].as<string>() + " " + row["name"].as<string>() + " " + row["lastname"].as<string>();
                result_entry["checkpoint_name"] = row["checkpoint_name"].as<string>();
                result_entry["score"] = row["score"].as<int>();
                result.push_back(result_entry);
            }
        } catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
            throw runtime_error(e.what());
        }
        return result;
    }
};

#endif
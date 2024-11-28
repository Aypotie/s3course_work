#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <format>
#include <stdio.h>
#include <vector>
#include <map>

#include "errors.hpp"
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
                "name VARCHAR(255) NOT NULL, "
                "max_score INT NOT NULL, "
                "date VARCHAR(32), "
                "description TEXT);"
            );

            txn.exec (
                "CREATE TABLE IF NOT EXISTS results ("
                "id SERIAL PRIMARY KEY, "
                "student_id INT NOT NULL, "
                "checkpoint_id INT NOT NULL, "
                "score INT NOT NULL);"
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

    void addCheckpoint(string name, int max_score, string date, string descript) {
        try {
            pqxx::work txn(conn);
            txn.exec0(
                "INSERT INTO checkpoints (name, max_score, date, description) VALUES (" +
                txn.quote(name) + ", " +
                txn.quote(max_score) + ", " +
                txn.quote(date) + ", " +
                txn.quote(descript) + ")"
            );
            txn.commit();
        }  catch (exception &e) {
            cerr << "Error: " << e.what() << endl;
            throw runtime_error(e.what());
        }
    }

    void addResults(int student_id, int checkpoint_id, int score) {
        try {
            pqxx::work txn(conn);
            txn.exec0(
                "INSERT INTO results (student_id, checkpoint_id, score) VALUES (" +
                txn.quote(student_id) + ", " +
                txn.quote(checkpoint_id) + ", " +
                txn.quote(score) + ")"
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

    void delUser(int id) {
        try {
            pqxx::work txn(conn);
            
            pqxx::result res = txn.exec("DELETE FROM student_group WHERE id = " + txn.quote(id));
            
            if (res.affected_rows() == 0) {
                throw ErrorStudentNotFound("Студента не существует");
            }
        
            txn.commit();
        } catch (const std::exception& e) {
            cerr << "Error: " << e.what() << endl;
            throw;
        }
    }

    void delCheckpoint(int id) {
        try {
            pqxx::work txn(conn);
            pqxx::result res = txn.exec("DELETE FROM checkpoints WHERE id = " + txn.quote(id));

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
            txn.exec0("DELETE FROM results WHERE id = " + txn.quote(id));
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
            pqxx::result res = txn.exec("SELECT id, name, max_score, date, description FROM checkpoints");

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
            pqxx::result res = txn.exec(
                "SELECT results.id, student_group.lastname, student_group.surname, student_group.name, "
                "checkpoints.name AS checkpoint_name, results.score "
                "FROM results "
                "JOIN student_group ON results.student_id = student_group.id "
                "JOIN checkpoints ON results.checkpoint_id = checkpoints.id;"
            );

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
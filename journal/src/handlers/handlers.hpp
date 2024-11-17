#ifndef ROUTES_HPP
#define ROUTES_HPP

#include "../../lib/crow_all.h"
#include "../storage/storage.hpp"

void setupRoutes(crow::SimpleApp& app, Database& dbs) {

    CROW_ROUTE(app, "/")([]() {
        return "Hello, world!";
    });

    CROW_ROUTE(app, "/student").methods(crow::HTTPMethod::POST)([&dbs](const crow::request& req) {
        auto body = crow::json::load(req.body);

        if(!body || !body["name"] || !body["surname"] || !body["lastname"]) {
            return crow::response(400, "Invalid request");
        }

        string name = body["name"].s();
        string surname = body["surname"].s();
        string lastname = body["lastname"].s();

        try {
            dbs.addUser(name, surname, lastname);
        } catch (exception &e) {
            return crow::response(500, "Internal error");  
        }
        return crow::response(200);
    });

    CROW_ROUTE(app, "/student").methods(crow::HTTPMethod::GET)([&dbs](const crow::request& req) {
        try {
            vector<map<string, crow::json::wvalue>> students = dbs.selectUsers();
            crow::json::wvalue res;
            for (size_t i = 0; i < students.size(); ++i) {
                crow::json::wvalue student;
                // Вместо присваивания используем перемещение (или создание нового объекта)
                student["id"] = crow::json::wvalue(students[i]["id"]);
                student["name"] = crow::json::wvalue(students[i]["name"]);
                student["surname"] = crow::json::wvalue(students[i]["surname"]);
                student["lastname"] = crow::json::wvalue(students[i]["lastname"]);
                res["students"][i] = std::move(student);  
            }
            return crow::response(res);
        } catch (exception &e) {
            return crow::response(500, "Internal error");
        }
    });
}

#endif 

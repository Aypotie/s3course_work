#ifndef ROUTES_HPP
#define ROUTES_HPP

#include <fstream>
#include "../../lib/crow_all.h"
#include "../storage/storage.hpp"

void setupRoutes(crow::SimpleApp& app, Database& dbs) {

    CROW_ROUTE(app, "/")([]() {
        string filename = "html/index.html";
        try {
            ifstream file(filename);
            if (!file.is_open()) {
                return crow::response(404, "HTML file not exists");
            }
            string html_content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
            file.close();

            crow::response res(html_content);
            res.set_header("Content-Type", "text/html");
            return res;
        } catch (exception &e)  {
            return crow::response(500, "Internal error");
        }
    });

    CROW_ROUTE(app, "/student").methods(crow::HTTPMethod::POST)([&dbs](const crow::request& req) {
        auto body = crow::json::load(req.body);

        if (body.error()) {
            return crow::response(400, "Invalid JSON");
        }

        if (!body.has("name") || !body.has("surname") || !body.has("lastname")) {
            return crow::response(400, "Missing required fields: name, surname, lastname");
        }

        string name = body["name"].s();
        string surname = body["surname"].s();
        string lastname = body["lastname"].s();

        if (name == "") {
            return crow::response(400, "Name is empty");
        } 

        if (surname == "") {
            return crow::response(400, "surname is empty");
        } 

        if (lastname == "") {
            return crow::response(400, "lastname is empty");
        } 

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

    CROW_ROUTE(app, "/group_name").methods(crow::HTTPMethod::POST)([&dbs](const crow::request& req) {
        auto body = crow::json::load(req.body);

        if (body.error()) {
            return crow::response(400, "Invalid JSON");
        }

        if (!body.has("name")) {
            return crow::response(400, "Missing name group");
        }

        string name = body["name"].s();

        if (name == "") {
            return crow::response(400, "Name is empty");
        }

        try {
            ofstream file("group_name.txt");
            if (!file.is_open()) {
                return crow::response(500, "Failed to create file");
            }

            file <<  name << endl;
            file.close();
        } catch (exception &e) {
            return crow::response(500, "Error creating file");
        }
        
        return crow::response(200);
    });

    CROW_ROUTE(app, "/group_name").methods(crow::HTTPMethod::GET)([&dbs](const crow::request& req) {
        string filename  = "group_name.txt"; 
        try {
            ifstream file(filename);
            if (!file.is_open())  {
                return crow::response(404, "Group not exists");
            }
            string group_name;;

            getline(file, group_name);
            file.close();

            if(group_name.empty()) {
                return crow::response(400, "File is empty");
            }

            crow::json::wvalue response;
            response["group_name"] = group_name;
            return crow::response(200, response);
        } catch (exception &e) {
            return crow::response(500, "Internal error");
        }
    });
}

#endif 

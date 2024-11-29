#ifndef ROUTES_HPP
#define ROUTES_HPP

#include <fstream>
#include <vector>

#include "../storage/errors.hpp"
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
    //name varchar , max_score int , date date, ddescription text
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
            for (size_t i = 0; i < students.size(); i++) {
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

    CROW_ROUTE(app, "/student/<int>").methods(crow::HTTPMethod::DELETE)([&dbs](int id){
        try {
            dbs.delUser(id);
            return crow::response(200);
        } catch (const ErrorStudentNotFound &e) {
            return crow::response(400, e.what());
        }catch (exception &e) {
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

    CROW_ROUTE(app, "/checkpoints").methods(crow::HTTPMethod::POST)([&dbs](const crow::request& req) {
        auto body = crow::json::load(req.body);

        if (body.error()) {
            return  crow::response(400, "Invalid JSON");
        }

        if (!body.has("name") || !body.has("max_score") || !body.has("date") || !body.has("description")) {
            return crow::response(400, "Missing required fields: name, max_score, date, description");
        }

        string name = body["name"].s();
        int max_score = body["max_score"].i();
        string date = body["date"].s();
        string descript = body["description"].s();
        
        if (name == "") {
            return crow::response(400, "name is empty");
        }

        if (date == "") {
            return crow::response(400, "date is empty");
        }

        if (max_score <= 0) {
            return crow::response(400, "max_score is empty");
        }

        if (descript == "") {
            return crow::response(400, "description is empty");
        }

        try {
            dbs.addCheckpoint(name, max_score, date, descript);
            return crow::response(200);
        } catch (const ErrorDate &e) {
            return crow::response(400, e.what());
        } catch (exception &e) {
            return crow::response(500, "Internal error");
        }
    });

    CROW_ROUTE(app, "/checkpoints").methods(crow::HTTPMethod::GET)([&dbs](const crow::request& req) {
        try {
            vector<map<string, crow::json::wvalue>> checkpoints = dbs.selectCheckpoints();
            crow::json::wvalue res;
            for (size_t i = 0; i < checkpoints.size(); i++) {
                crow::json::wvalue checkpoint;
                checkpoint["id"] = crow::json::wvalue(checkpoints[i]["id"]);
                checkpoint["name"] = crow::json::wvalue(checkpoints[i]["name"]);
                checkpoint["max_score"] = crow::json::wvalue(checkpoints[i]["max_score"]);
                checkpoint["date"] = crow::json::wvalue(checkpoints[i]["date"]);
                checkpoint["description"] = crow::json::wvalue(checkpoints[i]["description"]);

                res["checkpoints"][i] = move(checkpoint);
            }
            return crow::response(res);
        } catch (exception &e) {
            return crow::response(500, "Internal error");
        }
    });

    CROW_ROUTE(app, "/checkpoints/<int>").methods(crow::HTTPMethod::DELETE)([&dbs](int id) {
        try {
            dbs.delCheckpoint(id);
            return crow::response(200);
        } catch (const ErrorCheckpointNotFound& e) {
            return crow::response(400, e.what());
        } catch (const exception& e) {
            return crow::response(500, "Internal Server Error");
        }
    });


    CROW_ROUTE(app, "/results/<int>").methods(crow::HTTPMethod::DELETE)([&dbs](int id) {
        try {
            dbs.delResult(id);
            return crow::response(200);
        } catch (const std::exception& e) {
            return crow::response(500, "Internal error");
        }
    });


    CROW_ROUTE(app, "/results").methods(crow::HTTPMethod::GET)([&dbs]() {
        try {
            vector<map<string, crow::json::wvalue>> res = dbs.selectResults(); 
            crow::json::wvalue result;
            for (size_t i = 0; i < res.size(); i++) {
                crow::json::wvalue work;
                work["id"] = crow::json::wvalue(res[i]["id"]);
                work["student_name"] = crow::json::wvalue(res[i]["student_name"]);
                work["checkpoint_name"] = crow::json::wvalue(res[i]["checkpoint_name"]);
                work["score"] = crow::json::wvalue(res[i]["score"]);
                
                result["results"][i] = move(work);
            }
            return crow::response(result);
        } catch (exception& e) {
            return crow::response(500, "Internal Server Error");
        }
    });

    CROW_ROUTE(app, "/results").methods(crow::HTTPMethod::POST)([&dbs](const crow::request& req) {
        auto body = crow::json::load(req.body);

        if (body.error()) {
            return crow::response(400, "Invalid JSON");
        }

        if (!body.has("student_id") || !body.has("checkpoint_id") || !body.has("score")) {
            return crow::response(400, "Missing required fields: student_id, checkpoint_id, score");
        }

        int student_id = body["student_id"].i();
        int checkpoint_id = body["checkpoint_id"].i();
        int score = body["score"].i();

        if (score < 0) {
            return crow::response(400, "Invalid score");
        }

        try {
            dbs.addResults(student_id, checkpoint_id, score);
        } catch (exception& e) {
            return crow::response(500, "Internal error");
        }
        return crow::response(200);
    });

}

#endif 

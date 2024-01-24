#pragma once
#include <pqxx/pqxx>
#include <iostream>
#include <memory>

class DatabaseConnection {
public:
    static DatabaseConnection& getInstance() {
        static DatabaseConnection instance;
        return instance;
    }

    pqxx::connection* getConnection() {
        return conn.get();
    }

private:
    std::unique_ptr<pqxx::connection> conn;

    DatabaseConnection() {
        try {
            conn = std::make_unique<pqxx::connection>("host=localhost dbname=postgres user=postgres password=password");
            if (conn->is_open()) {
                std::cout << "Database connection established." << std::endl;
            }
            else {
                std::cerr << "Database connection failed." << std::endl;
            }
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
    }

    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;

    ~DatabaseConnection() {
        if (conn && conn->is_open()) {
            conn->close();
            std::cout << "Database connection closed." << std::endl;
        }
    }
};
#pragma once
#include "DatabaseConnection.hpp"
#include "validate.hpp"
#include "Match.hpp"
#include <iostream>
#include <string>


class Player {
public:
    Player() {}

    static void addPlayer(const std::string& firstName, const std::string& lastName) {
        DatabaseConnection& db = DatabaseConnection::getInstance();
        if (!isAlpha(firstName) || !isAlpha(lastName)) {
            std::cout << "Names should contain only letters." << std::endl;
            return;
        }
        if (firstName.length() > 25 || lastName.length() > 25) {
            std::cout << "Names should have maximum 25 letters." << std::endl;
            return;
        }

        std::string formattedFirstName = formatName(firstName);
        std::string formattedLastName = formatName(lastName);

        try {
            std::string query = "INSERT INTO public_to.players (first_name, last_name) VALUES ('" + formattedFirstName + "', '" + formattedLastName + "');";
            pqxx::work w(*db.getConnection());
            w.exec(query);
            w.commit();
            std::cout << "Player added successfully." << std::endl;

        }
        catch (const pqxx::sql_error& e) {
            std::cerr << "SQL error: " << e.what() << std::endl;
            std::cerr << "Query was: " << e.query() << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Exception in addPlayer: " << e.what() << std::endl;
        }
    }

    static void showPlayerStatistics(int playerId = -1) {
        DatabaseConnection& db = DatabaseConnection::getInstance();
        std::string query = "SELECT * FROM public_to.players";
        if (playerId != -1) {
            query += " WHERE id = " + std::to_string(playerId);
        }
        pqxx::nontransaction nt(*db.getConnection());
        pqxx::result r = nt.exec(query);
        for (auto row : r) {
            std::cout << "Player ID: " << row[0].as<int>() << ", First Name: " << row[1].as<std::string>() << ", Last Name: " << row[2].as<std::string>() << std::endl;
        }
    }

    static void updateMatchResults(int winnerId, int loserId) {
        DatabaseConnection& db = DatabaseConnection::getInstance();

        try {
            pqxx::work w(*db.getConnection());

            std::string queryWinner = "UPDATE public_to.players SET matches_won = matches_won + 1 WHERE id = " + std::to_string(winnerId) + ";";
            w.exec(queryWinner);

            std::string queryLoser = "UPDATE public_to.players SET matches_lost = matches_lost + 1 WHERE id = " + std::to_string(loserId) + ";";
            w.exec(queryLoser);

            w.commit();
            std::cout << "Match results updated successfully." << std::endl;
        }
        catch (const pqxx::sql_error& e) {
            std::cerr << "SQL error: " << e.what() << std::endl;
            std::cerr << "Query was: " << e.query() << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Exception in updateMatchResults: " << e.what() << std::endl;
        }
    }
};
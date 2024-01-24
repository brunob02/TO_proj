#include "DatabaseConnection.hpp"
#include "validate.hpp"
#include "Player.hpp"

void showMatches(DatabaseConnection& db, int matchId = -1) {
    std::string query = "SELECT * FROM public_to.matches";
    if (matchId != -1) {
        query += " WHERE id = " + std::to_string(matchId);
    }
    pqxx::nontransaction nt(*db.getConnection());
    pqxx::result r = nt.exec(query);
    for (auto row : r) {
        std::cout << "Match ID: " << row[0].as<int>() << ", Player 1 ID: " << row[1].as<int>() << ", Player 2 ID: " << row[2].as<int>() << ", Status: " << row[5].as<std::string>() << std::endl;
    }
}

bool playerExists(DatabaseConnection& db, int playerId) {
    pqxx::nontransaction nt(*db.getConnection());
    std::string query = "SELECT COUNT(*) FROM public_to.players WHERE id = " + std::to_string(playerId) + ";";
    pqxx::result r = nt.exec(query);
    return r[0][0].as<int>() > 0;
}

int main() {
    DatabaseConnection& db = DatabaseConnection::getInstance();
    pqxx::connection* conn = db.getConnection();

    if (conn) {
        std::cout << "Connection successful" << std::endl;
    }
    else {
        std::cout << "Connection failed" << std::endl;
    }

    while (true) {
        std::cout << "1. Add player\n";
        std::cout << "2. Show players\n";
        std::cout << "3. Add match\n";
        std::cout << "4. Show match\n";
        std::cout << "0. Exit\n";
        std::cout << "Enter choice: ";
        int choice;
        std::cin >> choice;

        if (choice == 0) break;

        switch (choice) {
        case 1: {
            std::string firstName, lastName;
            std::cout << "Enter First Name: ";
            std::cin >> firstName;
            std::cout << "Enter Last Name: ";
            std::cin >> lastName;
            Player::addPlayer(firstName, lastName);
            break;
        }
        case 2: {
            int playerId;
            std::cout << "Enter Player ID (or -1 for all): ";
            std::cin >> playerId;
            Player::showPlayerStatistics(playerId);

            break;
        }
        case 3: {
            int player1Id, player2Id, noSets;

            std::cout << "Enter Player 1 ID: ";
            std::cin >> player1Id;

            std::cout << "Enter Player 2 ID: ";
            std::cin >> player2Id;

            std::cout << "Enter number of sets: ";
            std::cin >> noSets;

            std::tm dateTm = {}; 
            std::string dateInput, timeInput;

            std::cout << "Enter the date (MM-DD): ";
            std::cin >> dateInput;
            if (dateInput.size() == 5) {
                time_t now = time(0);
                tm ltm; 
                localtime_s(&ltm, &now); 

                int year = 1900 + ltm.tm_year;
                dateInput = std::to_string(year) + "-" + dateInput;
            }

            std::istringstream dateStream(dateInput);
            dateStream >> std::get_time(&dateTm, "%Y-%m-%d");
            if (dateStream.fail()) {
                std::cerr << "Invalid date format." << std::endl;
                break;
            }

            std::cout << "Enter the time (HH): ";
            std::cin >> timeInput;
            if (timeInput.size() == 2) {
                timeInput += ":00:00";
            }

            std::istringstream timeStream(timeInput);
            timeStream >> std::get_time(&dateTm, "%H:%M:%S");
            if (timeStream.fail()) {
                std::cerr << "Invalid time format." << std::endl;
                break;
            }

            dateTm.tm_isdst = -1; 

            time_t matchTime = mktime(&dateTm);
            if (matchTime == -1) {
                std::cerr << "Failed to convert time." << std::endl;
                break;
            }

            char buffer[80];
            strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", &dateTm);
            std::string predictedStartTime(buffer);

            try {
                Match newMatch(player1Id, player2Id, noSets, predictedStartTime);
                std::cout << "New match scheduled." << std::endl;
            }
            catch (const std::exception& e) {
                std::cerr << "An error occurred while scheduling the match: " << e.what() << std::endl;
            }
            break;
        }
        case 4: {
            int matchId;
            std::cout << "Podaj ID meczu (lub -1 dla wszystkich): ";
            std::cin >> matchId;
            showMatches(db, matchId);
            break;
        }
        default:
            std::cout << "Unknown option!\n";
        }
    }

    return 0;
}
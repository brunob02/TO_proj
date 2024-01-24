#include "Match.hpp"
#include "MatchState.hpp"
#include <iostream>
#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>

Match::Match(int player1Id, int player2Id, int noSets, const std::string& predictedStartTime)
    : player_id1(player1Id), player_id2(player2Id), winner_id(-1), predicted_start_time(predictedStartTime),
    actual_start_time(""), no_sets(noSets) {
    if (!playerExists(player_id1) || !playerExists(player_id2)) {
        throw std::invalid_argument("One or both players do not exist.");
    }
    changeState(new PendingState());
    currentState->handle(this);

    if (isLaterThanNow(predictedStartTime)) {
        changeState(new DelayedState());
        currentState->handle(this);
    }

    saveToDatabase();
}

bool Match::playerExists(int playerId)
{
    DatabaseConnection& db = DatabaseConnection::getInstance();
    pqxx::nontransaction nt(*db.getConnection());
    std::string query = "SELECT COUNT(*) FROM public_to.players WHERE id = " + std::to_string(playerId) + ";";
    pqxx::result r = nt.exec(query);
    return r[0][0].as<int>() > 0;
}

void Match::startMatch() {
    timer.start();

    actual_start_time = getCurrentTime();
    changeState(new StartedState());
    currentState->handle(this);

    if (id == -1) {
        std::cerr << "Match ID not set. Cannot update match." << std::endl;
        return;
    }

    DatabaseConnection& db = DatabaseConnection::getInstance();
    try {
        pqxx::work w(*db.getConnection());
        std::string matchQuery = "UPDATE public_to.matches SET actual_start_time = '" + actual_start_time +
            ", status_id = " + std::to_string(status_id) + " WHERE id = " + std::to_string(id) + ";";
        w.exec(matchQuery);
        w.commit();
        std::cout << "Match with ID " << id << " updated successfully." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception in updateMatchInDatabase: " << e.what() << std::endl;
    }
}

void Match::endMatch(int winningPlayerId) {
    int elapsedMinutes = timer.stop(); 
    duration += std::chrono::minutes(elapsedMinutes);

    winner_id = winningPlayerId;
    changeState(new FinishedState());
    currentState->handle(this);

    updateMatchInDatabase();

}

void Match::suspendMatch() {
    changeState(new SuspendedState());
    currentState->handle(this);

    updateMatchInDatabase();
}



void Match::saveToDatabase() {
    DatabaseConnection& db = DatabaseConnection::getInstance();
    try {
        pqxx::work w(*db.getConnection());
        std::string query = "INSERT INTO public_to.matches (status_id, player_id1, player_id2, predicted_start_time, duration, no_sets) VALUES (" +
            std::to_string(status_id) + ", " + std::to_string(player_id1) + ", " +
            std::to_string(player_id2) + ", '" + predicted_start_time + "', '00:00:00', " + std::to_string(no_sets) + ") RETURNING id;";

        pqxx::result r = w.exec(query);
        if (!r.empty()) {
            id = r[0]["id"].as<int>(); 
        }
        w.commit();
        std::cout << "Match saved successfully with ID: " << id << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception in saveToDatabase: " << e.what() << std::endl;
    }
}

void Match::updateMatchInDatabase() {
    if (id == -1) {
        std::cerr << "Match ID not set. Cannot update match." << std::endl;
        return;
    }

    DatabaseConnection& db = DatabaseConnection::getInstance();
    try {
        pqxx::work w(*db.getConnection());
        std::string durationQuery = "UPDATE public_to.matches SET duration = duration + interval '" +
            std::to_string(duration.count()) + " minutes' WHERE id = " + std::to_string(id) + ";";
        w.exec(durationQuery);

        
        std::string matchQuery = "UPDATE public_to.matches SET actual_start_time = '" + actual_start_time +
            "', winner_id = " + std::to_string(winner_id) + ", status_id = " + std::to_string(status_id) +
            " WHERE id = " + std::to_string(id) + ";";
        w.exec(matchQuery);
        w.commit();
        std::cout << "Match with ID " << id << " updated successfully." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception in updateMatchInDatabase: " << e.what() << std::endl;
    }
}

std::string Match::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::tm tm_buf;
    localtime_s(&tm_buf, &in_time_t); 

    std::stringstream ss;
    ss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S"); 
    return ss.str();
}

bool Match::isLaterThanNow(const std::string& dateTimeStr) {
    std::tm tm = {};
    std::istringstream ss(dateTimeStr);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

    if (ss.fail()) {
        return false;
    }

    auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    auto now = std::chrono::system_clock::now();

    return tp > now;
}

int Match::getStatusId(const std::string& statusName) {
    DatabaseConnection& db = DatabaseConnection::getInstance();
    try {
        pqxx::nontransaction nt(*db.getConnection());
        std::string query = "SELECT id FROM public_to.match_status WHERE status = '" + statusName + "';";
        pqxx::result r = nt.exec(query);

        if (r.size() == 0) {
            throw std::runtime_error("Status not found.");
        }

        return r[0][0].as<int>();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception in getStatusId: " << e.what() << std::endl;
        throw;
    }
}

Match::~Match() {
    delete currentState;
}

void Match::changeState(MatchState* state) {
    delete currentState;
    currentState = state;
}
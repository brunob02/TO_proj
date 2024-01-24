#pragma once
#include "DatabaseConnection.hpp"
#include "validate.hpp"
#include "Timer.cpp"

class MatchState;

class Match {
private:
    int id;
    int player_id1;
    int player_id2;
    int winner_id;
    std::string predicted_start_time;
    std::string actual_start_time;
    int no_sets;
    MatchState* currentState;
    Timer timer;
    std::chrono::minutes duration;
public:
    int status_id;
    Match(int player1Id, int player2Id, int noSets, const std::string& predictedStartTime);

    static bool playerExists(int playerId);

    void startMatch();

    void endMatch(int winningPlayerId);

    void suspendMatch();

    void saveToDatabase();

    void updateMatchInDatabase();

    static std::string getCurrentTime();

    static bool isLaterThanNow(const std::string& dateTimeStr);

    static int getStatusId(const std::string& statusName);

    ~Match();

    void changeState(MatchState* state);
};
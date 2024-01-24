#pragma once
#include "Match.hpp"

class MatchState {
public:
    virtual void handle(Match* match) = 0;
    ~MatchState() {}
};

class PendingState : public MatchState {
public:
    void handle(Match* match) {
        match->status_id = Match::getStatusId("Pending");
    }
};

class StartedState : public MatchState {
public:
    void handle(Match* match) {
        match->status_id = Match::getStatusId("Started");
    }
};

class SuspendedState : public MatchState {
public:
    void handle(Match* match) {
        match->status_id = Match::getStatusId("Suspended");
    }
};

class FinishedState : public MatchState {
public:
    void handle(Match* match) {
        match->status_id = Match::getStatusId("Finished");
    }
};

class DelayedState : public MatchState {
public:
    void handle(Match* match) {
        match->status_id = Match::getStatusId("Delayed");
    }
};

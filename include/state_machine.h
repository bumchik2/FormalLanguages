#pragma once

#include <vector>
#include <iostream>
#include <string>

using std::string;
using std::vector;
using std::ostream;
using std::istream;

struct Edge { // we will only work with 1-letter state machines
    int to = -1;
    char symbol;
};

struct EdgeExtended {
    int from = -1;
    int to = -1;
    char symbol;
};

bool operator == (const Edge& e1, const Edge& e2);
ostream& operator << (ostream& os, Edge edge);

struct StateMachine {
    int states_number;
    int starting_state = 0;
    vector<vector<Edge>> edges;
    vector<int> terminal_states;
    string alphabet;

    StateMachine() = default;
    StateMachine(int new_n, string new_alphabet, const vector<EdgeExtended>& new_edges,
            const vector<int>& new_terminal_vertex, int new_starting_vertex = 0);

    void initialize(int new_n, string new_alphabet, int new_starting_vertex = 0);
    void addEdge(int from, int to, char symbol);
    void addEdge(const EdgeExtended&);
    void addTerminalState(int new_terminal_vertex);
    void invert(bool print_log = false);

    int go(int from, char c) const;

    bool is_determined() const;
    bool is_full() const;
};

ostream& operator << (ostream& os, const StateMachine&);

StateMachine removeRedundantStates(const StateMachine& sm);
StateMachine determined(const StateMachine& sm, bool print_log = false);
StateMachine determinedFull(const StateMachine& sm, bool print_log = false);
StateMachine determinedMinimal(const StateMachine&, bool print_log = false);

istream& operator >> (istream& is, StateMachine&);

StateMachine getMinimalFullDeterminedStateMachine(const StateMachine&,
        bool print_log = false);
StateMachine getMinimalFullDeterminedAdditionStateMachine(const StateMachine&, 
        bool print_log = false);

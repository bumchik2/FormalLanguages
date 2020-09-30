#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <queue>
#include <sstream>
#include <utility>
#include <cstdlib>

using std::pair;
using std::vector;
using std::string;
using std::unordered_map;
using std::runtime_error;
using std::exception;
using std::queue;
using std::max;
using std::min;

using std::ostream;
using std::istream;
using std::cerr;
using std::ostringstream;
using std::cin;
using std::cout;
using std::endl;

#include "state_machine.h"


bool operator == (const Edge& e1, const Edge& e2) {
    return e1.to == e2.to && e1.symbol == e2.symbol;
}

ostream& operator << (ostream& os, Edge edge) {
    os << "letter: " << edge.symbol << "; to: " << edge.to;
    return os;
}


StateMachine::StateMachine(int new_n, string new_alphabet, const vector<EdgeExtended>& new_edges,
            const vector<int>& new_terminal_states, int new_starting_vertex) {
    initialize(new_n, new_alphabet, new_starting_vertex);
    for (unsigned int i = 0; i < new_edges.size(); ++i) {
        addEdge(new_edges[i]);
    }
    for (unsigned int i = 0; i < new_terminal_states.size(); ++i) {
        addTerminalState(new_terminal_states[i]);
    }
}

void StateMachine::initialize(int new_states_number, string new_alphabet, 
            int new_starting_state) {
    states_number = new_states_number;
    alphabet = new_alphabet;
    starting_state = new_starting_state;
    edges = vector<vector<Edge>>(new_states_number);
    terminal_states.clear();
}

void StateMachine::addEdge(int from, int to, char symbol) {
    if (from < 0 || from >= states_number || to < 0 || to >= states_number) {
        throw runtime_error("incorrect vertex number in addEdge");
    }

    if (find(alphabet.begin(), alphabet.end(), symbol) == alphabet.end()) {
        throw runtime_error("the alphabet doesn't contain the symbol");
    }

    if (find(edges[from].begin(), edges[from].end(), Edge{to, symbol}) != edges[from].end()) {
        return;
    }

    edges[from].push_back(Edge{to, symbol});
}

void StateMachine::addEdge(const EdgeExtended& edge_expanded) {
    addEdge(edge_expanded.from, edge_expanded.to, edge_expanded.symbol);
}

void StateMachine::addTerminalState(int new_terminal_state) {
    if (new_terminal_state < 0 || new_terminal_state >= states_number) {
        throw runtime_error("incorrect vertex number in addTerminalState");
    }

    if (find(terminal_states.begin(), terminal_states.end(), new_terminal_state)
                == terminal_states.end()) {
        terminal_states.push_back(new_terminal_state);
    }
}

void StateMachine::invert(bool print_log) {
    vector<int> new_terminal_states;
    for (int i = 0; i < states_number; ++i) {
        if (find(terminal_states.begin(), terminal_states.end(), i) == terminal_states.end()) {
            new_terminal_states.push_back(i);
        }
    }
    terminal_states = std::move(new_terminal_states);
    if (print_log) {
        cout << "Edges have been inverted successfully!" << endl;
    }
}

int StateMachine::go(int from, char symbol) const {
    int answer = -1;
    for (unsigned int edge_number = 0; edge_number < edges[from].size(); ++edge_number) {
        Edge current_edge = edges[from][edge_number];
        if (current_edge.symbol == symbol && answer == -1) {
            answer = current_edge.to;
        } else if (current_edge.symbol == symbol && answer != -1) {
            throw runtime_error("can't call go-method in a non-determined state machine");
        }
    }
    return answer;
}

bool StateMachine::is_determined() const {
    for (int from = 0; from < states_number; ++from) {
        for (unsigned symbol_number = 0; symbol_number < alphabet.size(); ++symbol_number) {
            char symbol = alphabet[symbol_number];
            int to_number = 0;
            for (unsigned int edge_number = 0; edge_number < edges[from].size(); ++edge_number) {
                Edge current_edge = edges[from][edge_number];
                if (current_edge.symbol == symbol) {
                    ++to_number;
                }
            }
            if (to_number > 1) {
                return false;
            }
        }
    }
    return true;
}

bool StateMachine::is_full() const {
    for (int from = 0; from < states_number; ++from) {
        for (unsigned int symbol_number = 0; symbol_number < alphabet.size(); ++symbol_number) {
            char symbol = alphabet[symbol_number];
            if (go(from, symbol) == -1) {
                return false;
            }
        }
    }
    return true;
}

ostream& operator << (ostream& os, const StateMachine& state_machine) {
    os << "State number: " << state_machine.states_number << 
            "; Starting state: " << state_machine.starting_state << '\n';
    os << "Alphabet: " << state_machine.alphabet << '\n';
    os << "Edges:\n";
    for (int from = 0; from < state_machine.states_number; ++from) {
        os << from << ": ";
        for (unsigned int edge_number = 0; edge_number < state_machine.edges[from].size(); 
                    ++edge_number) {
            if (edge_number != 0) {
                os << "; ";
            }
            os << state_machine.edges[from][edge_number];
        }
        os << '\n';
    }
    os << "Terminal states:\n";
    for (unsigned int i = 0; i < state_machine.terminal_states.size(); ++i) {
        os << state_machine.terminal_states[i] << ' ';
    }
    os << '\n';
    return os;
}

void dfs(const StateMachine& state_machine, vector<bool>& used, int current_vertex) {
    used[current_vertex] = true;
    for (unsigned edge_number = 0; edge_number < state_machine.edges[current_vertex].size(); ++edge_number) {
        Edge current_edge = state_machine.edges[current_vertex][edge_number];
        if (!used[current_edge.to]) {
            dfs(state_machine, used, current_edge.to);
        }
    }
}

StateMachine removeRedundantStates(const StateMachine& state_machine) { // state_machine can be anything
    vector<bool> used(state_machine.states_number, false);
    vector<int> old_numbers_by_new_numbers;
    unordered_map<int, int> new_numbers_by_old_numbers;

    dfs(state_machine, used, state_machine.starting_state);

    for (int i = 0; i < state_machine.states_number; ++i) {
        if (used[i]) {
            old_numbers_by_new_numbers.push_back(i);
            new_numbers_by_old_numbers[i] = old_numbers_by_new_numbers.size() - 1;
        }
    }

    StateMachine answer;
    answer.initialize(old_numbers_by_new_numbers.size(), state_machine.alphabet, 
            new_numbers_by_old_numbers[state_machine.starting_state]);

    // adding all the edges
    for (int old_from = 0; old_from < state_machine.states_number; ++old_from) {
        if (!used[old_from]) {
            continue;
        }
        for (unsigned int edge_number = 0; edge_number < state_machine.edges[old_from].size();
                    ++edge_number) {
            Edge current_edge = state_machine.edges[old_from][edge_number];
            int new_from = new_numbers_by_old_numbers[old_from];
            int new_to  = new_numbers_by_old_numbers[current_edge.to];
            answer.addEdge(new_from, new_to, current_edge.symbol);
        }
    }

    // marking terminal vertex
    for (unsigned i = 0; i < state_machine.terminal_states.size(); ++i) {
        int old_terminal_state = state_machine.terminal_states[i];
        if (used[old_terminal_state]) {
            answer.addTerminalState(new_numbers_by_old_numbers[old_terminal_state]);
        }
    }
    return answer;
}

int getSet(int vertex) {
    return 1 << vertex;
}

bool setContainsState(int set_number, int vertex_number) {
    return (set_number >> vertex_number) & 1;
}

void printSet(int set_number, int max_vertex_number) {
    for (int i = 0; i < max_vertex_number; ++i) {
        if (setContainsState(set_number, i)) {
            cout << i;
        }
    }
}

int getToSet(const StateMachine& state_machine, int current_set, char symbol) {
    int to_set = 0;

    for (int vertex_number = 0; vertex_number < state_machine.states_number; ++vertex_number) {
        if (!setContainsState(current_set, vertex_number)) {
            continue;
        }
        for (unsigned edge_number = 0; edge_number < state_machine.edges[vertex_number].size();
                    ++edge_number) {
            Edge current_edge = state_machine.edges[vertex_number][edge_number];
            if (current_edge.symbol != symbol) {
                continue;
            }
            to_set |= getSet(current_edge.to);
        }
    }

    return to_set;
}

StateMachine determined(const StateMachine& state_machine, bool print_log) { // state_machine can be anything
    if (print_log) {
        cout << "--------DETERMINING-PROCESS-STARTS---------" << endl;
    }

    StateMachine pre_answer;
    pre_answer.initialize(1 << state_machine.states_number, state_machine.alphabet, 
            getSet(state_machine.starting_state));
    string alphabet = pre_answer.alphabet = state_machine.alphabet;

    int starting_set = getSet(state_machine.starting_state);
    queue<int> q;
    q.push(starting_set);

    vector<bool> used(1 << state_machine.states_number, false);
    used[starting_set] = true;

    // adding all the edges in this cycle
    while(q.size() != 0) {
        int current_set = q.front();
        for (unsigned symbol_number = 0; symbol_number < alphabet.size(); ++symbol_number) {
            char symbol = alphabet[symbol_number];
            if (print_log) {
                printSet(current_set, state_machine.states_number);
                cout << " " << symbol << " ";
            }

            int to_set = getToSet(state_machine, current_set, symbol);

            if (print_log) {
                printSet(to_set, state_machine.states_number);
                cout << endl;
            }
            pre_answer.addEdge(current_set, to_set, symbol);

            if (used[to_set]) {
                continue;
            }
            used[to_set] = true;
            q.push(to_set);
        }
        q.pop();
    }

    // calculating terminal states
    for (int i = 0; i < (1 << state_machine.states_number); ++i) { // i is the new state number
        for (unsigned j = 0; j < state_machine.terminal_states.size(); ++j) {
            if (setContainsState(i, state_machine.terminal_states[j])) {
                pre_answer.addTerminalState(i);
                break;
            }
        }
    }
    if (print_log) {
        cout << "--------DETERMINING-PROCESS-IS-OVER--------" << endl;
    }
    return removeRedundantStates(pre_answer);
}

StateMachine determinedFull(const StateMachine& state_machine, bool print_log) { // state_machine needs to be determined
    if (print_log) {
        cout << "--------MAKING-FULL-PROCESS-STARTS----------" << endl;
    }

    StateMachine pre_answer;
    pre_answer.initialize(state_machine.states_number + 1, state_machine.alphabet, state_machine.starting_state);

    // copying edges
    for (int i = 0; i < state_machine.states_number; ++i) {
        for (unsigned j = 0; j < state_machine.edges[i].size(); ++j) {
            Edge current_edge = state_machine.edges[i][j];
            pre_answer.addEdge(i, current_edge.to, current_edge.symbol);
        }
    }

    // copying terminal state
    for (unsigned i = 0; i < state_machine.terminal_states.size(); ++i) {
        pre_answer.addTerminalState(state_machine.terminal_states[i]);
    }

    // making state_machine.states_number redundant state
    for (int from = 0; from < pre_answer.states_number; ++from) {
        for (unsigned symbol_number = 0; symbol_number < pre_answer.alphabet.size(); ++symbol_number) {
            char symbol = pre_answer.alphabet[symbol_number];
            int to = pre_answer.go(from, symbol);
            if (to == -1) {
                pre_answer.addEdge(from, pre_answer.states_number - 1, symbol);
            }
        }
    }
    if (print_log) {
        cout << "--------MAKING-FULL-PROCESS-IS-OVER---------" << endl;
    }
    return removeRedundantStates(pre_answer);
}

template<typename T>
ostream& operator << (ostream& os, const vector<T>& v) {
    for (unsigned i = 0; i < v.size(); ++i) {
        if (i != 0) {
            os << "; ";
        }
        os << v[i];
    }
    return os;
}

int get_hash(const vector<int>& v, int multiplier) {
    int answer = 0;
    for (unsigned i = 0; i < v.size(); ++i) {
        answer = multiplier * answer + (v[i] + 1);
    }
    return answer;
}

StateMachine determinedMinimal(const StateMachine& state_machine, bool print_log) { // state_machine needs to be full determined
    if (print_log) {
        cout << "--------MINIMIZATION-PROCESS-STARTS---------" << endl;
    }

    int total_stages_number = state_machine.states_number * 2; // more then needed just in case
    vector<int> set_number (state_machine.states_number, 0);

    for (unsigned i = 0; i < state_machine.terminal_states.size(); ++i) {
        set_number[state_machine.terminal_states[i]] = 1;
    }

    if (print_log) {
        cout << set_number << endl;
    }

    for (int stage_number = 0; stage_number < total_stages_number; ++stage_number) {
        unordered_map<int, bool> hash_met_before;
        unordered_map<int, int> set_number_by_hash;
        vector<vector<int>> vector_to_hash(state_machine.states_number);
        vector<int> new_set_number(state_machine.states_number, 0);
        int total_sets_number = 0;

        for (int from = 0; from < state_machine.states_number; ++from) {
            int hash_multiplier = state_machine.states_number + 3;
            vector_to_hash[from].push_back(set_number[from]);
            for (unsigned symbol_number = 0; symbol_number < state_machine.alphabet.size();
                        ++symbol_number) {
                char symbol = state_machine.alphabet[symbol_number];
                int to = state_machine.go(from, symbol);
                if (to == -1) {
                    throw runtime_error("state_machine is not full in determinedMinimal");
                }
                vector_to_hash[from].push_back(set_number[to]);
            }

            int vector_hash = get_hash(vector_to_hash[from], hash_multiplier);
            if (hash_met_before[vector_hash]) {
                new_set_number[from] = set_number_by_hash[vector_hash];
            } else {
                hash_met_before[vector_hash] = true;
                set_number_by_hash[vector_hash] = new_set_number[from] = total_sets_number;
                ++total_sets_number;
            }
        }

        set_number = new_set_number;
        if (print_log)
            cout << set_number << endl;
    }

    int different_sets_number = 0;
    for (int i = 0; i < state_machine.states_number; ++i) {
        different_sets_number = max(set_number[i] + 1, different_sets_number);
    }

    StateMachine answer;
    answer.initialize(different_sets_number, state_machine.alphabet, 
            set_number[state_machine.starting_state]);

    // adding edges
    for (int from = 0; from < state_machine.states_number; ++from) {
        for (unsigned edge_number = 0; edge_number < state_machine.edges[from].size();
                    ++edge_number) {
            Edge edge = state_machine.edges[from][edge_number];
            answer.addEdge(set_number[from], set_number[edge.to], edge.symbol);
        }
    }

    // adding terminal vertex
    for (unsigned i = 0; i < state_machine.terminal_states.size(); ++i) {
        int old_terminal_state = state_machine.terminal_states[i];
        answer.addTerminalState(set_number[old_terminal_state]);
    }
    if (print_log) {
        cout << "--------MINIMIZATION-PROCESS-IS-OVER--------" << endl;
    }
    return answer;
}

istream& operator >> (istream& is, StateMachine& state_machine) {
    int states_number, edges_number;
    is >> states_number >> edges_number;
    string alphabet;
    is >> alphabet;
    state_machine.initialize(states_number, alphabet);

    // we could add reading start_state, but I personally don't need it when making tasks (start_vertex is always 0)

    for (int i = 0; i < edges_number; ++i) { // read edges, all of them should be 1-letter
        int from, to;
        string s;
        is >> from >> to >> s; // there are problems with reading chars, so I read string
        if (s.size() != 1) {
            throw runtime_error("all the edges should have the length of 1");
        }
        state_machine.addEdge(from, to, s[0]);
    }

    int terminal_states_number;
    is >> terminal_states_number;
    for (int i = 0; i < terminal_states_number; ++i) {
        int terminal_state;
        cin >> terminal_state;
        state_machine.addTerminalState(terminal_state);
    }
    return is;
}

StateMachine getMinimalFullDeterminedStateMachine(const StateMachine& state_machine, 
            bool print_log) {
    StateMachine determined_state_machine = determined(state_machine, print_log);
    StateMachine full_state_machine = determinedFull(determined_state_machine, print_log);
    StateMachine minimal_state_machine = determinedMinimal(full_state_machine, print_log);
    return minimal_state_machine;
}

StateMachine getMinimalFullDeterminedAdditionStateMachine(const StateMachine& state_machine,
            bool print_log) {
    StateMachine determined_state_machine = determined(state_machine, print_log);
    StateMachine full_state_machine = determinedFull(determined_state_machine, print_log);
    full_state_machine.invert(print_log);
    StateMachine minimal_state_machine = determinedMinimal(full_state_machine, print_log);
    return minimal_state_machine;
}
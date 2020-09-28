#include <bits/stdc++.h>

using namespace std;

struct Edge { // we will only work with 1-letter state machines
    int to = -1;
    char c;
};

bool operator == (const Edge& e1, const Edge& e2) {
    return e1.to == e2.to && e1.c == e2.c;
}

ostream& operator << (ostream& os, Edge edge) {
    os << "letter: " << edge.c << "; to: " << edge.to;
    return os;
}

struct StateMachine {
    int n = 0; // vertex number
    int starting_vertex = 0;
    vector<vector<Edge>> edges;
    vector<int> terminal_vertex;
    string alphabet;

    void initialize(int new_n, string new_alphabet, int new_starting_vertex = 0) {
        n = new_n;
        alphabet = new_alphabet;
        starting_vertex = new_starting_vertex;
        edges = vector<vector<Edge>>(new_n);
        terminal_vertex.clear();
    }

    void addEdge(int from, int to, char c) {
        if (find(alphabet.begin(), alphabet.end(), c) == alphabet.end()) {
            throw runtime_error("the alphabet doesn't contain the symbol");
        }

        if (find(edges[from].begin(), edges[from].end(), Edge{to, c}) != edges[from].end()) {
            return;
        }

        edges[from].push_back(Edge{to, c});
    }

    void addTerminalVertex(int new_terminal_vertex) {
        if (find(terminal_vertex.begin(), terminal_vertex.end(), new_terminal_vertex) == terminal_vertex.end()) {
            terminal_vertex.push_back(new_terminal_vertex);
        }
    }

    void invert() {
        vector<int> new_terminal_vertex;
        for (int i = 0; i < n; ++i) {
            if (find(terminal_vertex.begin(), terminal_vertex.end(), i) == terminal_vertex.end()) {
                new_terminal_vertex.push_back(i);
            }
        }
        terminal_vertex = std::move(new_terminal_vertex);
        cout << "Edges have been inverted successfully!" << endl;
    }

    int go(int from, char c) const {
        int answer = -1;
        for (int i = 0; i < edges[from].size(); ++i) {
            Edge current_edge = edges[from][i];
            if (current_edge.c == c && answer == -1) {
                answer = current_edge.to;
            } else if (current_edge.c == c && answer != -1) {
                throw runtime_error("can't call go-method in a non-determined state machine");
            }
        }
        return answer;
    }
};

ostream& operator << (ostream& os, const StateMachine& sm) {
    os << "Vertex number: " << sm.n << "; Starting vertex: " << sm.starting_vertex << '\n';
    os << "Alphabet: " << sm.alphabet << '\n';
    os << "Edges:\n";
    for (int i = 0; i < sm.edges.size(); ++i) {
        os << i << ": ";
        for (int j = 0; j < sm.edges[i].size(); ++j) {
            if (j != 0) {
                os << "; ";
            }
            os << sm.edges[i][j];
        }
        os << '\n';
    }
    os << "Terminal vertex:\n";
    for (int i = 0; i < sm.terminal_vertex.size(); ++i) {
        os << sm.terminal_vertex[i] << ' ';
    }
    os << '\n';
    return os;
}

int getSet(int vertex) {
    return 1 << vertex;
}

bool setContainsVertex(int set_number, int vertex_number) {
    return (set_number >> vertex_number) & 1;
}

void print_set(int set_number, int max_vertex_number) {
    for (int i = 0; i < max_vertex_number; ++i) {
        if (setContainsVertex(set_number, i)) {
            cout << i;
        }
    }
}

void dfs(const StateMachine& sm, vector<bool>& used, int current_vertex) {
    used[current_vertex] = true;
    for (int edge_number = 0; edge_number < sm.edges[current_vertex].size(); ++edge_number) {
        Edge current_edge = sm.edges[current_vertex][edge_number];
        if (!used[current_edge.to]) {
            dfs(sm, used, current_edge.to);
        }
    }
}

StateMachine removeRedundantVertex(const StateMachine& sm) { // sm can be anything
    vector<bool> used(sm.n, false);
    vector<int> old_numbers_by_new_numbers;
    unordered_map<int, int> new_numbers_by_old_numbers;

    dfs(sm, used, sm.starting_vertex);

    for (int i = 0; i < sm.n; ++i) {
        if (used[i]) {
            old_numbers_by_new_numbers.push_back(i);
            new_numbers_by_old_numbers[i] = old_numbers_by_new_numbers.size() - 1;
        }
    }

    StateMachine answer;
    answer.initialize(old_numbers_by_new_numbers.size(), sm.alphabet, new_numbers_by_old_numbers[sm.starting_vertex]);

    // adding all the edges
    for (int old_from = 0; old_from < sm.n; ++old_from) {
        if (!used[old_from]) {
            continue;
        }
        for (int edge_number = 0; edge_number < sm.edges[old_from].size(); ++edge_number) {
            Edge current_edge = sm.edges[old_from][edge_number];
            int new_from = new_numbers_by_old_numbers[old_from];
            int new_to  = new_numbers_by_old_numbers[current_edge.to];
            answer.addEdge(new_from, new_to, current_edge.c);
        }
    }

    // marking terminal vertex
    for (int i = 0; i < sm.terminal_vertex.size(); ++i) {
        int old_terminal_vertex = sm.terminal_vertex[i];
        if (used[old_terminal_vertex]) {
            answer.addTerminalVertex(new_numbers_by_old_numbers[old_terminal_vertex]);
        }
    }
    return answer;
}

StateMachine determined(const StateMachine& sm) { // sm can be anything
    cout << "--------DETERMINING-PROCESS-STARTS---------" << endl;

    StateMachine pre_answer;
    pre_answer.initialize(1 << sm.n, sm.alphabet, getSet(sm.starting_vertex));
    string alphabet = pre_answer.alphabet = sm.alphabet;

    int starting_set = getSet(sm.starting_vertex);
    queue<int> q;
    q.push(starting_set);

    vector<bool> used(1 << sm.n, false);
    used[starting_set] = true;

    // adding all the edges in this cycle
    while(q.size() != 0) {
        int current_set = q.front();
        for (int char_number = 0; char_number < alphabet.size(); ++char_number) {
            print_set(current_set, sm.n);
            char c = alphabet[char_number];
            cout << " " << c << " ";
            int to_set = 0;

            // calculating to_set in the cycle
            for (int vertex_number = 0; vertex_number < sm.n; ++vertex_number) {
                if (!setContainsVertex(current_set, vertex_number)) {
                    continue;
                }
                for (int edge_number = 0; edge_number < sm.edges[vertex_number].size(); ++edge_number) {
                    Edge current_edge = sm.edges[vertex_number][edge_number];
                    if (current_edge.c != c) {
                        continue;
                    }
                    to_set |= getSet(current_edge.to);
                }
            }

            print_set(to_set, sm.n);
            cout << endl;
            pre_answer.addEdge(current_set, to_set, c);

            if (used[to_set]) {
                continue;
            }
            used[to_set] = true;
            q.push(to_set);
        }
        q.pop();
    }

    // calculating terminal vertex
    for (int i = 0; i < (1 << sm.n); ++i) {
        for (int j = 0; j < sm.terminal_vertex.size(); ++j) {
            if (setContainsVertex(i, sm.terminal_vertex[j])) {
                pre_answer.addTerminalVertex(i);
                break;
            }
        }
    }

    cout << "--------DETERMINING-PROCESS-IS-OVER--------" << endl;
    return removeRedundantVertex(pre_answer);
}

StateMachine determinedFull(const StateMachine& sm) { // sm needs to be determined
    cout << "--------MAKING-FULL-PROCESS-STARTS----------" << endl;

    StateMachine pre_answer;
    pre_answer.initialize(sm.n + 1, sm.alphabet, sm.starting_vertex);

    // copying edges
    for (int i = 0; i < sm.n; ++i) {
        for (int j = 0; j < sm.edges[i].size(); ++j) {
            Edge current_edge = sm.edges[i][j];
            pre_answer.addEdge(i, current_edge.to, current_edge.c);
        }
    }

    // copying terminal vertex
    for (int i = 0; i < sm.terminal_vertex.size(); ++i) {
        pre_answer.addTerminalVertex(sm.terminal_vertex[i]);
    }

    // making sm.n redundant vertex
    for (int from = 0; from < pre_answer.n; ++from) {
        for (int char_number = 0; char_number < pre_answer.alphabet.size(); ++char_number) {
            char c = pre_answer.alphabet[char_number];
            int to = pre_answer.go(from, c);
            if (to == -1) {
                pre_answer.addEdge(from, pre_answer.n - 1, c);
            }
        }
    }
    cout << "--------MAKING-FULL-PROCESS-IS-OVER---------" << endl;
    return removeRedundantVertex(pre_answer);
}

int get_hash(const vector<int>& v, int multiplier) {
    int answer = 0;
    for (int i = 0; i < v.size(); ++i) {
        answer = multiplier * answer + (v[i] + 1);
    }
    return answer;
}

template<typename T>
ostream& operator << (ostream& os, const vector<T>& v) {
    for (int i = 0; i < v.size(); ++i) {
        if (i != 0) {
            os << "; ";
        }
        os << v[i];
    }
    return os;
}

StateMachine determinedMinimal(const StateMachine& sm) { // sm needs to be full determined
    cout << "--------MINIMIZATION-PROCESS-STARTS---------" << endl;

    int total_stages_number = sm.n * 2; // more then needed just in case
    vector<int> set_number (sm.n, 0);

    for (int i = 0; i < sm.terminal_vertex.size(); ++i) {
        set_number[sm.terminal_vertex[i]] = 1;
    }

    cout << set_number << endl;

    for (int stage_number = 0; stage_number < total_stages_number; ++stage_number) {
        unordered_map<int, bool> hash_met_before;
        unordered_map<int, int> set_number_by_hash;
        vector<vector<int>> vector_to_hash(sm.n);
        vector<int> new_set_number(sm.n, 0);
        int total_sets_number = 0;

        for (int from = 0; from < sm.n; ++from) {
            int hash_multiplier = sm.n + 3;
            vector_to_hash[from].push_back(set_number[from]);
            for (int char_number = 0; char_number < sm.alphabet.size(); ++char_number) {
                char c = sm.alphabet[char_number];
                int to = sm.go(from, c);
                if (to == -1) {
                    throw runtime_error("sm is not full in determinedMinimal");
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
        cout << set_number << endl;
    }

    int different_sets_number = 0;
    for (int i = 0; i < sm.n; ++i) {
        different_sets_number = max(set_number[i] + 1, different_sets_number);
    }

    StateMachine answer;
    answer.initialize(different_sets_number, sm.alphabet, set_number[sm.starting_vertex]);

    // adding edges
    for (int from = 0; from < sm.n; ++from) {
        for (int edge_number = 0; edge_number < sm.edges[from].size(); ++edge_number) {
            Edge edge = sm.edges[from][edge_number];
            answer.addEdge(set_number[from], set_number[edge.to], edge.c);
        }
    }

    // adding terminal vertex
    for (int i = 0; i < sm.terminal_vertex.size(); ++i) {
        int old_terminal_vertex = sm.terminal_vertex[i];
        answer.addTerminalVertex(set_number[old_terminal_vertex]);
    }

    cout << "--------MINIMIZATION-PROCESS-IS-OVER--------" << endl;
    return answer;
}

istream& operator >> (istream& is, StateMachine& sm) {
    int n, m; // vertex_number, edges_number
    is >> n >> m;
    string alphabet;
    is >> alphabet;
    sm.initialize(n, alphabet);

    // we could add reading start_vertex, but I personally don't need it when making tasks (start_vertex is always 0)

    for (int i = 0; i < m; ++i) { // read edges, all of them should be 1-letter
        int from, to;
        string s;
        is >> from >> to >> s; // there are problems with reading chars, so I read string
        if (s.size() != 1) {
            throw runtime_error("all the edges should have the length of 1");
        }
        sm.addEdge(from, to, s[0]);
    }

    int terminal_vertex_number;
    is >> terminal_vertex_number;
    for (int i = 0; i < terminal_vertex_number; ++i) {
        int v;
        cin >> v;
        sm.addTerminalVertex(v);
    }
    return is;
}

StateMachine getMinimalFullDeterminedStateMachine(const StateMachine& sm) {
    StateMachine determined_sm = determined(sm);
    StateMachine full_sm = determinedFull(determined_sm);
    StateMachine minimal_sm = determinedMinimal(full_sm);
    return minimal_sm;
}

StateMachine getMinimalFullDeterminedAdditionStateMachine(const StateMachine& sm) {
    StateMachine determined_sm = determined(sm);
    StateMachine full_sm = determinedFull(determined_sm);
    full_sm.invert();
    StateMachine minimal_sm = determinedMinimal(full_sm);
    return minimal_sm;
}

int main () {
    StateMachine sm;
    cin >> sm;

    // Task1:
    //StateMachine minimal_sm = getMinimalFullDeterminedStateMachine(sm);
    //cout << minimal_sm << endl;

    // Task2:
    StateMachine minimal_addition_sm = getMinimalFullDeterminedAdditionStateMachine(sm);
    cout << minimal_addition_sm << endl;
    return 0;
}

/*
Test (|w|a - |w|b % 3 == 0)

9 18
ab
0 1 a
0 3 b
1 2 a
1 4 b
2 0 a
2 5 b
3 4 a
3 6 b
4 5 a
4 7 b
5 3 a
5 8 b
6 7 a
6 0 b
7 8 a
7 1 b
8 6 a
8 2 b
3
0 4 8

Task 1

7 13
ab
0 0 a
0 0 b
0 1 a
1 2 a
2 2 a
2 2 b
2 3 b
3 5 b
1 4 b
4 5 a
5 5 b
5 6 a
6 6 b
1
6

Task 2

6 10
ab
0 1 a
1 2 a
2 5 b
5 2 a
5 1 a
1 3 b
3 5 a
5 3 b
3 4 b
4 3 a
2
0 5

*/

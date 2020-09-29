#pragma once

#include <chrono>
#include <iostream>
#include <string>

#include "state_machine.h"

using namespace std::chrono;

class LogDuration {
public:
    explicit LogDuration(const std::string& msg = "")
        : message(msg + ": ")
        , start(steady_clock::now())
    {
    }

    ~LogDuration() {
        auto finish = steady_clock::now();
        auto dur = finish - start;
        std::cerr << message
            << duration_cast<milliseconds>(dur).count()
            << " ms" << std::endl;
    }
private:
    std::string message;
    steady_clock::time_point start;
};

#define UNIQ_ID_IMPL(lineno) _a_local_var_##lineno
#define UNIQ_ID(lineno) UNIQ_ID_IMPL(lineno)

#define LOG_DURATION(message) \
    LogDuration UNIQ_ID(__LINE__){message};

template<class T, class U>
void AssertEqual(const T& t, const U& u, const string& hint = {}) {
    if (!(t == u)) {
        ostringstream os;
        os << "Assertion failed: " << t << " != " << u;
        if (!hint.empty()) {
            os << " hint: " << hint;
        }
        throw runtime_error(os.str());
    }
}

inline void Assert(bool b, const string& hint) {
    AssertEqual(b, true, hint);
}

class TestRunner {
public:
    template <class TestFunc>
    void RunTest(TestFunc func, const string& test_name) {
        try {
            func();
            cerr << test_name << " OK" << endl;
        } catch (exception& e) {
            ++fail_count;
            cerr << test_name << " fail: " << e.what() << endl;
        } catch (...) {
            ++fail_count;
            cerr << "Unknown exception caught" << endl;
        }
    }

    ~TestRunner() {
        if (fail_count > 0) {
            cerr << fail_count << " unit tests failed. Terminate" << endl;
            exit(1);
        }
    }

private:
    int fail_count = 0;
};

#define ASSERT_EQUAL(x, y) {            \
    ostringstream os;                     \
    os << #x << " != " << #y << ", "      \
        << __FILE__ << ":" << __LINE__;     \
    AssertEqual(x, y, os.str());          \
}

#define ASSERT(x) {                     \
    ostringstream os;                     \
    os << #x << " is false, "             \
        << __FILE__ << ":" << __LINE__;     \
    Assert(x, os.str());                  \
}

#define RUN_TEST(tr, func) \
    tr.RunTest(func, #func)

void TestRemoveRedundancy() {
    vector<pair<int, Edge>> edges = {
        {0, {1, 'a'}},
        {1, {1, 'b'}},
        {2, {0, 'a'}},
    };
    vector<int> terminal_vertex = {1};

    StateMachine sm(4, "ab", edges, terminal_vertex, 0);
    sm = removeRedundantVertex(sm);

    AssertEqual(sm.n, 2, "not all the redundant vertex were removed");
    AssertEqual(static_cast<int>(sm.terminal_vertex.size()), 1, "exactly one terminal vertex should remain");
}

double randomDouble01() {
    double random_value = rand();
    double max_value = 32768;
    return random_value / max_value;
}

int randomIntBetween(int l, int r) {
    return randomDouble01() * (r - l) + l + 1;
}

StateMachine getRandomStateMachine(int min_size, int max_size, const string& alphabet,
        double edge_probability, double terminal_probability) {
    int n = randomIntBetween(min_size, max_size);

    StateMachine sm;
    sm.initialize(n, alphabet, 0);

    for (int from = 0; from < n; ++from) {
        for (int to = 0; to < n; ++to) {
            for (unsigned char_number = 0; char_number < alphabet.size(); ++char_number) {
                if (randomDouble01() < edge_probability) {
                    sm.addEdge(from, to, alphabet[char_number]);
                }
            }
        }
    }

    for (int i = 0; i < n; ++i) {
        if (randomDouble01() < terminal_probability) {
            sm.addTerminalVertex(i);
        }
    }

    return sm;
}

void TestDetermination() {
    const int iterations_number = 200;
    const int min_size = 3;
    const int max_size = 10;
    const string alphabet = "abc";
    const double edge_probability = 0.1;
    const double terminal_probability = 0.3;

    for (int i = 0; i < iterations_number; ++i) {
        StateMachine sm = getRandomStateMachine(min_size, max_size, alphabet, edge_probability, terminal_probability);
        sm = determined(sm);
        Assert(sm.is_determined(), "state machine is not determined after determination");
    }
}

void TestFullDetermination() {
    const int iterations_number = 200;
    const int min_size = 3;
    const int max_size = 10;
    const string alphabet = "abc";
    const double edge_probability = 0.1;
    const double terminal_probability = 0.3;

    for (int i = 0; i < iterations_number; ++i) {
        StateMachine sm = getRandomStateMachine(min_size, max_size, alphabet, edge_probability, terminal_probability);
        sm = determined(sm);
        Assert(sm.is_determined(), "state machine is not determined after determination");
        sm = determinedFull(sm);
        Assert(sm.is_determined(), "state machine is not determined after making full");
        Assert(sm.is_full(), "state machine is not full after making full");
    }
}

void TestMinimalFullDetermination() {
    vector<pair<int, Edge>> edges = {
        {0, {1, 'a'}},
        {0, {3, 'b'}},
        {1, {2, 'a'}},
        {1, {4, 'b'}},
        {2, {0, 'a'}},
        {2, {5, 'b'}},
        {3, {4, 'a'}},
        {3, {6, 'b'}},
        {4, {5, 'a'}},
        {4, {7, 'b'}},
        {5, {3, 'a'}},
        {5, {8, 'b'}},
        {6, {7, 'a'}},
        {6, {0, 'b'}},
        {7, {8, 'a'}},
        {7, {1, 'b'}},
        {8, {6, 'a'}},
        {8, {2, 'b'}},
    };
    vector<int> terminal_vertex = {0, 4, 8};

    StateMachine sm(9, "ab", edges, terminal_vertex, 0);
    sm = getMinimalFullDeterminedStateMachine(sm);

    AssertEqual(sm.n, 3, "vertex number is not minimal");
    AssertEqual(static_cast<int>(sm.terminal_vertex.size()), 1, "terminal vertex number has to be different");
    Assert(sm.is_determined(), "state machine is not determined");
    Assert(sm.is_full(), "state machine is not full");
}

void RunTests() {
    TestRunner tr;
    tr.RunTest(TestRemoveRedundancy, "TestRemoveRedundancy");
    tr.RunTest(TestMinimalFullDetermination, "TestMinimalFullDetermination");
    tr.RunTest(TestDetermination, "TestDetermination");
    tr.RunTest(TestFullDetermination, "TestFullDetermination");
}

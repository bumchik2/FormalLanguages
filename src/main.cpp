#include "state_machine.h"

#include <iostream>

using std::cin;
using std::cout;
using std::endl;

int main () {
    // checking if to state machines are equal
    //StateMachine state_machine1;
    // >> state_machine1;
    //StateMachine state_machine2;
    //cin >> state_machine2;
    //cout << areEqual(state_machine1, state_machine2) << endl;

    StateMachine state_machine;
    cin >> state_machine;

    // Task1:
    StateMachine minimal_state_machine =
            getMinimalFullDeterminedStateMachine(state_machine, true);
    cout << minimal_state_machine << endl;

    // Task2:
    //StateMachine minimal_addition_state_machine =
    //        getMinimalFullDeterminedAdditionStateMachine(state_machine, true);
    //cout << minimal_addition_state_machine << endl;
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
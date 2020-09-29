﻿# FormalLanguages
Algorithms with state machines by Sudakov Elisey, Б05-923


Алгоритмы работают только с однобуквенными автоматами (для упрощения реализации,
а также по той причине, что убрать эпсилон-переходы или многобуквенные переходы,
несложно и вручную)

Чтобы получить минимальный полный детерминированный автомат, необходимо передать
автомат в функцию getMinimalFullDeterminedStateMachine.

Чтобы получить минимальный полный детерминированный автомат, необходимо передать
автомат в функцию getMinimalFullDeterminedAdditionStateMachine.

Дополнительным параметром в функцию можно передавать bool print_log - в зависимости
от того, нужен ли вывод пошагового решения в стандартный поток вывода.


Пример использования программы следующий:

    int main() {
      StateMachine sm;
      cin >> sm;
      StateMachine minimal_addition_sm = getMinimalFullDeterminedAdditionStateMachine(sm, true);
      cout << minimal_addition_sm << endl;
      return 0;
    }


на вход программы sm нужно подавать в формате:

    #вершин #ребер
    алфавит (строка)
    ребра в формате from to char
    #терминальных вершин
    номера терминальных вершин


Пример входных данных:

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


Примечание: для упрощения моей собственной работы с программой и уменьшения 
размера входных данных, я работаю в предположении, что вершины нумеруются
от 0 до #вершин-1, причём, в случае считывания данных у пользователя нулевая
вершина - стартовая (и пользователю необязательно это указывать отдельно).

// AI_lab_minmax.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <algorithm>
#include <random>


using namespace std;
using namespace std::chrono;

vector<int> moves;

int boardSize = 6;
char playerChar;
char botChar;

int countPoints(const string& state)
{
    return count(state.cbegin(), state.cend(), botChar) - count(state.cbegin(), state.cend(), playerChar);
}


struct Node
{
    string state;
    int8_t value;
	Node(string newState, int newValue) : state(newState), value(newValue) {}
	Node(string newState) : state(newState), value(countPoints(newState)) {}

};
bool compareNodes(const Node& a, const Node& b) {
    return a.value < b.value; // Сортируем по возрастанию
}

void sortChilds(std::vector<Node>& childs) {
    sort(childs.begin(), childs.end(), compareNodes);
}

bool isValid(int x, int y) {
    return x >= 0 && x < 6 && y >= 0 && y < 6;
}

string infection(const string& state, int index, bool isMaximizing)
{
	string newState = state;
	
	int x = index % boardSize;
	int y = index / boardSize;
	vector<pair<int, int>> moves = { {-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {1, 1}, {-1, 1}, {1, -1} };
	for (pair<int, int>& move : moves)
	{
		int newX = x + move.first;
		int newY = y + move.second;
		int newIndex = newY * boardSize + newX;
        if (isMaximizing)
        {
            if (isValid(newX, newY) && state[newIndex] == playerChar)
            {
                newState[newIndex] = botChar;
            }
		}
		else
		{
			if (isValid(newX, newY) && state[newIndex] == botChar)
			{
				newState[newIndex] = playerChar;
			}
        }
	}
    return newState;
}
	

vector<Node> getChilds(int index, Node& parent, bool isMaximizing) {
    int x = index % 6; // столбец
    int y = index / 6; // строка
    vector<Node> childs;
    // Возможные направления (для клонирования или переноса)
    vector<pair<int, int>> cloneMoves = { {-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {1, 1}, {-1, 1}, {1, -1} };
    vector<pair<int, int>> jumpMoves = { {-2, 0}, {2, 0}, {0, -2}, {0, 2}, {-2, -2}, {2, 2}, {-2, 2}, {2, -2} };

    // Проверка возможных перемещений (клонирование)
    for (pair<int, int>& move : cloneMoves) {
        int newX = x + move.first;
        int newY = y + move.second;
        int newIndex = newY * boardSize + newX;
        if (isMaximizing)
        {
            if (isValid(newX, newY) && parent.state[newIndex] == '.')
            {
                string newState = parent.state;
                newState[newIndex] = botChar;
                newState = infection(newState, newIndex, isMaximizing);
                childs.push_back(Node(newState, countPoints(newState)));
            }

        }
        else
        {
            if (isValid(newX, newY) && parent.state[newIndex] == '.')
            {
                string newState = parent.state;
                newState[newIndex] = 'X';
                newState = infection(newState, newIndex, isMaximizing);
                childs.push_back(Node(newState, countPoints(newState)));
            }
        }
    }

    // Проверка возможных перемещений (перенос)
    for (pair<int, int>& move : jumpMoves) {
        int newX = x + move.first;
        int newY = y + move.second;
        int newIndex = newY * boardSize + newX;
        int oldIndex = y * boardSize + x;
        if (isMaximizing)
        {
            if (isValid(newX, newY) && parent.state[newIndex] == '.')
            {
                string newState = parent.state;
                newState[newIndex] = botChar;
                newState[oldIndex] = '.';
                newState = infection(newState, newIndex, isMaximizing);
                childs.push_back(Node(newState, countPoints(newState)));
            }
        }
        else {
            if (isValid(newX, newY) && parent.state[newIndex] == '.')
            {
                string newState = parent.state;
                newState[newIndex] = playerChar;
                newState[oldIndex] = '.';
                newState = infection(newState, newIndex, isMaximizing);
                childs.push_back(Node(newState, countPoints(newState)));
            }
        }
    }
    sortChilds(childs);
    return childs;
}




void printBoard(const string& state)
{
    int count = 1;
    cout << "  a b c d e f" << endl;
    for (size_t i = 0; i < state.size(); i++)
    {
        if (!(i % boardSize))
        {
            cout << count++ << " ";

        }
        cout << state[i] << " ";
        if ((i + 1) % boardSize == 0)
        {
            cout << endl;
        }
    }
}

ostream& operator<<(ostream& os, const Node& node) {
    os << "State: " << node.state << ", Value: " << node.value;
    return os;
}

Node minimax(int depth, Node& node, bool isMaximizing, int alpha, int beta, unordered_map<string, pair<string, int>>& checked) {
    if (depth == 0) {
        return node;
    }
    Node bestNode = node;
    if (isMaximizing) {
        int maxEval = INT8_MIN;
        for (size_t i = 0; i < node.state.size(); i++) {
            if (node.state[i] == botChar) {
                vector<Node> childs = getChilds(i, node, isMaximizing);
                for (Node& child : childs) {
                    if (checked.find(child.state) == checked.end()) {
                        checked[child.state] = { child.state , depth };
                        Node evalNode = minimax(depth - 1, child, false, alpha, beta, checked);
                        checked[child.state] = { evalNode.state , depth - 1 };
                        int eval = evalNode.value;
                        if (eval >= maxEval) {

                            maxEval = eval;
                            bestNode = child;

                        }
                        alpha = max(alpha, eval);
                        if (beta <= alpha)
                            break;
                    }
                    else if (depth > checked[child.state].second ) {
                        Node evalNode = minimax(depth - 1, child, false, alpha, beta, checked);
                        checked[child.state] = { evalNode.state , depth - 1 };
                        int eval = evalNode.value;
                        if (eval >= maxEval) {

                            maxEval = eval;
                            bestNode = child;

                        }
                        alpha = max(alpha, eval);
                        if (beta <= alpha)
                            break;
                    }
                    else
                    {
                        int eval = countPoints(checked[child.state].first);
                        if (eval >= maxEval) {

                            maxEval = eval;
                            bestNode = child;

                        }
                        alpha = max(alpha, eval);
                        if (beta <= alpha)
                            break;
                    }
                }
            }
            
        }
    }
    else {
        int minEval = INT8_MAX;
        for (size_t i = 0; i < node.state.size(); i++) {
            if (node.state[i] == 'X') {
                vector<Node> childs = getChilds(i, node, isMaximizing);
                for (Node& child : childs) {
                    if (checked.find(child.state) == checked.end()) {
                        checked[child.state] = { child.state , depth };
                        Node evalNode = minimax(depth - 1, child, true, alpha, beta, checked);
                        checked[child.state] = { evalNode.state , depth - 1 };
                        int eval = evalNode.value;
                        if (eval <= minEval) {
;
                            minEval = eval;
                            bestNode = child;

                        }
                        beta = min(beta, eval);
                        if (beta <= alpha)
                            break;
                    }
                    else if (depth > checked[child.state].second) {
                        checked[child.state] = { child.state , depth };
                        Node evalNode = minimax(depth - 1, child, true, alpha, beta, checked);
                        checked[child.state] = { evalNode.state , depth - 1 };
                        int eval = evalNode.value;
                        if (eval <= minEval) {
                            
                            minEval = eval;
                            bestNode = child;

                        }
                        beta = min(beta, eval);
                        if (beta <= alpha)
                            break;
                    }
                    else
                    {
                        int eval = countPoints(checked[child.state].first);
                        if (eval <= minEval) {

                            minEval = eval;
                            bestNode = child;

                        }
                        beta = min(beta, eval);
                        if (beta <= alpha)
                            break;
                    }
                }
            }
        }
    }
    return bestNode;
    // Обработка случая, когда ни один узел не был найден
    throw runtime_error("No valid move found");  // Обработка ошибки
}
bool canMakeMove(const string& state, bool isMaximizing) {
    for (size_t i = 0; i < state.size(); i++) {
        if ((isMaximizing && state[i] == botChar) || (!isMaximizing && state[i] == playerChar)) {
            Node checkNode = Node(state, countPoints(state));
            vector<Node> childs = getChilds(i, checkNode, isMaximizing);
            if (!childs.empty()) return true;  // Если есть хотя бы один допустимый ход
        }
    }
    return false;  // Нет доступных ходов
}

int findMove(const string& oldState, const string& newState, char botChar, pair<int, int>& oldPos, pair<int, int>& newPos) {
    bool foundOldPos = false;
    bool foundNewPos = false;
    vector<pair<int, int>> cloneMoves = { {-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {1, 1}, {-1, 1}, {1, -1} };
    if (count(oldState.cbegin(), oldState.cend(), '.') == count(newState.cbegin(), newState.cend(), '.')) {
        for (int i = 0; i < oldState.size(); ++i) {
            if (oldState[i] == botChar && newState[i] == '.') {
                // Нашли старую позицию бота (если он делал перемещение)
                oldPos = { i % boardSize, i / boardSize };
                foundOldPos = true;
            }
            if (oldState[i] == '.' && newState[i] == botChar) {
                // Нашли новую позицию бота
                newPos = { i % boardSize, i / boardSize };
                foundNewPos = true;
            }
        }
    }
    else {
        for (int i = 0; i < oldState.size(); ++i) {
            if (oldState[i] == '.' && newState[i] == botChar) {
                // Нашли новую позицию бота (если он делал клонирование)
                newPos = { i % boardSize, i / boardSize };
                foundNewPos = true;
                for (pair<int, int>& move : cloneMoves) {
                    int newX = newPos.first + move.first;
                    int newY = newPos.second + move.second;
                    int newIndex = newY * boardSize + newX;
                    if (isValid(newX, newY) && oldState[newIndex] == botChar)
                    {
                        oldPos = { newIndex % boardSize, newIndex / boardSize };
                        foundOldPos = true;
                        return 1;
                    }

                    
                }

            }

        }

    }

    // Если обе позиции найдены, проверяем, действительно ли это перемещение или клонирование
    if (foundOldPos && foundNewPos) {
        // Если старая позиция и новая одна и та же, значит, был только клон
        if (oldPos == newPos) {
            // Для клонирования, не изменяем oldPos
            return 1; // Клонирование
        }
        else {
            return 0; // Перемещение
        }
    }

    // Если не нашли старую позицию, возможно, это клонирование
    if (!foundOldPos && foundNewPos) {
        oldPos = newPos;  // В случае клонирования старая позиция остается той же
        return 2; // Клонирование
    }

    return -1; // Ошибка
}

//int main() {
//    setlocale(LC_ALL, "Russian");
//    string startState = "X....O........................O....X";  // Начальное состояние поля
//    Node root(startState, countPoints(startState));
//    unordered_set<string> checked;
//    printBoard(root.state);  // Печать начальной доски
//
//    bool isMaximizing = false;  // Определяем, кто ходит. false — пользователь, true — компьютер.
//    int depth = 8;  // Глубина поиска алгоритма минимакс
//
//    while (true) {
//        try {
//            if (isMaximizing) {
//                // Ход компьютера с использованием минимакса
//                cout << "Компьютер делает ход..." << endl;
//                Node bestMove = minimax(depth, root, isMaximizing, -1000, 1000, checked);
//                cout << "Current size: " << checked.size() << endl;
//                root.state = bestMove.state;
//                printBoard(root.state);
//                isMaximizing = false;  // Передаем ход пользователю
//                checked.clear();
//            }
//            else {
//                // Ход игрока
//                string oldPosition;
//                string newPosition;
//                cout << "Введите свой ход: (ex. a1 b2)" << endl;
//                cout << "Ход: ";
//                cin >> oldPosition >> newPosition;
//
//                int x = oldPosition[0] - 'a';  // Столбец от 'a'
//                int y = oldPosition[1] - '1';  // Строка от '1'
//                int oldIndex = y * boardSize + x;
//
//                int newX = newPosition[0] - 'a';  // Столбец новой позиции
//                int newY = newPosition[1] - '1';  // Строка новой позиции
//                int newIndex = newY * boardSize + newX;
//
//                // Проверка, может ли игрок сделать ход
//                if (root.state[oldIndex] != 'X' || root.state[newIndex] != '.') {
//                    cout << "Неверный ход! Попробуйте снова." << endl;
//                    continue;  // Запрашиваем ход еще раз, если неверный
//                }
//
//                // Обновляем состояние доски после хода игрока
//                if (abs(x - newX) == 2 || abs(y - newY) == 2)
//                {
//                    root.state[oldIndex] = '.';
//                }
//                root.state[newIndex] = 'X';
//                root.state = infection(root.state, newIndex, false);  // Применяем инфекцию
//                printBoard(root.state);
//
//                isMaximizing = true;  // Передаем ход компьютеру
//            }
//
//            // Проверка окончания игры
//            if (!canMakeMove(root.state, isMaximizing)) {
//                cout << "Игра окончена!" << endl;
//                break;
//            }
//        }
//        catch (const runtime_error& e) {
//            cout << "Ошибка: " << e.what() << endl;
//            break; // Или продолжить, если нужно
//        }
//    }
//
//    return 0;
//}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");

    if (argc < 2) {
        cerr << "Ошибка: Не указан цвет, которым играет бот." << endl;
        return 1;
    }



    // Последний аргумент командной строки - цвет, которым играет бот (0 или 1)
    int botColor = stoi(argv[argc - 1]);  // 0 для 'X', 1 для 'O'
    bool isMaximizing = (botColor == 0);  // true, если бот играет за 'O', false — за 'X'
    botChar = isMaximizing ? 'X' : 'O';
    playerChar = isMaximizing ? 'O' : 'X';

    string startState = "X....O........................O....X";  // Начальное состояние поля
    Node root(startState, countPoints(startState));
    unordered_map<string, pair<string, int>> checked;
    printBoard(root.state);  // Печать начальной доски

    //int depth = stoi(argv[argc - 2]);  // Глубина поиска
    int depth = 5;
    bool memory = false;
    bool random = false;
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-d" && i + 1 < argc) {  // Проверяем, что есть аргумент после флага -d
            depth = stoi(argv[i + 1]);
            i++;
        }
        if (arg == "-m" && i + 1 < argc) {  // Проверяем, что есть аргумент после флага -m
            memory = (bool)stoi(argv[i + 1]);
            i++;
        }
        if (arg == "-r" && i + 1 < argc) {  // Проверяем, что есть аргумент после флага -m
            random = (bool)stoi(argv[i + 1]);
            i++;
        }
    }

    double totalTimeSpent = 0.0;
    vector<string> moveHistory; // Вектор для хранения истории ходов
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(0, 25);


    while (true) {
        try {
            if (isMaximizing) {
                // Ход бота
                cout << "Calculating turn... ";
                auto start = high_resolution_clock::now();
                Node bestMove = root;
                if (random)
                {

                    vector<int> indexes;
                    for (size_t i = 0; i < root.state.size(); i++)
                    {
                        if (root.state[i] == botChar)
                        {
							indexes.push_back(i);
                        }
                    }

					int randomIndex = indexes[dis(gen) % indexes.size()];
					vector<Node> childs = getChilds(randomIndex, root, isMaximizing);
					bestMove = childs[rand() % childs.size()];
                }
                else
                {
                    bestMove = minimax(depth, root, isMaximizing, -1000, 1000, checked);
                }
                auto end = high_resolution_clock::now();
                duration<double> timeTaken = duration_cast<duration<double>>(end - start);
                cout << "Time spent: " << timeTaken.count() << " sec." << "(Memory " << (memory ? "On" : "Off") << ")" << endl;

                totalTimeSpent += timeTaken.count();  // Добавляем время, затраченное на ход
                // Находим, какой именно ход был сделан
                pair<int, int> oldPos, newPos;
                findMove(root.state, bestMove.state, botChar, oldPos, newPos);

                // Преобразуем координаты в формат (например, a1 -> b2)
                string oldPosition = string(1, 'a' + oldPos.first) + to_string(oldPos.second + 1);
                string newPosition = string(1, 'a' + newPos.first) + to_string(newPos.second + 1);

                // Пишем ход бота в stderr
                cerr << oldPosition << " " << newPosition << endl;

                // Обновляем состояние доски
                //moveHistory.push_back(root.state);
                root.state = bestMove.state;
                printBoard(root.state);
                isMaximizing = false;  // Передаем ход сопернику
                if (!memory)
                {
                    checked.clear();
                }
            }
            else {
                startTurn:
                cout << "Enter turn..." << endl;
                // Ход соперника
                string oldPosition;
                string newPosition;
                auto start = high_resolution_clock::now();
                cin >> oldPosition;  // Чтение хода соперника через stdin

                if (oldPosition == "u1") { // Проверка на команду u1
                    if (!moveHistory.empty()) {
                        root.state = moveHistory.back(); // Возвращаем последнее состояние
                        moveHistory.pop_back(); // Убираем последнее состояние из истории
                        cout << "Последний ход отменен." << endl;
                        printBoard(root.state);
                        isMaximizing = true; // Возвращаем ход боту
                    }
                    else {
                        cout << "Нет ходов для отмены." << endl;
                    }
                    goto startTurn; // Пропускаем итерацию цикла
                }
                cin >> newPosition;

                int x = oldPosition[0] - 'a';  // Столбец от 'a'
                int y = oldPosition[1] - '1';  // Строка от '1'
                int oldIndex = y * boardSize + x;

                int newX = newPosition[0] - 'a';  // Столбец новой позиции
                int newY = newPosition[1] - '1';  // Строка новой позиции
                int newIndex = newY * boardSize + newX;

                // Проверка хода соперника
                if (root.state[oldIndex] != playerChar || root.state[newIndex] != '.') {
                    cout << "Wrong turn! Write correct turn." << endl;
                    continue;  // Пропускаем ход, если неверный
                }

                // Обновляем состояние доски после хода соперника
                moveHistory.push_back(root.state);
                if (abs(x - newX) == 2 || abs(y - newY) == 2) {
                    root.state[oldIndex] = '.';
                }
                root.state[newIndex] = playerChar;
                root.state = infection(root.state, newIndex, false);  // Применяем инфекцию
                printBoard(root.state);

                isMaximizing = true;  // Передаем ход боту
            }

            // Проверка окончания игры
            if (!canMakeMove(root.state, isMaximizing)) {
                cout << "Game over!" << endl;

                cout << "Time spent on calculating in total: " << totalTimeSpent << " sec." << "(Memory " << (memory ? "On" : "Off") << ")" << endl;

                if (countPoints(root.state) > 0) {
                    return  0;
                }
                else if (countPoints(root.state) < 0) {
                    return 3; 
                }
                else {
                    return 4;  // Ничья — код 4
                }
            }
        }
        catch (const runtime_error& e) {
            cerr << "Ошибка: " << e.what() << endl;
            return 3;  // Если произошла ошибка, бот проигрывает
        }
    }

    return 0;
}
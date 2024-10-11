// AI_lab_minmax.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <string>
#include <unordered_set>

using namespace std;
vector<int> moves;

int boardSize = 6;

struct Node
{
    string state;
    Node* parent;
    int value;
	Node(string newState, Node* newParent, int newValue) : state(newState), parent(newParent), value(newValue) {}

};

int countPoints(const string& state)
{
    return count(state.cbegin(), state.cend(), 'O') - count(state.cbegin(), state.cend(), 'X');
}

bool isValid(int x, int y) {
    return x >= 0 && x < 6 && y >= 0 && y < 6;
}

string infection(string& state, int index, bool isMaximizing)
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
            if (isValid(newX, newY) && state[newIndex] == 'X')
            {
                newState[newIndex] = 'O';
            }
		}
		else
		{
			if (isValid(newX, newY) && state[newIndex] == 'O')
			{
				newState[newIndex] = 'X';
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
                newState[newIndex] = 'O';
                newState = infection(newState, newIndex, isMaximizing);
                childs.push_back(Node(newState, &parent, countPoints(newState)));
            }

        }
        else
        {
            if (isValid(newX, newY) && parent.state[newIndex] == '.')
            {
                string newState = parent.state;
                newState[newIndex] = 'X';
                newState = infection(newState, newIndex, isMaximizing);
                childs.push_back(Node(newState, &parent, countPoints(newState)));
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
                newState[newIndex] = 'O';
                newState[oldIndex] = '.';
                newState = infection(newState, newIndex, isMaximizing);
                childs.push_back(Node(newState, &parent, countPoints(newState)));
            }
        }
        else {
            if (isValid(newX, newY) && parent.state[newIndex] == '.')
            {
                string newState = parent.state;
                newState[newIndex] = 'X';
                newState[oldIndex] = '.';
                newState = infection(newState, newIndex, isMaximizing);
                childs.push_back(Node(newState, &parent, countPoints(newState)));
            }
        }
    }
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

Node& minimax(int depth, Node& node, bool isMaximizing, int alpha, int beta, unordered_set<string>& checked) {
    if (depth == 0) {
        return node;
    }
    Node* bestNode = nullptr;  // Создаем указатель на лучший ход

    if (isMaximizing) {
        int maxEval = INT8_MIN;
        for (size_t i = 0; i < node.state.size(); i++) {
            if (node.state[i] == 'O') {
                vector<Node> childs = getChilds(i, node, isMaximizing);
                for (Node& child : childs) {
                    if (checked.find(child.state) == checked.end()) {
                        checked.insert(child.state);
                        Node& evalNode = minimax(depth - 1, child, false, alpha, beta, checked);
                        int eval = evalNode.value;
                        if (eval >= maxEval) {

                            maxEval = eval;
                            //bestNode = &child; // Сохраняем ссылку на лучший узел
                            bestNode = new Node(child.state, child.parent, child.value);

                        }
                        alpha = max(alpha, eval);
                        if (beta <= alpha)
                            break;
                    }
                    else
                    {

                        int eval = child.value;
                        if (eval >= maxEval) {

                            maxEval = eval;
                            bestNode = new Node(child.state, child.parent, child.value);

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
                        checked.insert(child.state);
                        Node& evalNode = minimax(depth - 1, child, true, alpha, beta, checked);
                        int eval = evalNode.value;
                        if (eval <= minEval) {
;
                            minEval = eval;
                            //bestNode = &child; // Сохраняем ссылку на лучший узел
                            bestNode = new Node(child.state, child.parent, child.value);

                        }
                        beta = min(beta, eval);
                        if (beta <= alpha)
                            break;
                    }
                    else
                    {
                        int eval = child.value;
                        if (eval <= minEval) {
                            ;
                            minEval = eval;
                            //bestNode = &child; // Сохраняем ссылку на лучший узел
                            bestNode = new Node(child.state, child.parent, child.value);

                        }
                        beta = min(beta, eval);
                        if (beta <= alpha)
                            break;
                    }
                }
            }
        }
    }
    if (bestNode) {
        return *bestNode;  // Возвращаем лучший найденный ход, если он есть
    }

    if (!bestNode) {
        return node; // Возврат текущего узла, если не найден лучший
    }
    // Обработка случая, когда ни один узел не был найден
    throw runtime_error("No valid move found");  // Обработка ошибки
}
bool canMakeMove(const string& state, bool isMaximizing) {
    for (size_t i = 0; i < state.size(); i++) {
        if ((isMaximizing && state[i] == 'O') || (!isMaximizing && state[i] == 'X')) {
            vector<Node> childs = getChilds(i, *(new Node(state, nullptr, countPoints(state))), isMaximizing);
            if (!childs.empty()) return true;  // Если есть хотя бы один допустимый ход
        }
    }
    return false;  // Нет доступных ходов
}



int main() {
    setlocale(LC_ALL, "Russian");
    string startState = "X....O........................O....X";  // Начальное состояние поля
    Node root(startState, nullptr, countPoints(startState));
    unordered_set<string> checked;
    printBoard(root.state);  // Печать начальной доски

    bool isMaximizing = false;  // Определяем, кто ходит. false — пользователь, true — компьютер.
    int depth = 7;  // Глубина поиска алгоритма минимакс

    while (true) {
        try {
            if (isMaximizing) {
                // Ход компьютера с использованием минимакса
                cout << "Компьютер делает ход..." << endl;
                Node bestMove = minimax(depth, root, isMaximizing, -1000, 1000, checked);
                root.state = bestMove.state;
                printBoard(root.state);
                isMaximizing = false;  // Передаем ход пользователю
            }
            else {
                // Ход игрока
                string oldPosition;
                string newPosition;
                cout << "Введите свой ход: (ex. a1 b2)" << endl;
                cout << "Ход: ";
                cin >> oldPosition >> newPosition;

                int x = oldPosition[0] - 'a';  // Столбец от 'a'
                int y = oldPosition[1] - '1';  // Строка от '1'
                int oldIndex = y * boardSize + x;

                int newX = newPosition[0] - 'a';  // Столбец новой позиции
                int newY = newPosition[1] - '1';  // Строка новой позиции
                int newIndex = newY * boardSize + newX;

                // Проверка, может ли игрок сделать ход
                if (root.state[oldIndex] != 'X' || root.state[newIndex] != '.') {
                    cout << "Неверный ход! Попробуйте снова." << endl;
                    continue;  // Запрашиваем ход еще раз, если неверный
                }

                // Обновляем состояние доски после хода игрока
                if (abs(x - newX) == 2 || abs(y - newY) == 2)
                {
                    root.state[oldIndex] = '.';
                }
                root.state[newIndex] = 'X';
                root.state = infection(root.state, newIndex, false);  // Применяем инфекцию
                printBoard(root.state);

                isMaximizing = true;  // Передаем ход компьютеру
            }

            // Проверка окончания игры
            if (!canMakeMove(root.state, isMaximizing)) {
                cout << "Игра окончена!" << endl;
                break;
            }
        }
        catch (const runtime_error& e) {
            cout << "Ошибка: " << e.what() << endl;
            break; // Или продолжить, если нужно
        }
    }

    return 0;
}
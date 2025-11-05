#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <windows.h>
#include <stdexcept>
#include <algorithm>

using namespace std;

const string ALPHABET = "абвгдеёжзийклмнопрстуфхцчшщъыьэюяАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ.,!?;:-(){}[]\"'_-+=0123456789";
string substitution_file = "table.txt";

void shuffle(string& str) {
    srand(static_cast<unsigned int>(time(nullptr)));

    for (size_t i = str.length() - 1; i > 0; i--) {
        size_t j = rand() % (i + 1);
        swap(str[i], str[j]);
    }
}

map<char, char> generateSubstitutionTable() {
    map<char, char> table;
    string shuffled_alphabet = ALPHABET;

    shuffle(shuffled_alphabet);

    for (size_t i = 0; i < ALPHABET.size(); i++) {
        table[ALPHABET[i]] = shuffled_alphabet[i];
    }

    return table;
}

void saveSubstitutionTable(const map<char, char>& table, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Не удалось открыть файл '" + filename + "' для записи таблицы замены");
    }

    for (const auto& p : table) {
        file << p.first << " " << p.second << endl;
    }
}

map<char, char> loadSubstitutionTable(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Файл таблицы замены '" + filename + "' не найден");
    }

    map<char, char> table;
    char key, value;

    while (file >> key >> value) {
        table[key] = value;
    }

    return table;
}

bool substitutionTableExists(const string& filename) {
    ifstream file(filename);
    return file.is_open();
}

string encrypt(const string& text, const map<char, char>& table) {
    string result = text;

    for (char& c : result) {
        auto it = table.find(c);
        if (it != table.end()) {
            c = it->second;
        }
    }

    return result;
}

string decrypt(const string& text, const map<char, char>& table) {
    string result = text;

    map<char, char> reverse_table;
    for (const auto& pair : table) {
        reverse_table[pair.second] = pair.first;
    }

    for (char& c : result) {
        auto it = reverse_table.find(c);
        if (it != reverse_table.end()) {
            c = it->second;
        }
    }

    return result;
}

void viewSubstitutionTable(const map<char, char>& table) {
    if (table.empty()) {
        cout << "Таблица замены пустая!" << endl;
        return;
    }

    cout << "Таблица замены:" << endl;
    for (const auto& pair : table) {
        cout << "'" << pair.first << "' -> '" << pair.second << "'" << endl;
    }
}

vector<string> getAvailableModes() {
    vector<string> modes = { "encrypt", "decrypt", "view", "generate" };
    return modes;
}

map<string, string> parseCommandLineArgs(int argc, char* argv[]) {
    map<string, string> args;

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];

        if (arg == "--mode") {
            if (i + 1 < argc) {
                args["mode"] = argv[++i];
            }
            else {
                throw runtime_error("Для параметра --mode не указано значение");
            }
        }
        else if (arg == "--text") {
            if (i + 1 < argc) {
                args["text"] = argv[++i];
            }
            else {
                throw runtime_error("Для параметра --text не указано значение");
            }
        }
        else if (arg == "--view") {
            args["view"] = "true";
        }
        else if (arg == "--table") {
            if (i + 1 < argc) {
                args["table"] = argv[++i];
            }
            else {
                throw runtime_error("Для параметра --table не указано имя файла");
            }
        }
        else if (arg == "--random") {
            args["random"] = "true";
        }
        else if (arg == "--help") {
            args["help"] = "true";
        }
        else if (arg == "--modes") {
            args["modes"] = "true";
        }
        else {
            throw runtime_error("Неизвестный параметр: " + arg);
        }
    }

    return args;
}

void printHelp(const char* programName) {
    cout << "Программа для шифрования/дешифрования текста шифром замены" << endl;
    cout << "Использование: " << programName << " [ПАРАМЕТРЫ]" << endl;
    cout << endl;
    cout << "Параметры:" << endl;
    cout << "  --mode <режим>     Режим работы: 'encrypt' или 'decrypt'" << endl;
    cout << "  --text <текст>     Текст для обработки" << endl;
    cout << "  --table <файл>     Файл таблицы замены" << endl;
    cout << "  --random           Сгенерировать случайную таблицу" << endl;
    cout << "  --view             Показать таблицу замены" << endl;
    cout << "  --modes            Показать доступные режимы" << endl;
    cout << "  --help             Показать справку" << endl;
}

void interactiveMode() {
    map<char, char> substitution_table;
    string filename;

    cout << "-=-=-=-=-=-= Шифр замены =-=-=-=-=-=" << endl;

    cout << "Введите название файла для таблицы замены (или нажмите Enter для '" << substitution_file << "'): ";
    string input_filename;
    getline(cin, input_filename);

    filename = substitution_file;

    cout << endl << "Выберите источник таблицы замены:" << endl;
    cout << "1 - Загрузить из файла" << endl;
    cout << "2 - Сгенерировать случайную таблицу" << endl;
    cout << "3 - Показать доступные режимы" << endl;
    cout << "Ваш выбор: ";

    int choice;
    cin >> choice;
    cin.ignore();

    try {
        if (choice == 1) {
            substitution_table = loadSubstitutionTable(filename);
            cout << "Таблица замены загружена из файла '" << filename << "'." << endl;
        }
        else if (choice == 2) {
            substitution_table = generateSubstitutionTable();
            saveSubstitutionTable(substitution_table, filename);
            cout << "Случайная таблица создана и сохранена в '" << filename << "'." << endl;
        }
        else if (choice == 3) {
            vector<string> modes = getAvailableModes();
            cout << "Доступные режимы работы:" << endl;
            for (const auto& mode : modes) {
                cout << "- " << mode << endl;
            }
            return;
        }
        else {
            cout << "Неверный выбор." << endl;
            return;
        }

        cout << endl << "Выберите режим работы:" << endl;
        cout << "1 - Шифрование" << endl;
        cout << "2 - Дешифрование" << endl;
        cout << "3 - Просмотр таблицы замены" << endl;
        cout << "Ваш выбор: ";

        int mode_choice;
        cin >> mode_choice;
        cin.ignore();

        if (mode_choice == 1 || mode_choice == 2) {
            cout << endl << "Введите текст для обработки:" << endl;
            string text;
            getline(cin, text);

            if (text.empty()) {
                cout << "Текст не может быть пустым." << endl;
                return;
            }

            string result;
            if (mode_choice == 1) {
                result = encrypt(text, substitution_table);
                cout << endl << "Зашифрованный текст: " << result << endl;
            }
            else {
                result = decrypt(text, substitution_table);
                cout << endl << "Расшифрованный текст: " << result << endl;
            }
        }
        else if (mode_choice == 3) {
            viewSubstitutionTable(substitution_table);
        }
        else {
            cout << "Неверный выбор режима." << endl;
        }

    }
    catch (const exception& e) {
        cout << "Ошибка: " << e.what() << endl;
    }
}

int main(int argc, char* argv[]) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    if (argc == 1) {
        interactiveMode();
        return 0;
    }

    try {
        auto args = parseCommandLineArgs(argc, argv);

        if (args.count("help")) {
            printHelp(argv[0]);
            return 0;
        }

        if (args.count("modes")) {
            vector<string> modes = getAvailableModes();
            cout << "Доступные режимы работы:" << endl;
            for (const auto& mode : modes) {
                cout << "- " << mode << endl;
            }
            return 0;
        }

        string filename = args.count("table") ? args["table"] : substitution_file;
        map<char, char> substitution_table;

        if (args.count("random")) {
            substitution_table = generateSubstitutionTable();
            saveSubstitutionTable(substitution_table, filename);
            cout << "Создана новая случайная таблица в '" << filename << "'" << endl;
        }
        else {
            substitution_table = loadSubstitutionTable(filename);
            cout << "Таблица замены загружена из '" << filename << "'" << endl;
        }

        if (args.count("view")) {
            viewSubstitutionTable(substitution_table);
        }

        if (args.count("mode") && args.count("text")) {
            string result;
            if (args["mode"] == "encrypt") {
                result = encrypt(args["text"], substitution_table);
                cout << "Зашифрованный текст: " << result << endl;
            }
            else if (args["mode"] == "decrypt") {
                result = decrypt(args["text"], substitution_table);
                cout << "Расшифрованный текст: " << result << endl;
            }
            else {
                cerr << "Неверный режим. Используйте 'encrypt' или 'decrypt'" << endl;
                return 1;
            }
        }

    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        printHelp(argv[0]);
        return 1;
    }

    return 0;
}
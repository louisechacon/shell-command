#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <utility>
#include <vector>

using namespace std;

void process_command(string command, vector<string> args);
pair<string, vector<string>> split_command_and_args(string line_command);
void add_history(string line_command);
string get_history(int offset);

vector<string> history_list;
vector<string> path_list = {"/bin/", "/usr/bin/"};

int main() {
    while (true) {
        string line_command;
        string command;
        vector<string> args;

        cout << "$";
        getline(cin, line_command);
        auto result = split_command_and_args(line_command);
        command = result.first;
        args = result.second;

        if (command != "history")
            history_list.push_back(line_command);
        process_command(command, args);
    }
    return 0;
}

string get_history(int offset) {
    return history_list[history_list.size() - 1 - offset];
}

void add_history(string line_command) {
    history_list.push_back(line_command);

    if (history_list.size() > 10) {
        history_list.erase(history_list.begin());
    }
}

// comando argumento1 argumento2
pair<string, vector<string>> split_command_and_args(string line_command) {
    string command;
    vector<string> args;
    string current;
    for (int i = 0; i < line_command.size(); i++) {
        if (line_command[i] == ' ') {
            if (!current.empty()) {
                args.push_back(current);
                current.clear();
            }
        } else if (i == line_command.size() - 1) {
            current.push_back(line_command[i]);
            args.push_back(current);
            current.clear();
        } else {
            current.push_back(line_command[i]);
        }
    }
    command = args.front();
    args.erase(args.begin());
    return {command, args};
}

void process_command(string command, vector<string> args) {
    // Se for comando interno...
    if (command == "history") {
        if (args.empty()) {
            for (int i = history_list.size() - 1; i >= 0; i--) {
                cout << i << " " << get_history(i) << endl;
            }
            return;
        } else if (args.size() == 1 && args[0] == "-c") {
            history_list.clear();
            return;
        } else if (args.size() == 1) {
            try {
                int offset = stoi(args[0]);

                if (offset < 0 || offset >= history_list.size()) {
                    cout << "Erro: offset inválido." << endl;
                    return;
                }

                string line_command = get_history(offset);
                auto result = split_command_and_args(line_command);
                command = result.first;
                args = result.second;
            } catch (...) {
                cout << "Erro: offset inválido." << endl;
            }
        }
    }
    if (command == "exit")
        exit(0);
    if (command == "pwd") {
        char dir[1024];

        if (getcwd(dir, sizeof(dir)) != nullptr) {
            cout << dir << endl;
        }
        return;
    }
    if (command == "cd") {
        if (args.size() != 1) {
            cout << "Uso: cd <diretorio>" << endl;
            return;
        }

        if (chdir(args[0].c_str()) != 0) {
            cout << "Erro: diretório não encontrado." << endl;
        }
        return;
    }

    // Se for comando externo...

    /* Se for caminho relativo, procurar o comando na lista de diretórios
    Se for absoluto verifica se comando existe
    */
    string command_path;

    if (command[0] == '/' || command.substr(0, 2) == "./" ||
        command.substr(0, 3) == "../") {
        command_path = command;
    } else {
        for (string dir : path_list) {
            string possible_path = dir + command;

            if (access(possible_path.c_str(), X_OK) == 0) {
                command_path = possible_path;
                break;
            }
        }
    }
    if (!command_path.empty()) {
        // Executar comando externo
        pid_t pid = fork();
        if (pid < 0) { // Erro
            std::cout << "Erro de execução!" << std::endl;
            return;
        } else if (pid == 0) { // processo filho
            char *argv[args.size() + 2];
            argv[0] = (char *)command.c_str();
            for (int i = 0; i < args.size(); i++) {
                argv[i + 1] = (char *)args[i].c_str();
            }
            argv[args.size() + 1] = nullptr;
            execve(command_path.c_str(), argv, NULL);
        } else { // Processo pai
            // Processo pai espera processo filho terminar.
            waitpid(pid, nullptr, 0);
        }
    } else {
        cout << "Erro: comando não encontrado ou não é executável." << endl;
    }
}

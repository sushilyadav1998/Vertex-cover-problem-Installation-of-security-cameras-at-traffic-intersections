#include <vector>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

using namespace std;
vector<pid_t> kids;

void sigchld_handler(int sig)
{
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        // Check if the terminated process was rgen
        if (pid == kids[0]) {
            // cerr << "Error: rgen process terminated\n";
            // Kill the remaining child processes
            for (int i = 1; i < kids.size(); i++) {
                kill(kids[i], SIGTERM);
            }
            exit(1);
        }
    }
}

int main(int argc, char** argv)
{
    // Set up signal handler for SIGCHLD
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        cerr << "Error: failed to set SIGCHLD handler\n";
        exit(1);
    }
    // Create pipes
    // The first element of the pipe_rgen_to_a1 array will be used for writing to the pipe, 
    // and the second element will be used for reading from the pipe.
    int pipe_rgen_to_a1[2], pipe_a1_to_a2[2]; 

    if (pipe(pipe_rgen_to_a1) == -1) {
        cerr << "Error: creating the pipe_rgen_to_a1" << endl;
        exit(1);
    }
    if (pipe(pipe_a1_to_a2) == -1) {
        cerr << "Error: creating the pipe_a1_to_a2" << endl;
        exit(1);
    }

    // Fork rgen process
    pid_t child_rgen = fork();
    if (child_rgen == -1) {
        cerr << "Error: bad child_rgen forking!" << endl;
        exit(1);
    }
    else if (child_rgen == 0) {
        // Child process: spawn rgen
        dup2(pipe_rgen_to_a1[1], STDOUT_FILENO);
        close(pipe_rgen_to_a1[0]);
        close(pipe_rgen_to_a1[1]);
        close(pipe_a1_to_a2[0]);
        close(pipe_a1_to_a2[1]);
        execvp("./rgen", argv);
        cerr << "Error: Exec of rgen!\n";
        exit(1);
    }

    kids.push_back(child_rgen);

    // Fork a1 process
    pid_t child_a1 = fork();
    if (child_a1 == -1) {
        cerr << "Error: bad child_a1 forking!" << endl;
        exit(1);
    }
    else if (child_a1 == 0) {
        // Child process: spawn a1
        dup2(pipe_rgen_to_a1[0], STDIN_FILENO);
        dup2(pipe_a1_to_a2[1], STDOUT_FILENO);
        close(pipe_rgen_to_a1[0]);
        close(pipe_rgen_to_a1[1]);
        close(pipe_a1_to_a2[0]);
        close(pipe_a1_to_a2[1]);
        char* pyArg[3];
        pyArg[0] = (char*)"/usr/bin/python3";
        pyArg[1] = (char*)"a1ece650.py";
        pyArg[2] = nullptr;
        // char* pyArg[2];
        // pyArg[0] = (char*)"a1ece650.py";
        // pyArg[1] = nullptr;
        execvp("/usr/bin/python3", pyArg);
        // execv("/usr/bin/python3", pyArg);
        cerr << "Error: Exec of a1!\n";
        exit(1);
    }

    kids.push_back(child_a1);

    // Fork a2 process
    pid_t child_a2 = fork();
    if (child_a2 == -1) {
        cerr << "Error: bad child_a2 forking!" << endl;
        exit(1);
    }
    else if (child_a2 == 0) {
        // Child process: spawn a2
        // std::cout << "pipe_a1_to_a2: " << pipe_a1_to_a2[0] <<std::endl;
        close(pipe_rgen_to_a1[0]);
        close(pipe_rgen_to_a1[1]);

        close(pipe_a1_to_a2[1]);
        dup2(pipe_a1_to_a2[0], STDIN_FILENO);
        close(pipe_a1_to_a2[0]);
        // std::cout << "pipe_a1_to_a2[0] = " << pipe_a1_to_a2[0] << std::endl;
        // dup2(STDOUT_FILENO, pipe_a1_to_a2[1]); // Redirect stdout to pipe to send output to parent process
        // dup2(pipe_a1_to_a2[1], STDOUT_FILENO);
        execvp("./a2ece650", static_cast<char* const*>(nullptr));
        cerr << "Error: Exec of a2!\n";
        exit(1);
    }

    kids.push_back(child_a2);
 

    // Close pipe ends in parent process
    close(pipe_rgen_to_a1[0]);
    close(pipe_rgen_to_a1[1]);
    close(pipe_a1_to_a2[0]);
    // close(pipe_a1_to_a2[1]);

    // while (!std::cin.eof()) {
    // std::string line;
    // std::getline(std::cin, line);
    // // if (line.size() > 0)
    // //     std::cout << line << std::endl;
    // }
    // while (!std::cin.eof()) {
    while (!cin.eof()) {
    string line;
    getline(cin, line);
    if (line.empty()) continue; // skip empty lines
    if (line == "EOF") break; // stop loop if end of file is reached
    if (line.size() > 0) {
        // Write the input to the pipe_a1_to_a2
        // write(pipe_a1_to_a2[1], line.c_str(), line.size());
        dup2(pipe_a1_to_a2[1], STDOUT_FILENO);
        // close(pipe_a1_to_a2[1]);
        cout << line << endl;

        // Add a newline character to the end of the input
        // char newline = '\n';
        // write(pipe_a1_to_a2[1], &newline, 1);
    }

    }

    int status;
    for (pid_t k : kids) {
        kill(k, SIGTERM);
        waitpid(k, &status, 0);
    }

    return 0;

}
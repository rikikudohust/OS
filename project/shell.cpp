#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <vector>

#define MAXLINE 1024
#define UNDEFINE 0
#define FG 1
#define BG 2
#define STOP 3


const std::string username = "Hieu";
char input[MAXLINE];
std::vector<int> processid;
int n = 0;
int mark = 0;
std::string cmd, argc;
struct process_t {
	pid_t pid;
	int jid;
	int status;
	char cmdline[MAXLINE];
};

std::vector<struct process_t> processes;

void takeInput();
void help();
void get_current_dir();
void ls();
void child();
void list();
void killchild(int id);
void kill_all();
void exit_process();

void takeInput() {
    std::cout << " ";
    std::cin.getline(input,MAXLINE);
    if (input[0] != ' ') {
        int len = strlen(input);
        int i = 0;
        while (input[i] != ' ' && i < len) {
            cmd += input[i];
            i++;
        }
        while (input[i] == ' ' && i < len) {
            ++i;
        }
        
        while(input[i] != ' ' && i < len) {
            argc += input[i];
            ++i;
        }
    }
}

void get_current_dir() {
    char dir[500];
    getcwd(dir,sizeof(dir));
    std::cout << "\n"<<username << ":" << dir;
}
void help() {
    std::cout << "kill -1   ---- stop all running process\n";
    std::cout << "kill 'id' ----stop running process\n";
    std::cout << "ls        ----show all file and folder in current dir\n";
    std::cout << "date      ---- show date and time\n";
    std::cout << "child     ---- launch child program\n";
    std::cout << "calc      ---- lauch calculate program";
    std::cout << "list      ---- list all process\n";
    std::cout << "cd        ---- Change current diractory\n";
    std::cout << "clear     ---- clear screen\n";
}

void child() {
     pid_t pid;
     char *const parmList[] = {"./test", NULL};
     if ((pid = fork()) == -1)
        perror("fork() error");
     else if (pid == 0) {
        execvp("./test", parmList);
        exit(0);
     } else {
         processid.push_back(pid);
         mark = 1;
     }
}

void calc() {
     pid_t pid;
     char *const parmList[] = {".//usr/bin/gnome-calculator", NULL};
     if ((pid = fork()) == -1)
        perror("fork() error");
     else if (pid == 0) {
        execvp("gnome-calculator", parmList);
        exit(0);
     } else {
         wait(NULL);
     }
}

void kill_all() {
    for (size_t i = 0; i < processid.size(); ++i) {
        killchild(processid[i]);
    }
	processid.clear();
	n = 0;
}
void killchild(int id) {
    for (size_t i = 0; i < processid.size(); ++i) {
        if (id == processid[i]) {
            kill(id, SIGKILL);
            processid.erase(processid.begin() + i);
			n = processid.size();
        }
    }   
}

void list() {
    for (size_t i = 0;  i < processid.size(); ++i) {
        std::cout << processid[i] << std::endl;
    }
}

void sigint(int a){   
    printf("Killing foreground process ... \n");
    for (size_t i = 0; i < processid.size(); ++i) {
        killchild(processid[i]);
    }
    processid.clear();
    n = 0;
}
int main() {
    while(1) {
        cmd = "";
        argc = "";
        get_current_dir();
        takeInput();
        if (!strcmp(input,"help")) {
            help();
        } else if (!strcmp(cmd.c_str(),"clear")) {
            system("clear");
        } else if (!strcmp(cmd.c_str(), "date")) {
            system("date");
        } else if (!strcmp(cmd.c_str(),"ls")) {
            system("ls");
        } else if (!strcmp(cmd.c_str(),"calc")) {
            calc();
        } else if (!strcmp(cmd.c_str(),"child")) {
            child();
            if (mark == 1) {
                continue;
            }
        } else if (!strcmp(cmd.c_str(),"kill")) {
            if (!strcmp(argc.c_str(),"-a")) {
                kill_all();
            } else {
                int id = atoi(argc.c_str());
                killchild(id);
            }
        } else if (!strcmp(cmd.c_str(),"list")) {
            list();
        } else if (!strcmp(cmd.c_str(),"cd")) {
            chdir(argc.c_str());
        }
		signal(SIGINT, sigint);
    }
}

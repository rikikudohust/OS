#include <iostream>
#include <sched.h>
#include <stdio.h>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <vector>

#define MAXPROCESS 10
#define MAXLINE 1024
#define UNDEFINE 0
#define FG 1
#define BG 2
#define STOP 3

using std::vector;
using std::string;
using std::cout;

//{{{=========Global Variable=================
const string username = "Hieu";
char input[MAXLINE];
char cmd[MAXLINE];
char argc[MAXLINE];

int n = 0;               // Amount of process
int bg = 1;              // variable to check background mode
int next_prc = 1;       // the next id process in list
char** environ;
struct pr_t {
	pid_t pid;
	int jid;
	int status;
	char cmdline[MAXLINE];
};
struct pr_t processes[MAXPROCESS];     // list of process

//}}}

void takeInput();
void help();
void get_current_dir();
void ls();
void list();
void killchild(int id);
void kill_all();
void childSignalHandler(int signum);
int getprocess(pid_t pid);
int add_process(struct pr_t* process, pid_t pid, int status, char* cmdline);

// Xu ly Input
void takeInput() {
    std::cout << " ";
    std::cin.getline(input,MAXLINE);
    if (input[0] != ' ') {
        int len = strlen(input);
        int i = 0;
		int j = 0;
        while (input[i] != ' ' && i < len) {
            cmd[j++] = input[i++];
        }
        while (input[i] == ' ' && i < len) {
            ++i;
        }
		j = 0;
        while(i < len) {
            argc[j++] = input[i++];
        }
    }
}

// Hien thi thu muc hien tai
void get_current_dir() {
    char dir[MAXLINE];
    getcwd(dir,sizeof(dir));
    std::cout << "\n"<<username << ":" << dir;
}

// Show cach su dung
void help() {
    std::cout << "kill -a   ---- stop all running process\n";
    std::cout << "kill 'id' ----stop running process\n";
    std::cout << "ls        ----show all file and folder in current dir\n";
    std::cout << "date      ---- show date and time\n";
    std::cout << "child     ---- launch child program\n";
    std::cout << "calc      ---- lauch calculate program";
    std::cout << "list      ---- list all process\n";
    std::cout << "cd        ---- Change current diractory\n";
    std::cout << "clear     ---- clear screen\n";
}

// Hien thi tat ca cac tien trinh con
void list() {
	for (int i = 0; i < MAXPROCESS; ++i) {
		if (processes[i].pid != 0) {
			printf("[%d] %d %s\n", processes[i].jid, processes[i].pid, processes[i].cmdline);
		}
	}
}

void sigint(int a){   
	return;
}

struct pr_t* getprocess(struct pr_t* process, pid_t pid) {
	for (int i = 0; i < MAXPROCESS; i++) {
		if (process[i].pid == pid) {
			return &process[i];
		}
	}
	return nullptr;
}

void execs(char* agrc) {
	char* argv[2] = {agrc,NULL};
	struct pr_t *processid;        //Creating an object of the structure
    sigset_t set;               //Declaring a set 
    sigemptyset(&set);          //Initializing it as empty
    sigaddset(&set,SIGCHLD);    //Adding the SIGCHLD signal to the set 	

	pid_t pid;
	
	sigprocmask(SIG_BLOCK,&set, NULL);
	pid = fork();

	if (pid == 0) {
		sigprocmask(SIG_UNBLOCK,&set, NULL);  
        setpgid(0,0);                          
        if(execve(argv[0],argv, environ)<0) {   
           printf("%s,Command not found", argv[0]);
           exit(0);
        }
	}
	if (!bg) {
		waitpid(pid, NULL, 0);
		return;
	} else {
		add_process(processes, pid, BG, agrc);
		printf("[%d] %d %s\n",next_prc - 1 , pid, agrc);
		sigprocmask(SIG_UNBLOCK, &set, NULL);

	}
}

void reset(struct pr_t* process) {
	process->pid = 0;
	process->status = UNDEFINE;
	process->jid = 0;
	process->cmdline[0] = '\0';
}

int killprocess(pid_t pid) {
	for (int i = 0; i < MAXPROCESS; ++i) {
		if (processes[i].pid == pid ) {
			kill(pid, SIGINT);
			waitpid(pid,NULL, 0);
			reset(&processes[i]);
			return 1;
		}
	}
	return 0;
}
void kill_all() {
	for (int i = 0; i < MAXPROCESS; ++i) {
		if (processes[i].pid > 0) {
			kill(processes[i].pid, SIGINT);
			waitpid(processes[i].pid,NULL,0);
			reset(&processes[i]);
		}
	}
}

int add_process(struct pr_t* process, pid_t pid, int status, char* cmdline) {
	if (pid < 1) {
		return 0;
	}
	for (int i = 0; i < MAXPROCESS; ++i) {
		if (process[i].pid == 0) {
			process[i].pid = pid;
			process[i].jid = next_prc++;
			process[i].status = status;
			if (next_prc > MAXPROCESS) {
				next_prc = 1;
			}
			strcpy(process[i].cmdline, cmdline);
			return 1;
		}
	}
	return 0;
}
void cal() {
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
int main() {
	for (int i = 0; i < MAXPROCESS; ++i) {
		reset(&processes[i]);
	}
    while(1) {
		memset(cmd, 0, sizeof(cmd));
		memset(argc, 0, sizeof(argc));
		signal(SIGINT, sigint);
        get_current_dir();
        takeInput();
        if (!strcmp(input,"help")) {
            help();
        } else if (!strcmp(cmd,"clear")) {
            system("clear");
        } else if (!strcmp(cmd, "date")) {
            system("date");
        } else if (!strcmp(cmd,"ls")) {
            system("ls");
		} else if(!strcmp(cmd,"quit")) {
				exit(0);
        } else if (!strcmp(cmd,"kill")) {
            if (!strcmp(argc,"-a")) {
                kill_all();
            } else {
                int id = atoi(argc);
				killprocess(id);
            }
        } else if (!strcmp(cmd,"list")) {
            list();
        } else if (!strcmp(cmd,"cd")) {
            chdir(argc);
		} else if (!strcmp(cmd,"cal")) {
			cal();
		} else if(!strcmp(cmd,"fg")) {
			bg = 0;
			execs(argc);
		} else if (!strcmp(cmd,"bg")) {
			bg = 1;
			execs(argc);
		}
    }
	exit(0);
}

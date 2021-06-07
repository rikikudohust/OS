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
int fg;
typedef void handler_t(int);
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

//{{{=========== Xu ly input ==========================
void takeInput() {
    std::cout << " ";
    std::cin.getline(input,MAXLINE);
}
int parseline(char* cmd, char** argv) {
	int i = 0;
	char* p;
	p = strtok(cmd," ");
	while(p != NULL) {
		argv[i] = p;
		i++;
		p = strtok(NULL," ");
	}
	return i;
}
//}}}
void get_current_dir() {
    char dir[MAXLINE];
    getcwd(dir,sizeof(dir));
    std::cout << "\n"<<username << ":" << dir;
}

//{{{========== Huong dan su dung ==========
void help() {
    std::cout << "kill -a		Stop all running process\n";
    std::cout << "kill 'id'	Stop running process\n";
    std::cout << "ls		Show all file and folder in current dir\n";
    std::cout << "date		Show date and time\n";
    std::cout << "calc		Lauch calculate program\n";
    std::cout << "list		List all process\n";
    std::cout << "cd		Change current diractory\n";
    std::cout << "clear		Clear screen\n";
    std::cout << "fg <filename>   Run program with foreground\n";
    std::cout << "bg <filename>   Run program with background\n";
	std::cout << "quit		Exit shell\n";
}
//}}}
//{{{ Hien thi tat ca cac tien trinh con
void list() {
	for (int i = 0; i < MAXPROCESS; ++i) {
		if (processes[i].pid != 0) {
			if (processes[i].status == BG) {
				printf("%s ","BG");
			} else if (processes[i].status == STOP) {
				printf("%s ","STOP ");
			}
			printf("[%d] %d %s\n", processes[i].jid, processes[i].pid, processes[i].cmdline);
		}
	}
}

//}}}
//{{{========== Tao tien trinh con va xu ly ==========
struct pr_t* getprocess(struct pr_t* process, pid_t pid) {
	for (int i = 0; i < MAXPROCESS; i++) {
		if (process[i].pid == pid) {
			return &process[i];
		}
	}
	return nullptr;
}

void execs(char** agrc, int bg) {
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
        if(execvp(agrc[0],agrc)<0) {   
           printf("%s,Command not found", agrc[0]);
           exit(1);
        }
	}
	if (!bg) {
		add_process(processes, pid, FG, agrc[0]);
		waitpid(pid, NULL, 0);
		return;
	} else {
		add_process(processes, pid, BG, agrc[0]);
		printf("[%d] %d %s\n",next_prc - 1 , pid, agrc[0]);
		sigprocmask(SIG_UNBLOCK, &set, NULL);
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
//}}}
//{{{========== killprocess ==========
void reset(struct pr_t* process) {
	process->pid = 0;
	process->status = UNDEFINE;
	process->jid = 0;
	process->cmdline[0] = '\0';
}
int killprocess(pid_t pid) {
	for (int i = 0; i < MAXPROCESS; ++i) {
		if (processes[i].pid == pid ) {
			if (processes[i].status == STOP) {
				kill(processes[i].pid,SIGCONT);
			}
			kill(-pid, SIGINT);
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
			if (processes[i].status == STOP) {
				kill(processes[i].pid,SIGCONT);
			}
			kill(-processes[i].pid, SIGINT);
			waitpid(processes[i].pid,NULL,0);
			reset(&processes[i]);
		}
	}
}
//}}}
//{{{========== Calculator foreground ==========
void cal() {
	pid_t pid;
     char *const parmList[] = {"gnome-calculator", NULL};
     if ((pid = fork()) == -1)
        perror("fork() error");
     else if (pid == 0) {
        execvp("gnome-calculator", parmList);
        exit(0);
     } else {
         wait(NULL);
     }
}
//}}}
//{{{========== Xu ly tin hieu Ctrl + C ==========
pid_t fgpid(struct pr_t *process) {
    int i;
	for (i = 0; i < MAXPROCESS; i++)
	if (process[i].status == FG)
	    return process[i].pid;
    return 0;
}
void sigint_handler(int sig)
{
    int pid = fgpid(processes);
    if(pid!=0) killprocess(pid);                  //send sigint to the process group
    return;
}

//}}}
//{{{========== Stop and Restart process ==========
int stop(pid_t pid) {
	for (int i = 0; i < MAXPROCESS; ++i) {
		if (processes[i].pid == pid) {
			if (processes[i].status == BG) {
				kill(pid,SIGSTOP);
				processes[i].status = STOP;
			}
			return 1;
		}
	}
	return 0;
}
int restart(pid_t pid) {
	for (int i = 0; i < MAXPROCESS; ++i) {
		if (processes[i].pid == pid) {
			if (processes[i].status == STOP) {
				kill(pid, SIGCONT);
				processes[i].status = BG;
			}
			return 1;
		}
	}
	return 0;
}
//}}}

//{{{==============Xu ly cau lenh========================
void commandProcess(char* cmd) {
	char* agrc[1024];
	int agrv = parseline(cmd,agrc);
	if (agrv == 1) {
			if (!strcmp(input,"help")) {
				help();
			} else if (!strcmp(agrc[0],"clear")) {
				system("clear");
			} else if (!strcmp(agrc[0], "date")) {
				system("date");
			} else if (!strcmp(agrc[0],"ls")) {
				system("ls");
			} else if(!strcmp(agrc[0],"quit")) {
				exit(0);
			} else if(!strcmp(agrc[0],"list")) {
				list();
			} else if (!strcmp(agrc[0],"calc")) {
				cal();
			} else {
				execs(agrc, 0);
			}
		} else if (agrv == 2) {
			if (!strcmp(agrc[0],"cd")) {
				chdir(agrc[1]);
			} else if (!strcmp(agrc[0],"kill")) {
				if (!strcmp(agrc[1],"-a")) {
					kill_all();
				} else {
					int id = atoi(agrc[1]);
					killprocess(id);
				}
			} else if (!strcmp(agrc[1],"&")) {
				execs(agrc,1);
			} else if (!strcmp(agrc[0],"stop")) {
				int id = atoi(agrc[1]);
				stop(id);
			} else if (!strcmp(agrc[0],"restart")) {
				int id = atoi(agrc[1]);
				restart(id);
			}
		} else {
			return;
		}
}
//}}}
//{{{================= Main===================
int main() {
	for (int i = 0; i < MAXPROCESS; ++i) {
		reset(&processes[i]);
	}
	signal(SIGINT,  sigint_handler);
    while(1) {
        get_current_dir();
        takeInput();
		commandProcess(input);
    }
	exit(0);
}
//}}

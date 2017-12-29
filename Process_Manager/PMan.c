#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <readline/readline.h>
#include <readline/history.h>

/*Global defined variables*/
#define MAX_PROC 4096
#define MAX_CMD 4096

char * formatViolationMsg = " ";

int proc_count = 0;
/*Object: struct proc*/
typedef struct
{
	pid_t pid;   //process id
	char *cmd;   //char *cmd - command and arguments
	bool isStop; //bool isStop - whether process is stopped.
} proc;

proc *proc_list[MAX_PROC];

/*check process existence*/
int is_process_existed(long pid)
{
	int i;
	for (i = 0; i < proc_count; i++)
	{
		if((long)proc_list[i]->pid == pid) return 1;
	}
	return 0;
}

void update_process()
{
	int i;
	for (i = 0; i < proc_count; i++)
	{
		int status;
		if (waitpid(proc_list[i]->pid, &status, WNOHANG) > 0)
		{
			//alert the user to the end of this process
			printf("Process with PID %ld terminated\n", (long)proc_list[i]->pid);
			proc_count--;
			proc_list[i] = proc_list[proc_count];
		}
	}
	sleep(1);
}




int main()
{
	char *input = NULL; /* initialize input */
	char *prompt = "PMan: >";
	int pid_cmd; /* get pid from command */
	char *command[MAX_CMD];
	int cmd_count;
	char *s; 
	while (input = readline(prompt))
	{
		cmd_count = 0;
		char first_com[1024];
		s = strtok(input, " ");
		if (s != NULL)	strcpy(first_com, s);
		while (s != NULL)
		{
			command[cmd_count] = s;
			s = strtok(NULL, " ");
			cmd_count++;
		}
		/*Implement bgkill functuion*/
		if (strcmp(command[0], "bgkill") == 0)
		{
			//to check command info, if violated , print msg
			if (cmd_count == 2)
			{
				if(is_process_existed(strtol(command[1], NULL, 0))==1)
				{
					pid_cmd = atoi(command[1]);
					kill(strtol(command[1], NULL, 0), SIGTERM);
					printf("kill the process %d\n", pid_cmd);
				}
				else
				{
					printf("Error: The process you want to kill is not existed in the process_list.\n");
				}
			}
			else
			{
				printf("not follow the format: bgkill pid\n");
				continue;
			}
			update_process();
		}
		/*Implement bg command*/
		else if (strcmp(command[0], "bg") == 0)
		{
			//to check command info, if violated , print msg
			if (cmd_count < 2)
			{
				printf("Not follow the format: bg ./programName\n");
				continue;
			}
			
			//create child process
			int pid = fork();

			//to check command info, if violated , print msg
			if (pid == 0)
			{
				if (execvp(command[1], command) == -1)
				{
					printf("Invaild command input\n");
					continue;
				}
			}
			//fork failed
			else if (pid < 0)
			{
				printf("Failed fork\n");
				continue;
			}
			//collect info of process
			else
			{
				//printf("Process ID || Process Running");
				proc_list[proc_count] = malloc(sizeof(proc));
				proc_list[proc_count]->cmd = command[1];
				proc_list[proc_count]->pid = pid;
				proc_list[proc_count]->isStop = 0;
				proc_count++;
				update_process();
			}
		}
		/*Implement bgstart function*/
		else if (strcmp(command[0], "bgstart") == 0)
		{
			//to check command info, if violated , print msg
			if (cmd_count == 2)
			{
				if(is_process_existed(strtol(command[1], NULL, 0))==1)
				{
					pid_cmd = atoi(command[1]);
					kill(strtol(command[1], NULL, 0), SIGCONT);
					printf("restart the process %d\n", pid_cmd);
				}
				else
				{
					printf("The process that you want to kill is not existed in the process_list.\n");			
				}
			}
			else
			{
				printf("Not follow the format: bgstart pid\n");
				continue;
			}
			update_process();
		}
		/* Implement bgstop command*/
		else if (strcmp(command[0], "bgstop") == 0)
		{
			//to check command info, if violated , print msg
			if (cmd_count == 2)
			{
				if(is_process_existed(strtol(command[1], NULL, 0))==1)
				{
					pid_cmd = atoi(command[1]);
					kill(strtol(command[1], NULL, 0), SIGSTOP);
					printf("Stop the process %d\n", pid_cmd);
				}
				else
				{
					printf("The process you want to kill is not existed in the process_list.\n");			
				}
			}
			else
			{
				printf("Not follow the format: bgstop pid\n");
				continue;
			}
			update_process();
		}

		/*Implement pstat function*/
		else if (strcmp(command[0], "pstat") == 0)
		{
			//to check command info, if violated , print msg
			if (cmd_count != 2)
			{
				printf("not follow format: pstat pid\n");
				continue;
			}

			FILE *fp_stat;
			char path_stat[1024];
			char fileName[1000];
			sprintf(fileName, "/proc/%s", command[1]);
			if (fopen(fileName, "r") == NULL)
			{
				printf("Error: Process %s does not exist\n", command[1]);
				continue;
			}

			sprintf(fileName, "/proc/%s/stat", command[1]);
			fp_stat = fopen(fileName, "r");

			int stat_cnt = 0;

			if (fp_stat)
			{
				while (fgets(path_stat, sizeof(path_stat) - 1, fp_stat) != NULL)
				{
					char *str;
					str = strtok(path_stat, " ");
					while (str != NULL)
					{
						stat_cnt++;
						
						if (stat_cnt == 1)		printf("Display the information of process with PID %s\n--------------------------------------------------\n", str);
						
						else if (stat_cnt == 2)		printf("||comm:                     	      || %s\n--------------------------------------------------\n", str);
						
						else if (stat_cnt == 3)		printf("||state:                    	      || %s\n--------------------------------------------------\n", str);

						else if (stat_cnt == 14)	printf("||utime: 			      || %lf\n--------------------------------------------------\n", (atof(str)) / sysconf(_SC_CLK_TCK));

						else if (stat_cnt == 15)	printf("||stime:			      || %lf\n--------------------------------------------------\n", (atof(str)) / sysconf(_SC_CLK_TCK));
				
						else if (stat_cnt == 24)	printf("||rss: 			  	      || %s\n--------------------------------------------------\n", str);

						str = strtok(NULL, " ");

					}
				}
			}
			else
			{
				perror("proc stat file cannot be opened\n");
				exit(1);
			}
			if (pclose(fp_stat) == -1)
			{
				printf("failed in closing the stream\n");
				continue;
			}
			FILE *fp_status;
			sprintf(fileName, "/proc/%s/status", command[1]);
			fp_status = fopen(fileName, "r");
			if (fp_status)
			{

				char *path_status;
				size_t length = 0;
				char *token;

				while (getline(&path_status, &length, fp_status) != -1)
				{

					token = strtok(path_status, " :");
					if (token != NULL)
					{
						char *str1 = "voluntary_ctxt_switches";
						char *str2 = "nonvoluntary_ctxt_switches";

						if (strcmp(token, str1) == 0)	
							printf("||%s:            ||%s\n--------------------------------------------------\n", str1, strtok(NULL, " :\n"));
						
						if (strcmp(token, str2) == 0)	
							printf("||%s:         ||%s\n", str2, strtok(NULL, " :\n"));
						
					}
				}
			}
			if (pclose(fp_status) == -1)
			{
				printf("failed in closing the stream\n");
				continue;
			}
		}
		else if (strcmp(command[0], "bglist") == 0)
		{
			update_process();
			if (cmd_count != 1)
			{
				printf("not follow the format: bglist\n");
				continue;
			};

			int i;

			for (i = 0; i < proc_count; i++)	printf("Process ID:                %d\nProcess running:          %s\n", proc_list[i]->pid, proc_list[i]->cmd);
			
			printf("total background jobs:        %d\n", proc_count);
		}
		else
		{
			printf("command not found\n USAGE: \n bgkill <pid> \n bgstop <pid> \n bgstart <pid> \n bg ./programName \n bglist \n pstat <pid>\n");
			continue;
		}
		update_process();
	}
	return;
}

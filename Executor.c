#include "custom_headers.h"
void command(int num,char *cmd_line,char **tokens)
{
	if(strcmp(tokens[num-1],"&")==0)
	{
		tokens[num-1]=NULL; // remove &
		launch_process(cmd_line,tokens,1); // launch in background
	}
	else if(strcmp(tokens[0],"fg")==0)
	{
		if(num!=2)
		{
			printf("Improper use of fg\n");
			return;
		}
		put_to_foreground(atoi(tokens[1]));
	}
	else if(strcmp(tokens[0],"cd")==0)
	{
		if(num>2)
		{
			printf("Improper use of cd\n");
			return;	
		}
		change_dir(tokens[1]);
	}
	else
		launch_process(cmd_line,tokens,0); // launch in foreground
}
void launch_process(char *cmd_line,char **tokens, int bg)
{
	//printf("%s\n%s\n%s\n%d\n",cmd_line,tokens[0],tokens[1],bg);
	pid_t pid=fork();
	if(pid<0)
	{
		perror("fork failed\n");
		exit(-1);
	}
	else if(pid==0)
	{
		setpgid(pid,pid); // assign pgid=pid of process
		//Restore default signals in child process
		int inpfd=-1,outfd=-1;
		if(inp_file!=NULL)
		{
			if(assign_in()==-1)
				exit(-1);
		}
		if(out_file!=NULL)
		{
			if(assign_out()==-1)
				exit(-1);
		}
		//if(record==1)
		//{
		//	dup2(rec_fd,STDOUT_FILENO);
		//}
		if(bg==0)
			tcsetpgrp(shell,getpid());	// assign terminal to the process
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		signal(SIGTSTP, SIG_DFL);
		signal(SIGTTIN, SIG_DFL);
		signal(SIGTTOU, SIG_DFL);
		signal(SIGCHLD, SIG_DFL);
		int i;
		//for(i=0; cmd_line[i];i++)
			//printf("\n%s\n",tokens[i]);

		if(execvp(tokens[0],tokens)<0)
		{
			perror("Command execution error\n");
			exit(-1);
		}
		_exit(0);
	}
	//this part executes in parent ........pid=child process id
	if(bg==0)
	{
		tcsetpgrp(shell,pid);
		int status;
		waitpid(pid, &status, 0);// stops parent till pid process changes state
		if(WIFSTOPPED(status)) // if process was stopped by a signal
			fprintf(stderr,"Command stopped\n");
		tcsetpgrp(shell,shell_pgid);//relinquish control of terminal
	}
	else
	{
		// add process to the tracking list
		int x=add_to_background(pid,cmd_line);
		printf("[%d]\t%d\n",x,pid);
	}
}
int add_to_background(pid_t pid, char *cmd_line)
{
	int i;
	for(i=0;i<=back_index;i++)	//search for empty slot
	{
		if(background[i].status==0)
			break;
	}
	if(i>back_index)		// no empty slot
		back_index++;
	background[i].id=pid;
	background[i].name=strdup(cmd_line);
	background[i].status=1;
	return i;
}
void rem_from_background(int x)
{
	background[x].status=0;
}
void put_to_foreground(int index)
{
	if(background[index].status==0)
	{
		printf("Process does not exist\n");
		return;
	}
	pid_t pid,pgid;
	int status;
	pid=background[index].id;
	pgid=getpgid(pid);
	tcsetpgrp(shell,pgid); // assign terminal to process
	killpg(pgid, SIGCONT);// Send signal to the pgid to continue if stopped
	waitpid(pid, &status, 0);// stops parent till pid process changes state
	if(WIFSTOPPED(status)) // if process was stopped by a signal
		fprintf(stderr,"Command stopped\n");
	background[index].status=1;
	tcsetpgrp(shell,shell_pgid);//relinquish control of terminal	
}
void change_dir(char *dir)
{
	if(dir==NULL || strcmp(dir,"~/")==0 || strcmp(dir,"~")==0 )	
	{
		chdir(home_dir);
		strcpy(current_dir,home_dir);
	}
	else if(chdir(dir)==0) // successful
	{
		getcwd(current_dir, BUF_LEN);
	}
	else
		perror("Command execution error\n");

}


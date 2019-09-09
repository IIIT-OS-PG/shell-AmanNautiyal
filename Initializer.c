#include "custom_headers.h"

void prompt()
{
	int i;
	//printf("home=%s\n",home_dir);
	//printf("Cwd=%s\n",current_dir);
	for(i=0;home_dir[i]!='\0';i++)
	{
		if(home_dir[i]!=current_dir[i])
			break;
	}
	if(home_dir[i]=='\0')
	{
		printf(":~");
		for(;current_dir[i]!='\0';i++)
			printf("%c",current_dir[i]);
	}
	else
	{
		printf(":");
		for(i=0;current_dir[i]!='\0';i++)
			printf("%c",current_dir[i]);
	}
	if(getuid()==0)
		printf("#");
	else
		printf("$");
}
void set_up()
{
	int fd=open("myrc",O_RDONLY);
	if(fd==-1)
	{
		printf("Error opening myrc\n");
		exit(-1);
	}
	char buffer[BUF_LEN];
	read(fd,buffer,BUF_LEN);
	char **tokens=malloc((sizeof(char)*BUF_LEN)*BUF_LEN);
	int num=get_tokens(buffer,tokens,"\n");
	int i;
	for(i=5;tokens[0][i];i++)
		home_dir[i-5]=tokens[0][i]; // set home
	home_dir[i]='\0';
	for(i=5;tokens[1][i];i++)
		path[i-5]=tokens[1][i]; // set path
	path[i]='\0';
	setenv("HOME",home_dir,0);
	setenv("PATH",home_dir,0);
	chdir(home_dir);
	strcpy(current_dir,home_dir);
	//record =1;
	//rec_fd=open("Recording.txt",O_WRONLY | O_CREAT | O_APPEND, S_IRWXU);
}
void shell_start()
{
	back_index=-1;
	shell=STDIN_FILENO;
	if(isatty(shell))	//if shell is fd referring to terminal
	{
		//loop till shell is in foreground
		//tcgetpgrp-return process group id of group on terminal associated to fd
		//getpgrp-process group of calling process
		while(tcgetpgrp(shell)!=(shell_pgid=getpgrp()))
			kill(shell_pgid,SIGTTIN);
		signal(SIGINT, SIG_IGN);//keyboard interrupts
		signal(SIGQUIT, SIG_IGN);//quit from keyboard
		signal(SIGTSTP, SIG_IGN);//stop typed at terminal
		signal(SIGTTIN, SIG_IGN);//terminal input for background process
		signal(SIGTTOU, SIG_IGN);//terminal output for background process
		signal(SIGCHLD, SIG_IGN);//child stopped or terminated
		shell_pgid=getpid(); //process id of calling process
		if(setpgid(shell_pgid,shell_pgid)<0)
		{
			perror("Failed to put shell in its own process group\n");
			exit(1);
		}
		tcsetpgrp(shell,shell_pgid);//make process group shell_pgid the foreground process group on terminal associated with shell
		set_up();
	}
}

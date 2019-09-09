#include "custom_headers.h"
void bg_ret(int sig)
{
	int stat,i;
	int rem_pid=waitpid(-1,&stat,WNOHANG);//-1 searches all children,WNOHAND return immediately if no child's status changed
	for(i=0;i<=back_index;i++)
	{
		if(background[i].id==rem_pid)
			break;
	}
	if(background[i].status==1 && i<=back_index)// to check if has not been put at foreground earlier
	{		
		printf("[%d]+\tDone\t\t%s\n",i,background[i].name);	
		rem_from_background(i);
	}
}
int main()
{
	int num;
	shell_start();
	while(1)
	{	
		prompt();
		signal(SIGCHLD,bg_ret);
		signal(SIGINT,SIG_IGN);
		inp_file=NULL;
		out_file=NULL;
		char **tokens=malloc((sizeof(char)*BUF_LEN)*BUF_LEN);
		char *cmd_line=read_cmd_line();	//string entered
		//if(record==1)
		//	write(rec_fd,cmd_line,BUF_LEN);
		if(strcmp(cmd_line,"exit")==0)
			return 0;
		if(strcmp(cmd_line,"\0")==0)
			continue;
		int chk=check(cmd_line);
		num=0;
		if(chk==0)	// no piping, no redirection
		{
			num=get_tokens(cmd_line,tokens," \t\n\'\"");
			command(num,cmd_line,tokens);
		}
		else if(chk==2) // redirection,no piping
		{
			num=get_red_tokens(cmd_line,tokens);
			command(num,cmd_line,tokens);
		}
		else if(chk==2 || chk==3) //piping present
		{
			//for now
		}
		if(num==0)
			continue;
		
		free(cmd_line);
		free(tokens);
	}
	return 0;
}

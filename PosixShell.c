#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#define cmd_delims
char* read_cmd_line()
{
	char *buffer=malloc(sizeof(char)*1024);
	char c;
	int i=0;
	while(1)
	{
		c=getchar(); // read character till newline
		if(c=='\n')
		{
			buffer[i]='\n'; //Replace newline with null
			break;
		}
		buffer[i]=c;
		i++;
	}
	return buffer; // return read string
}
int get_tokens(char *line,char** tokens)
{	
	int i=0;
	char *token=strtok(line," \t\r\n\a");
	for(i=0;i<1024;i++)
		tokens[i]='\0';
	i=0;
	while(token!=NULL)
	{
		tokens[i]=token;
		i++;
		token=strtok(NULL," \t\r\n\a");
	}
	return i;
}
void main()
{
	int i,n;
	while(1)
	{	
		char **tokens=malloc((sizeof(char)*1024)*1024);
		char *cmd_line=read_cmd_line();	//string entered
		get_tokens(cmd_line,tokens);
		execvp(tokens[0],tokens);
	}
}

#include "custom_headers.h"
char* read_cmd_line()
{
	//printf("In readline\n");
	char *buffer=malloc(sizeof(char)*1024);
	char c;
	int i=0;
	while(1)
	{
		c=getchar(); // read character till newline
		if(c=='\n')
		{
			buffer[i]='\0'; //Replace newline with null
			break;
		}
		buffer[i]=c;
		i++;
	}
	return buffer; // return read string
}
int get_tokens(char *cmd_line,char** tokens, char *delim)
{	
	int i=0;
	char *line=strdup(cmd_line);
	char *token=strtok(line,delim);//parse
	for(i=0;i<BUF_LEN;i++)
		tokens[i]='\0';
	i=0;
	while(token!=NULL)
	{
		tokens[i]=token;
		i++;
		token=strtok(NULL,delim);
	}
	return i;	
}
int check(char *cmd_line)
{
	int i,x;
	int pipe=-1,red=-1;
	for(i=0;cmd_line[i];i++)
	{
		if(cmd_line[i]=='|')
			pipe=1;
		else if(cmd_line[i]=='<' || cmd_line[i]=='>')
			red=1;
		if(red==1 && pipe==1)
			break;
	}
	if(red==-1 && pipe==-1)
		return 0;
	else if(red==-1 && pipe==1)
		return 1;
	else if(red==1 && pipe==-1)
		return 2;
	else if(red==1 && pipe==1)
		return 3;
}
int get_red_tokens(char *line,char** tokens)
{
	int i,num=0;
	int ired_index,ored_index;
	ired_index=ored_index=-1;
	inp_file=NULL;
	out_file=NULL;
	app=0;
	for(i=0; line[i];i++)
	{
		if(line[i]=='<')
			ired_index=ired_index==-1?i:ired_index; // index of first <
		else if(line[i]=='>')
		{
			ored_index=ored_index==-1?i:ored_index; // index of first >
			if(i!=0 && line[i-1]=='>')
				app=1;		// >> present
		}
	}
	if(ired_index!=-1 && ored_index!=-1) // 	both present
	{
		num=get_tokens(line,tokens,"<> \t\n\'\"");
		if(ired_index<ored_index)
		{
			if(strcmp(tokens[num-1],"&")==0)
			{
				inp_file=tokens[num-2];
				out_file=tokens[num-3];
				tokens[num-3]="&";
			}
			else
			{
				inp_file=tokens[num-1];
				out_file=tokens[num-2];
			}
			tokens[num-1]="\0";
			tokens[num-2]=NULL;
			num-=2;
			return num;
		}
	}
		if(ired_index!=-1)  // only < present
		{
			num=get_tokens(line,tokens,"<");
			char ** inp_tokens=malloc((sizeof(char)*BUF_LEN)*BUF_LEN);
			int temp=get_tokens(tokens[num-1],inp_tokens," \t\n"); // right side of <
			int bg=0;
			if(temp >1 && strcmp(inp_tokens[temp-1],"&")==0)
				bg=1; // & present at end
			inp_file=inp_tokens[0];
			num=get_tokens(tokens[0],tokens," \t\n\'\""); // left side of <
			if(bg==1)
			{
				tokens[num]="&";
				tokens[num+1]=NULL;
				num++;
			}
			free(inp_tokens);
		}
		if(ored_index!=-1) // only > or >> present
		{
			if(app=1)
				num=get_tokens(line,tokens,">>");
			else
				num=get_tokens(line,tokens,">");
			char ** out_tokens=malloc((sizeof(char)*BUF_LEN)*BUF_LEN);
			int bg=0;
			int temp=get_tokens(tokens[num-1],out_tokens," \t\n"); // right side of > or >>
			if(temp >1 && strcmp(out_tokens[temp-1],"&")==0)
				bg=1; // & present at end
			out_file=out_tokens[0];
			//printf("%s\n",out_file);
			num=get_tokens(tokens[0],tokens," \t\n\'\""); // left side of > or >>
			if(bg==1)
			{
				tokens[num]="&";
				tokens[num +1]=NULL;
				num++;
			}
			//printf("%d\n",num);
			free(out_tokens);
		}
		return num;
}
int assign_in()
{
	int fd = open(inp_file,O_RDONLY, S_IRWXU);
	if(fd==-1)
	{
		fprintf(stderr,"%s could not be read from\n",inp_file);
		close(fd);
		return fd;
	}
	dup2(fd,STDIN_FILENO); // fd and STDIN_FILENO(0) can be used interchangably
	close(fd);
	return fd;
}
int assign_out()
{
	int fd ;
	if(app=0)
		 fd=open(out_file,O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU); // > present
	else
		fd=open(out_file,O_WRONLY | O_CREAT | O_APPEND, S_IRWXU); // >> present
	if(fd==-1)
	{
		fprintf(stderr,"%s could not be written to\n",out_file);
		close(fd);
		return fd;
	}
	dup2(fd,STDOUT_FILENO); // fd and STDOUT_FILENO(1) can be used interchangably
	close(fd);
	return fd;
}

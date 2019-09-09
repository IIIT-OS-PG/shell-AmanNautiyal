//	HEADERS
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include<stdio.h>
#include<string.h>
#include <fcntl.h> 
#include<sys/types.h>
#include <sys/wait.h>
#define BUF_LEN 4096

//	FUNCTIONS

void prompt();
void set_up();
void shell_start();
char* read_cmd_line();
int get_tokens(char *line,char** tokens,char *delim);
void launch_process(char *cmd_line,char **tokens,int bg);
void bg_ret(int sig);
int add_to_background(pid_t pid, char *cmd_line);
void rem_from_background(int x);
void command(int num,char *cmd_line,char **tokens);
void put_to_foreground(int index);
void change_dir(char *dir);
int check(char *cmd_line);
int get_red_tokens(char *line,char** tokens);
int record;
int rec_fd;

//	VARIABLES

char home_dir[BUF_LEN];
char current_dir[BUF_LEN];
char path[BUF_LEN];
int shell;
pid_t shell_pgid;
int app;
struct process
{
	pid_t id,gid;
	int status;
	char *name;
}background[BUF_LEN];
int back_index;
char *inp_file,*out_file;
int assign_in();
int assign_out();

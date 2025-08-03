#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <ctype.h>
#include <stdbool.h>

#define inMaxT 1000//max number of input statement
#define arguMaxT 65  //taken argument
#define szHis 200 //size of history declaration
char *forhistory[szHis];  //here, history=forhistory to execute in the promt exactly
int totalHisCount=0;
bool varEXIT=false;




// Task:1
void sigint_handler(int sig) {
printf("\n");
varEXIT= true;} 






// Task:2
char* whspaceCancellation(char *pp) { //str=pp
while(isspace((unsigned char)*pp)) pp++;
if(*pp=='\0') return pp;
char *end=pp+strlen(pp)-1;
while(end>pp && isspace((unsigned char)*end)) end--;
end[1]='\0';
return pp;}






// Task:3
void hisPlusplus(const char *command1) {
if(totalHisCount<szHis){
forhistory[totalHisCount++]=strdup(command1);  }
}







// Task:4
char** taskparsing1(char *inp11) {
char **toargument=malloc(arguMaxT*sizeof(char*));
int l=0;
char *a=strtok(inp11," ");   
while(a!=NULL && l<arguMaxT-1) {
toargument[l++]=a;
a=strtok(NULL," ");}   
toargument[l]=NULL;
return toargument;}







// Task:5
void ioHandle(char **t) {
int a= 0;
while(t[a]) {
if(strcmp(t[a],"<")==0) {
int ln=open(t[a+1],O_RDONLY);
dup2(ln,STDIN_FILENO);
close(ln);
t[a]=NULL;}       
else if(strcmp(t[a],">")==0) {
int ln=open(t[a+1],O_WRONLY|O_CREAT|O_TRUNC,0644);
dup2(ln,STDOUT_FILENO);
close(ln);
 t[a]=NULL;
 }
else if(strcmp(t[a],">>")==0) {
int ln= open(t[ln+1],O_WRONLY|O_CREAT|O_APPEND,0644);
dup2(ln,STDOUT_FILENO);
close(ln);
t[a]=NULL;}
     a++;}
}





// Task:6
int singleCMDexc(char **arr) {
if(!arr[0]) return 0;
if(strcmp(arr[0],"exit")==0) {
varEXIT=true;
return 0;
}
if(strcmp(arr[0], "history") == 0) {
 for(int ppp=0;ppp<totalHisCount;ppp++) {
 printf("%d: %s\n",ppp+1,forhistory[ppp]);}
  return 0;}
pid_t pid=fork();
    if(pid==0) {
ioHandle(arr);
execvp(arr[0],arr);
perror("execvp failed");
exit(EXIT_FAILURE);
    }
else if(pid>0) {
int stat;
waitpid(pid,&stat, 0);
return WEXITSTATUS(stat);
    }
else {
perror("fork");
 return -1;
}}









// Task:7
void pipLinecmdEXC(char ***c, int nc) {
 int pipes[nc-1][2];
 pid_t pids[nc];

 for(int r=0;r<nc-1;r++) {
 pipe(pipes[r]);
 }
 for(int r=0;r<nc;r++) {
 pids[r]=fork();
 if(pids[r]==0) {
 if(r>0) {
 dup2(pipes[r-1][0],STDIN_FILENO);
 close(pipes[r-1][1]);
 }
if(r<nc-1) {
dup2(pipes[r][1],STDOUT_FILENO);
close(pipes[r][0]);
}
for(int rr=0;rr<nc-1;rr++) {
 if(rr!=r-1)close(pipes[rr][0]);
if(rr!=r)close(pipes[rr][1]);
}
 singleCMDexc(c[r]);
exit(EXIT_FAILURE);
}}
for(int r=0;r<nc-1;r++) {
close(pipes[r][0]);
close(pipes[r][1]);
}
for(int tt=0;tt<nc;tt++) {
 waitpid(pids[tt],NULL,0);
}
}





//Main Function
int main() {
struct sigaction sa;
sa.sa_handler=sigint_handler;
sigemptyset(&sa.sa_mask);
sa.sa_flags=SA_RESTART;
sigaction(SIGINT,&sa,NULL);
char input[inMaxT];
while(!varEXIT){
printf("sh> ");
fflush(stdout);
if(!fgets(input,inMaxT,stdin)) break;
input[strcspn(input,"\r\n")] ='\0';
hisPlusplus(input);





//Task:8
char *cMDLogic[arguMaxT];
int n=0;
char *saveptr;
char *t=strtok_r(input,";&&",&saveptr);       
while(t && n<arguMaxT-1) {
cMDLogic[n++]=whspaceCancellation(t);
t=strtok_r(NULL,";&&",&saveptr);}
int last_stat=0;
for(int p=0;p<n;p++) {
if(strlen(cMDLogic[p])==0) continue;






//Task:9
if(p> 0 && input[strlen(cMDLogic[p-1])]=='&' && last_stat!=0) {
continue;}
           





//Task:10
char *pipe_cmds[arguMaxT];
int nPipes=0;
char *psaveptr;
char *ptoken=strtok_r(cMDLogic[p], "|",&psaveptr);          
while(ptoken && nPipes<arguMaxT-1) {
pipe_cmds[nPipes++] =whspaceCancellation(ptoken);
ptoken=strtok_r(NULL, "|",&psaveptr);}
if(nPipes>1) {
char ***parsed_cmds=malloc(nPipes*sizeof(char**));
for(int j=0;j<nPipes;j++) {
parsed_cmds[j]=taskparsing1(pipe_cmds[j]);
}
pipLinecmdEXC(parsed_cmds,nPipes);
free(parsed_cmds);
}
else {
char **args=taskparsing1(cMDLogic[p]);
last_stat=singleCMDexc(args);
free(args);
 }
}}
   
   
   
    

//Task:11
for(int cl=0;cl<totalHisCount;cl++) {
free(forhistory[cl]);
 }
return 0;
}


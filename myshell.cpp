#include <dirent.h> 
#include <fcntl.h> 
#include <unistd.h>     // getpid(), getcwd()
#include <sys/types.h>  // type definitions, e.g., pid_t
#include <sys/wait.h>   // wait()
#include <signal.h>     // signal name constants and kill()
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>

using namespace std;
#define MAX_COMMAND_SIZE 1024
char local_path[100000];
char newfirstWord[10000];
int debugLog=0;

int find_file(char *dir_path, char *file_name);
void pipeCmdHandling(char **argv);

#define MAXSIZE 100

struct stack
{
    char thisStack[MAXSIZE][10000];
    int topElement;
};
typedef struct stack STACK;
STACK stack;

void pushd(char *dirStr);
char *popd (char *dirStr);
void display(void);

int main()
{
    int executed_via_pipeline = 0;
    stack.topElement = -1;
    /* Run infinite loop to take input from user */
    /* INFINITE LOOP TO EXECUTE COMMANDS IN LOOP */
    for(;;)
    {
        executed_via_pipeline = 0;
        int cmdCnt = 0;
        char temp [10000];
        getcwd(temp, 10000);
        /* Create the prompt for user to input commands */
        /* SHELL prompt */
        cout << "myShell:" << temp << " $ " ;
        char inputCommand[MAX_COMMAND_SIZE];
        cin.getline(inputCommand, MAX_COMMAND_SIZE);

        /* Check if user has pressed an empty command or not */
        if(strlen(inputCommand) > 0)
        {
        }
        else
        {
            /* User has just pressed enter. Go back to the prompt again */
            continue;
        }

        /* CODE TO DECODE THE INPUT COMMAND */
        /* Decode the command inputted by user */
        vector<char*> docodeCommand;
        /* decode inputCommand based on Spaces between various words of input command */
        char* firstWord = strtok(inputCommand, " ");
        char* subsecuentWords = firstWord;
        /* Loop till all the words are not parsed */
        while (subsecuentWords != NULL)
        {
            docodeCommand.push_back(subsecuentWords);
            subsecuentWords = strtok( NULL, " ");
        }
        /* decode inputCommand based on backward slash between various words of input command */
        

        /* Create and fill argv from decoded command. This argv is input 
         * to exec commands */
        char** argv = new char*[docodeCommand.size()+1];

        /* Loop till decodeCommand Size */
        for (cmdCnt = 0; cmdCnt <= docodeCommand.size(); cmdCnt++ )
        {
            if(cmdCnt == docodeCommand.size())
            {
                argv[docodeCommand.size()] = NULL;
            }
            else
            {
                argv[cmdCnt] = docodeCommand[cmdCnt];
            }

        }


        /* HERE WE CHECK IF THE COMMAND IS GIVEN BY / or not:
         * if / then its absolute path
         * else we search the env (environment to find the command */
        char * output = NULL;
        char b[]="/";
        output = strstr (firstWord,b);
        //if '/' is found then treat as absolute path  (TreatAsPath();)
        if(!output) 
        {
            if(debugLog)
            {
                printf("'/' present in command: Need to search in path variable\n");
            }
            char buffer[1000];
            sprintf( buffer, "tmux" );
            int found = 0;
            char *path;
            char *dir;

            /* This is ur getenv command */
            path = getenv("PATH");
            strcpy(local_path, path);
            for ( dir = strtok( local_path, ":" ); dir; dir = strtok( NULL, ":" ) ) 
            {
                /* Here we are trying to find the command in the path environment */
                found = find_file(dir, firstWord);
                if(found == 1)
                {
                    if(debugLog)
                    {
                        printf("Command found in [%s/%s]\n", dir, firstWord);
                        printf("firstWord %s\n", firstWord);
                    }
                    sprintf (newfirstWord, "%s/%s", dir, firstWord);
                    if(debugLog)
                    {
                        printf("newfirstWord %s\n", newfirstWord);
                    }
                    break;
                }
                else
                {
                    if(debugLog)
                    {
                        printf("Command not found in[%s/%s]\n", dir, firstWord);
                    }
                }
            }
            /* Leave this case now - i will come back later */
            if (found == 0)
            {
                /* This is the code to handle pushd popd and dirstack */
                if (!strcmp (firstWord, "pushd"))
                {
                    char cwd[1024];
                    if (getcwd(cwd, sizeof(cwd)) != NULL)
                    {
                    }
                    else
                    {
                        perror("getcwd() error");
                    }
                    if(cmdCnt == 2)
                    {
                        printf("Command pushd requires argument\n");
                    }
                    else
                    {
                        if(debugLog)
                        {
                            printf("Command pushd\n");
                        }
                        pushd(getcwd(cwd, sizeof(cwd)));
                        if(0 == chdir (argv[1]))
                        {
                        }
                        else
                        {
                            printf("chdir FAILED: No such directory\n");
                        }
                    }
                    continue;
                }
                if (!strcmp (firstWord, "popd"))
                {
                    printf("Command popd\n");
                    char tmp[1000];
                    popd(tmp);
                    if(0 == chdir (tmp))
                    {
                    }
                    else
                    {
                        printf("chdir FAILED: No such directory\n");
                    }
                    if(debugLog)
                    {
                        printf("popout cmd %s\n", tmp);
                    }
                    continue;
                }
                if (!strcmp (firstWord, "dirstack"))
                {
                    printf("Command dirstack\n");
                    display();
                    continue;
                }
                if (strcmp (firstWord, "cd"))
                {
                    printf("Command not found in any path\n");
                    continue;
                }
            }
        }
        else //if '/' is not found                                                                 !!!!!!
        {
            sprintf (newfirstWord, "%s", firstWord);
            printf("newfirstWord %s\n", newfirstWord);
        }

        if ( strcmp( inputCommand, "exit" ) == 0 )
        {
            return 0;
        }
        else //execute the command passed as argument (Executecommands();)
        {
            if (!strcmp (firstWord, "cd"))
            {
                if (argv[1] == NULL)
                {
                    chdir ("/");
                }
                else
                {
                    
                    chdir (argv[1]);
                }
            }
            else
            {
                /* This is the part where the command will get executed */
                pid_t kidpid = fork();

                if (kidpid < 0)
                {
                    perror( "Internal error: cannot fork." );
                    return -1;
                }
                else if (kidpid == 0)
                {
                    for(int ct=1; ct < cmdCnt-1; ct++)
                    {
                        /* This is the part where we check whether pipe is in command and we call the function to execute commands via pipe */
                        if(!strcmp (argv[ct], "|"))
                        {
                            executed_via_pipeline = 1;
                            if(debugLog)
                            {
                                printf("argv %s\n", argv[0]);
                                printf("argv %s\n", argv[1]);
                                printf("argv %s\n", argv[2]);
                            }
                            /* This is the pipeCmdHandling Function */
                            pipeCmdHandling(argv);
                            break;
                        }
                    }
                    if(executed_via_pipeline == 1)
                    {
                        continue;
                    }
                    for(int ct=1; ct < cmdCnt-1; ct++)
                    {
#if 0
                        if(!strcmp (argv[ct], "|"))
                        {
                            executed_via_pipeline = 1;
                            printf("argv %s\n", argv[0]);
                            printf("argv %s\n", argv[1]);
                            printf("argv %s\n", argv[2]);
                            pipeCmdHandling(argv);
                            break;
                        }
#endif
                        /* Here we are checking the redirection command */

                        if(!strcmp (argv[ct], ">"))
                        {
                            /* Based on redirection we use appropriate close and fopen calls */
                            printf("This is standard output command\n");
                            printf("output file name [%s]\n", argv[ct+1]);
                            argv[ct] = NULL;
                            close(1); // Release fd no - 1
                            fopen(argv[ct+1], "w");
                            break;
                        }
                        if(!strcmp (argv[ct], "2>"))
                        {
                            printf("This is standard output error command\n");
                            printf("output file name [%s]\n", argv[ct+1]);
                            argv[ct] = NULL;
                            close(2); // Release fd no - 1
                            fopen(argv[ct+1], "w");
                            break;
                        }
                        if(!strcmp (argv[ct], "<"))
                        {
                            printf("This is standard input command\n");
                            printf("output file name [%s]\n", argv[ct+1]);
                            argv[ct] = NULL;
                            close(0); // Release fd no - 1
                            fopen(argv[ct+1], "r");
                            break;
                        }
                        if(debugLog)
                        {
                        printf("argv %s\n", argv[ct]);
                        }
                    }
                    if(executed_via_pipeline == 0)
                    {
                        /* This is the exec command which is executed */
                        /* The arguments to this function are the command */
                        execvp (newfirstWord, argv);
                        // The following lines should not happen (normally) unless there's an error.
                        perror( inputCommand );
                    }
                    return -1;
                }
                else
                {
                    // I am the parent.  Wait for the child.
                    int waitPidStatus;
                    /* Here we are waiting for command to be executed */
                    pid_t returnPid = waitpid( kidpid, &waitPidStatus, 0 );

                    if (returnPid == -1) { 
                        perror("waitpid"); 
                        exit(EXIT_FAILURE); 
                    }

                    /* Exit messages- PART 1 requirement--- extra for personal */
                    if (WIFEXITED(waitPidStatus)) {
                        printf("Program exited with waitPidStatus %d\n", WEXITSTATUS(waitPidStatus));
                    } else if (WIFSIGNALED(waitPidStatus)) {
                        printf("Program was killed by signal %d\n", WTERMSIG(waitPidStatus));
                    } else if (WIFSTOPPED(waitPidStatus)) {
                        printf("Program was stopped by signal %d\n", WSTOPSIG(waitPidStatus));
                    } else if (WIFCONTINUED(waitPidStatus)) {
                        printf("Program was continued \n");
                    }
                }
            }
        }
    
//execution ends here
        for (cmdCnt = 0; cmdCnt <= docodeCommand.size(); cmdCnt++ ){
            delete argv[cmdCnt];
        }
        delete [] argv;
    }

    return 0;
}



int find_file(char *dir_path, char *file_name)
{
    DIR           *d;
    struct dirent *dir;
    d = opendir(dir_path);
    if (d)
    {
        /* Function to find the command: It will return 1 when command gets found */
        while ((dir = readdir(d)) != NULL)
        {
            int len = strlen(file_name);
            if (strlen(dir->d_name) == len && !strcmp(dir->d_name, file_name)) 
            {
#if 0
                printf("Command found in [%s]\n", dir_path);
#endif
                (void)closedir(d);
                /* Command found */
                return 1;
            }
#if 0
            printf("%s\n", dir->d_name);
#endif
        }
#if 0
        printf("Command not found\n");
#endif

        closedir(d);
        /* command not found */
        return 0;
    }
    return 0;
}

/*  Function to add an element to the stack */
void pushd (char *dirStr)
{
    /* Function adds entry to stack */
    int num;
    if (dirStr[0] == '\0')
    {
        return;
    }
    if (stack.topElement == (MAXSIZE - 1))
    {
        printf ("Stack is Full\n");
        return;
    }
    else
    {
        stack.topElement = stack.topElement + 1;
        strcpy(stack.thisStack[stack.topElement], dirStr);
        printf ("pushed element is = %s\n", stack.thisStack[stack.topElement]);
    }
    return;
}
/*  Function to delete an element from the stack */
char *popd (char *dirStr)
{
    /* deletes entry from stack */
    int num;
    if (stack.topElement == - 1)
    {
        printf ("Stack is Empty\n");
        return NULL;
    }
    else
    {
        printf ("poped element is = %s\n", stack.thisStack[stack.topElement]);
        strcpy(dirStr, stack.thisStack[stack.topElement]);
        stack.topElement = stack.topElement - 1;
    }
    return dirStr;
}

/*  Function to display the status of the stack */
void display ()
{
    /* displays the stack */
    int i;
    if (stack.topElement == -1)
    {
        printf ("Stack is empty\n");
        return;
    }
    else
    {
        printf ("\n The status of the stack is \n");
        for (i = 0; i <= stack.topElement; i++)
#if 0
        for (i = stack.topElement; i >= 0; i--)
#endif
        {
            printf ("%s\n", stack.thisStack[i]);
        }
    }
    printf ("\n");
}

void pipeCmdHandling(char **myArgs)
{
    // File descriptors
    int fileDesOrig[2]; // pos. 0 output, pos. 1 input of the pipe
    int fileDesNew[2];

    int pipeCmdNumCmds = 0;

    char *pipeCmdInput[256];

    pid_t pid;

    int err = -1;
    int end = 0;

    // Variables used for the different loops
    int i = 0;
    int j = 0;
    int k = 0;
    int l = 0;

#if 0
    myArgs[0] = "cat";
    myArgs[1] = "this.txt";
    myArgs[2] = "|";
    myArgs[3] = "wc";
    myArgs[4] = NULL;
#endif
    if(debugLog)
    {
        printf("myArgs %s\n", myArgs[0]);
        printf("myArgs %s\n", myArgs[1]);
        printf("myArgs %s\n", myArgs[2]);
    }
    while (myArgs[l] != NULL){
        if (strcmp(myArgs[l],"|") == 0){
            pipeCmdNumCmds++;
        }
        l++;
    }
    pipeCmdNumCmds++;

    while (myArgs[j] != NULL && end != 1){
        k = 0;
        while (strcmp(myArgs[j],"|") != 0){
            pipeCmdInput[k] = myArgs[j];
            j++;    
            if (myArgs[j] == NULL){
                end = 1;
                k++;
                break;
            }
            k++;
        }
        pipeCmdInput[k] = NULL;
        j++;            

        if (i % 2 != 0)
        {
            pipe(fileDesOrig); // for odd i
        }
        else
        {
            pipe(fileDesNew); // for even i
        }

        pid=fork();

        if(pid==-1)
        {
            if (i != pipeCmdNumCmds - 1){
                if (i % 2 != 0){
                    close(fileDesOrig[1]); // for odd i
                }else{
                    close(fileDesNew[1]); // for even i
                } 
            }                       
            printf("Child process could not be created\n");
            return;
        }
        if(pid==0)
        {
            if (i == 0)
            {
                dup2(fileDesNew[1], STDOUT_FILENO);
            }
            else if (i == pipeCmdNumCmds - 1)
            {
                if (pipeCmdNumCmds % 2 != 0)
                { 
                    dup2(fileDesOrig[0],STDIN_FILENO);
                }
                else
                { 
                    dup2(fileDesNew[0],STDIN_FILENO);
                }
            }
            else
            { 
                if (i % 2 != 0)
                {
                    dup2(fileDesNew[0],STDIN_FILENO); 
                    dup2(fileDesOrig[1],STDOUT_FILENO);
                }
                else
                { // for even i
                    dup2(fileDesOrig[0],STDIN_FILENO); 
                    dup2(fileDesNew[1],STDOUT_FILENO);                                        
                } 
            }

            int myct = 0;
            while ( pipeCmdInput[myct] != NULL)
            {
                if(!strcmp (pipeCmdInput[myct], ">"))
                {
                    printf("This is standard output pipeCmdInput\n");
                    printf("output file name [%s]\n", pipeCmdInput[myct+1]);
                    pipeCmdInput[myct] = NULL;
                    close(1); // Release fd no - 1
                    fopen(pipeCmdInput[myct+1], "w");
                    break;
                }
                if(!strcmp (pipeCmdInput[myct], "2>"))
                {
                    printf("This is standard output error pipeCmdInput\n");
                    printf("output file name [%s]\n", pipeCmdInput[myct+1]);
                    pipeCmdInput[myct] = NULL;
                    close(2); // Release fd no - 2
                    fopen(pipeCmdInput[myct+1], "w");
                    break;
                }
                if(!strcmp (pipeCmdInput[myct], "<"))
                {
                    printf("This is standard input pipeCmdInput\n");
                    printf("output file name [%s]\n", pipeCmdInput[myct+1]);
                    pipeCmdInput[myct] = NULL;
                    close(0); // Release fd no - 0
                    fopen(pipeCmdInput[myct+1], "r");
                    break;
                }
                myct++;
            }
            if(debugLog)
            {
                printf("this_cmd %s \n", pipeCmdInput[0]);
                printf("this_cmd %s \n", pipeCmdInput[1]);
                printf("this_cmd %s \n", pipeCmdInput[2]);
            }
            if (execvp(pipeCmdInput[0],pipeCmdInput)==err)
            {
                kill(getpid(),SIGTERM);
            }               
        }

        if (i == 0){
            close(fileDesNew[1]);
        }
        else if (i == pipeCmdNumCmds - 1){
            if (pipeCmdNumCmds % 2 != 0){                                 
                close(fileDesOrig[0]);
            }else{                                  
                close(fileDesNew[0]);
            }
        }else{
            if (i % 2 != 0){                                        
                close(fileDesNew[0]);
                close(fileDesOrig[1]);
            }else{                                  
                close(fileDesOrig[0]);
                close(fileDesNew[1]);
            }
        }

        waitpid(pid,NULL,0);

        i++;    
    }

#if 1
    sleep(2);
#endif
}


////rough work
vector<string>dealwithargs(char* args){
    char* ptr = args;
    vector<string>arguments;
    string singlearg;
    while((*ptr) != '\0'){
    if((*ptr) == ' ')
    {
        arguments.push_back(singlearg);singlearg.clear();ptr++;
        if((*ptr) == '\0')
            {
            arguments.push_back(singlearg);
            singlearg.clear();
            }
    }
        else if((*ptr) == '\\'){
        ptr++;
            if((*ptr) == '\0'){
                arguments.push_back(singlearg);
                singlearg.clear();
                }
            
            if((*ptr) == ' '){
            singlearg = singlearg + " ";
                ptr++;
                if((*ptr) == '\0'){
                arguments.push_back(singlearg);
                    singlearg.clear();
                    }
                }
            
            else{
                singlearg = singlearg + "\\";
                if((*ptr) == '\\'){continue;}
                else{
                    singlearg.push_back((*ptr));
                    ptr++;
                    if((*ptr) == '\0'){
                        arguments.push_back(singlearg);
                        singlearg.clear();
                        }
                    }
                }
            }
        
      else{
    singlearg.push_back((*ptr));
          ptr++;
            if((*ptr) == '\0'){
               arguments.push_back(singlearg);
              singlearg.clear();
            }
           }
        
       }
    
   for(size_t i =0;i<arguments.size();i++){
     if(arguments[i] == ""){
          arguments.erase(arguments.begin()+i);
         i =0;
         }
}
   return arguments;
}


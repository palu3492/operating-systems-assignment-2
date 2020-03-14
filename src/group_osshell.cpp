#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <fstream>
 
std::vector<std::string> splitString(std::string text, char d);
std::string getFullPath(std::string cmd, const std::vector<std::string>& os_path_list);
bool fileExists(std::string full_path, bool *executable);

int main (int argc, char **argv)
{
    std::string input;
    char* os_path = getenv("PATH");
    
    std::vector<std::string> os_path_list = splitString(os_path, ':');
    std::vector<std::string> history;
    std::vector<std::string> args;
    std::string line;
    
    std::ifstream history_file_read("history.txt");
    
    // Reads from the history.txt file and places each line into the history
    // vector, which holds the history of our commands.
    while (std::getline(history_file_read, line))
    {
        history.push_back(line);
    }
    
    history_file_read.close();
    
    

    std::cout << "Welcome to OSShell! Please enter your commands ('exit' to quit)." << std::endl;

    // Repeat:
    //  Print prompt for user input: "osshell> " (no newline)
    //  Get user input for next command
    //  If command is `exit` exit loop / quit program
    //  If command is `history` print previous N commands
    //  For all other commands, check if an executable by that name is in one of the PATH directories
    //  If yes, execute it
    //  If no, print error statement: "<command_name>: Error running command" (do include newline)
    
    std::string fullPath;
    
    std::cout << "osshell> ";    
    std::getline(std::cin, input);
    if (input != ""){
        history.push_back(input);
    }
    
    while (input.compare("exit") != 0){        
        if (input != ""){
            args = splitString(input, ' ');
            if (args.at(0) == "history"){
                if (args.size() == 1){
                    for (int i = 0; i < history.size()-1; i++){
                        std::cout << "  " << i + 1 << ": " << history.at(i) << std::endl;
                    }
                }
                else if (args.at(1) == "clear"){
                    history.clear();
                }
                else{
                    int flag = 0;
                    for (int i = 0; i < args.at(1).length(); i++){
                        if (!isdigit(args.at(1)[i])){
                            flag = 1;
                        }
                    }
                    if (flag == 0){
                        int size = atoi(args.at(1).c_str());
                        if (size < 1){
                            std::cout << "Error: history expects an integer > 0 (or 'clear')" << std::endl;
                        }
                        else{
                            for (int i = size; i > 0; i--){
                                std::cout << "  " << history.size() - i << ": " << history.at(history.size() - i - 1) << std::endl;
                            }
                        }
                    }
                    else{
                        std::cout << "Error: history expects an integer > 0 (or 'clear')" << std::endl;
                    }
                }
            }
            else{
                //Gets the full path of the input
                fullPath = getFullPath(input, os_path_list);
                
                bool isNowExecutable = false;
                std::string inputPath = "";
                
                //The if executes if it couldn't find the input in the PATH,
                //and if it did the else executes
                if (fullPath == ""){
                    //If we didn't find the input in the path, this if statement
                    //checks to see if it is a relative or full path.
                    if (fileExists(args.at(0), &isNowExecutable)){
                        //If we find the file, if it is executable by the user,
                        //we enter this if statement
                        if (isNowExecutable){
                            char* argv[args.size() + 1];
                            inputPath = args.at(0);
                            //This loop adds arguments to the argv array, and for the
                            //input we only add the command to the array, not the full
                            //path.  The inner loop removes any quotation marks
                            for (int i = 0; i < args.size(); i++){
                                if (i != 0){
                                    for (int j = 0; j < args.at(i).length(); j++){
                                        if (args.at(i)[j] == '"'){
                                            args.at(i).erase(j, 1);
                                        }
                                    }
                                }
                                else{
                                    int loc = args.at(i).find_last_of("/");
                                    args.at(i) = args.at(i).substr(loc + 1);
                                }
                                argv[i] = const_cast<char *>(args.at(i).c_str());
                            }
                
                            //The last argument in the array must be null
                            argv[args.size()] = NULL;
                    
                
                            //This creates a child, executes the command, and then
                            //terminates the child while the parent waits for the child
                            //to complete
                            int pid = fork();
                            if (pid == 0){
                                execv(const_cast<char *>(inputPath.c_str()), argv);
                                exit(0);
                            }
                            else{
                                wait(NULL);
                            } 
                        }
                    }
                    else{
                        //Prints if there is an error running the command
                        std::cout << input << ": Error running command" << std::endl;
                    }
                }
                else{
                    //Create a char* array for our arguments
                    char* argv[args.size() + 1];

                    //This the outer loop adds the arguments to the argv array,
                    //and the inner loop checks to see if there are any quotation
                    //marks.  If there are, it removes them.
                    for (int i = 0; i < args.size(); i++){
                        for (int j = 0; j < args.at(i).length(); j++){
                            if (args.at(i)[j] == '"' || args.at(i)[j] == '\''){
                                args.at(i).erase(j, 1);
                            }
                        }
                        argv[i] = const_cast<char *>(args.at(i).c_str());
                        
                        
                    }
                
                    //The last argument in the argument array must be null
                    argv[args.size()] = NULL;
                
                    //Here we fork, have the child execute the command, then exit,
                    //while the parent waits for the child to complete.
                    int pid = fork();
                    if (pid == 0){
                        execv(const_cast<char *>(fullPath.c_str()), argv);
                        exit(0);
                    }
                    else{
                        wait(NULL);
                    }
                }
            }
            
            
        }
        
        //This re-prompts the user for a new input
        std::cout << "osshell> ";
        std::getline(std::cin, input);
        if (input != ""){
            history.push_back(input);
        }
    }
    
    
    //The following code only executes after the command 'exit' has been
    //entered, and it writes the history vector to history.txt.
    std::ofstream history_file_write("history.txt");
    for (int i = 0; i < history.size(); i++){
        history_file_write << history.at(i) << std::endl;
    }
    history_file_write.close();
    return 0;
}

// Returns vector of strings created by splitting `text` on every occurance of `d`
std::vector<std::string> splitString(std::string text, char d)
{
    text.push_back(d);
    std::vector<std::string> result;
    int size = 0;
    int prev = 0;
    for (int i = 0; i < text.length(); i++){
        if (text[i] == d){
            size = i - prev;
            result.push_back(text.substr(prev, size));
            prev = i + 1;
            size++;
        }    
    
    }
    return result;
}

// Returns a string for the full path of a command if it is found in PATH, otherwise simply return ""
std::string getFullPath(std::string cmd, const std::vector<std::string>& os_path_list)
{
    
    //The two loops check to see if there is a space in the command, and
    //if there is, only uses the first part of the command
    std::string temp;
    int loc = -1;
    for (int i = 0; i < cmd.length(); i++){
        if (cmd[i] == ' '){
            loc = i;
            break;
        }
    }
    
    if (loc != -1){ 
        for (int i = 0; i < loc; i++){
            temp.push_back(cmd[i]);
        }
    
        cmd = temp;
    }
    
    std::string result = "";
    bool isExecutable;
    
    //The loop creates a full path using the path list and the passed command,
    //and runs it through fileExists.  If the file exists and is able to be
    //executed, then it sets result to that path.
    for (int i = 0; i < os_path_list.size(); i++){
        std::string checkPath = os_path_list.at(i) + "/" + cmd;
        if (fileExists(checkPath, &isExecutable)){
            if (isExecutable){
                result = checkPath;
            }
        }
    }
    
    return result;
}

// Returns whether a file exists or not; should also set *executable to true/false 
// depending on if the user has permission to execute the file
bool fileExists(std::string full_path, bool *executable)
{
    *executable = false;
    
    //The first if checks to see if the file exists, then the
    //inner if checks to see if it is executable.  If it is, then
    //it sets the executable pointer to true, and returns true
    //if the file exists, otherwise it returns false.
    if (access(full_path.c_str(), F_OK) == 0){
        if (access (full_path.c_str(), X_OK) == 0){
            *executable = true;
        }
        return true;
    }
    return false;
}

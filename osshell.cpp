#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <unistd.h>

// compile: g++ -std=c++11 -o osshell osshell.cpp
 
std::vector<std::string> splitString(std::string text, char d);
std::string getFullPath(std::string cmd, const std::vector<std::string>& os_path_list);
bool fileExists(std::string full_path, bool *executable);

int main (int argc, char **argv) {
    std::string input;
    char* os_path = getenv("PATH");
    std::vector<std::string> os_path_list = splitString(os_path, ':');

    std::cout << "Welcome to OSShell! Please enter your commands ('exit' to quit)." << std::endl;

    // Repeat:
    //  Print prompt for user input: "osshell> " (no newline)
    std::cout << "osshell> ";
    //  Get user input for next command
    std::string input_string = "";
    std::cin >> input_string;
    
    //  If command is `exit` exit loop / quit program
    //  If command is `history` print previous N commands
    //  For all other commands, check if an executable by that name is in one of the PATH directories
    //  If yes, execute it
    
    // find path varible
    std::string path_variable(getenv("PATH")); // read PATH variable and make string
    // std::cout << path_variable << std::endl << std::endl;
    std::string delimiter = ":";
	// split on colons
	size_t pos = 0;
	std::string directory_path;
	do{
		pos = path_variable.find(delimiter);
		directory_path = path_variable.substr(0, pos);
		
		bool executable;
		std::string full_path = directory_path + "/" + input_string;
		bool file_exists = fileExists(full_path, &executable);
		if(executable && file_exists) {
			std::cout << "found " << input_string << " in " << full_path << std::endl;
			char *const parmList[] = {(char *)full_path.c_str(), NULL};
			execv(full_path.c_str(), parmList);
			break;
		}
		
		// std::cout << token << std::endl; // print each directory expect last
		path_variable.erase(0, pos + delimiter.length()); // <-- last directory not addressed
	}while(pos != std::string::npos);
	// std::cout << path_variable << std::endl; // print last directory
	
		
	// if command then run it in sub process
	
	//   If no, print error statement: "<command_name>: Error running command" (do include newline)

	return 0;
}

// Returns vector of strings created by splitting `text` on every occurance of `d`
std::vector<std::string> splitString(std::string text, char d)
{
    std::vector<std::string> result;

    return result;
}

// Returns a string for the full path of a command if it is found in PATH, otherwise simply return ""
std::string getFullPath(std::string cmd, const std::vector<std::string>& os_path_list)
{
    return "";
}

#include <sys/stat.h>
// Returns whether a file exists or not; should also set *executable to true/false 
// depending on if the user has permission to execute the file
bool fileExists(std::string full_path, bool *executable) {
    *executable = false;
//     return false;
	struct stat buffer;
	if(stat (full_path.c_str(), &buffer) == 0) {
		*executable = buffer.st_mode & S_IXUSR;
		return true;
	}
	return false;
}
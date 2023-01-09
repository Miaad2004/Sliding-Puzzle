# pragma once

# define _CRT_SECURE_NO_WARNINGS

#if _WIN32 || _WIN64
# define WINDOWS
# include <windows.h>
# define CLEAR_UP_THE_SCREEN system("cls")
# define WAIT system("pause")
# define SET_CONSOLE_COLOR(x) system(x)

// Arrow keys
# define KEY_UP    72  
# define KEY_LEFT  75
# define KEY_RIGHT 77
# define KEY_DOWN  80
#endif 

# if __linux__ || __unix__ || __APPLE__
# pragma message "Colored text is not supported in linux based OSs."
# define LINUX
# define CLEAR_UP_THE_SCREEN system("clear")
# define SET_CONSOLE_COLOR(x)                 /* Replace with nothing */
# define WAIT \
	std::cout << "Press any key to continue . . ."; \
	std::cin.get(); \
	std::cout << std::endl;

/*'getch' Function is not supported in linux so the user have to enter the keys manually*/
# define KEY_UP    'w' 
# define KEY_LEFT  'a'  
# define KEY_RIGHT 'd'  
# define KEY_DOWN  's'  
# endif 

# include <iostream>
# include <conio.h>

# include <string>
# include <cstring>
# include <set>
# include <vector>

# include <filesystem>
# include <fstream>
# include <sstream>          /* For printing multiline strings line by line*/

# include <math.h>
# include <cstdlib>          /* For generating random numbers */
# include <chrono>           /* For calculating time periods */
# include <ctime>

# include "colors.h"         
# include "font.h"
# include "printFunctions.h"

typedef unsigned long long int Long_uInt;
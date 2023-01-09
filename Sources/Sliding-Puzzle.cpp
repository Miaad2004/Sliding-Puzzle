/*
* Fundamentals Of Programming Project
* SLIDING-PUZZLE By Miaad Kimiagari
* 1/3/2023
* For more info on the 15-Puzzle check out https://en.wikipedia.org/wiki/15_puzzle & https://15puzzle.netlify.app/
* C++ Language Standard: ISO C++20 Standard
*/

# include "../Headers/Sliding-Puzzle.h"

# define GRAPHICAL_OUTPUT 1             // IF 0; tables' elements will be printed as normal sized text           

std::string savePath;


enum Direction
{
    left, right, up, down
};

enum Difficulty
{
    easy = 11, normal = 45, hard = 10000    // Number of times elements are moved in the shuffle function
};

struct Player
{
public:
    char name[50];
    mutable bool complete = false;         //  True if the player has completed the game
    int score = 0;
    int movementCount = 0;
    int gameCount = 1;                     // Number of times the player has played the game  // CURRENTLY NOT USED
    Long_uInt duration = 0;                // In seconds
    Difficulty difficultyLevel = normal;

    int tableSize = 4;         // Max table size is 9   
    int** table;
    int** answer;              // Sorted table
    int nullPosI, nullPosJ;    // Position of '0' in the table

    Player()
    {
        bool hasBeenloaded = false;

        SET_CONSOLE_COLOR(COLOR_CONSOLE_BLACK_GREEN);
        std::cout << "Enter your name: ";
        gets_s(name);
        std::cout << std::endl;

        if (playerExists())
        {
            if (isComplete())    // If the player has completed the game
            {
                char input;
                std::cout << "You have completed the game " << this->gameCount << " time(s).\n" << std::endl;
                std::cout << "Do you want to play again?(YOUR LAST GAME WILL BE DELETED)(y/n)" << std::endl;
                std::cin >> input;
                if (input == 'n' || input == 'N')
                {
                    WAIT;
                    print::scoreboard();
                    exit(0);
                }
            }

            else
            {
                char input;
                std::cout << "Do you want to continue your last game?(IF NO YOUR PROGRESS WILL BE LOST)(y/n) " << std::endl;
                std::cin >> input;

                if (input == 'y' || input == 'Y')
                {
                    std::cout << "Loading..." << std::endl;
                    load();
                    hasBeenloaded = true;
                }
            }
        }

        if (!hasBeenloaded)      // Create new player
        {
            // Set the difficulty level
            std::cout << "Enter the difficulty level:(1,2,3)\n1) Easy\n2) Normal\n3) Hard" << std::endl;
            int temp;
            std::cin >> temp;

            switch (temp)
            {
            case 1: difficultyLevel = easy; break;
            case 2: difficultyLevel = normal; break;
            case 3: difficultyLevel = hard; break;
            default: difficultyLevel = normal; break;
            }

            // Set the table size
            std::cout << "Enter the table size: ";
            std::cin >> tableSize;
            std::cout << std::endl;

            while (tableSize > 9)   // Check if the input is valid
            {
                std::cout << "Maximum size is 9." << std::endl;
                std::cout << "Enter the table size: ";
                std::cin >> tableSize;
                std::cout << std::endl;
            }
            createTable();
        }
    }

    static int randInt(int lowerBound, int upperBound)     // Source: https://stackoverflow.com/questions/19238403/random-function-generator-between-two-integers-c
    {                                                      // Source: https://stackoverflow.com/questions/10051071/rand-function-does-not-work-properly-in-c
        return lowerBound + (std::rand() % (upperBound - lowerBound + 1));
    }

    void createTable()
    {
        // Create Dynamically allocated 2D arrays
        table = new int* [tableSize];
        answer = new int* [tableSize];
        for (int i = 0; i < tableSize; i++)
        {
            table[i] = new int[tableSize];
            answer[i] = new int[tableSize];
        }

        // Create a sorted table 
        for (int i = 0; i < tableSize * tableSize; i++)    // Assuming the table is a 1D array 
        {
            int row = i / tableSize;
            int col = i % tableSize;

            if (i == tableSize * tableSize - 1)            // If it's the last element
            {
                table[row][col] = answer[row][col] = 0;    // Set the last element to 0
                continue;
            }

            table[row][col] = answer[row][col] = i + 1;    // Fill in the table from 1 to 'tableSize'
        }

        // Shuffle the table
        shuffleTheTable();
    }

    void shuffleTheTable()
    {
        for (int i = 0; i < (int)difficultyLevel * tableSize; i++)         // Move elements based on the difficulty level
            move((Direction)randInt(0, 3), false);
    }

    void printTheTable() const
    {
        CLEAR_UP_THE_SCREEN;

        // Method 1) Print the elements as graphical objects defined in fonts.cpp
        if (GRAPHICAL_OUTPUT && tableSize <= 4)
            for (int i = 0; i < tableSize; i++)        // For each row
            {
                // Find the indexes of correctly positioned elements
                std::set <int> coloredIndexes;
                for (int j = 0; j < tableSize; j++)
                    if (table[i][j] == answer[i][j])
                        coloredIndexes.insert(j);

                // Print the row
                print::row(table[i], tableSize, coloredIndexes);
            }

        // Method 2) Print the elements as numbers
        else
            for (int i = 0; i < tableSize; i++)        // For each row
            {
                for (int j = 0; j < tableSize; j++)
                {
                    bool isCorrect = table[i][j] == answer[i][j];            // If the element id correct it'll be printed in yellow
                    print::element(table[i][j], isCorrect);
                    std::cout << " | ";       // Print the vertical grids
                }

                std::cout << std::endl;
                print::printChar('-', tableSize * strlen("  |  "));          // Print the horizontal grids
                std::cout << std::endl;
            }
    }


    void updateNullPos()    // Updates the current position of '0' in the table
    {
        for (int i = 0; i < tableSize; i++)
            for (int j = 0; j < tableSize; j++)
                if (table[i][j] == 0)
                {
                    nullPosI = i;
                    nullPosJ = j;
                }
    }

    bool isValidIndex(int i, int j) const          // Checks if the given index exists in the table
    {
        if (i >= tableSize || j >= tableSize || i < 0 || j < 0)
            return false;

        return true;
    }

    void move(Direction direction, bool isHuman = true)      // Moves '0' to the given direction if possible
    {
        this->updateNullPos();        // Update the position of '0' in the table
        int i = this->nullPosI;
        int j = this->nullPosJ;

        switch (direction)
        {
        case(up):
            if (isValidIndex(i - 1, j))
            {
                table[i][j] = table[i - 1][j];
                table[i - 1][j] = 0;
                if (isHuman) movementCount++;            // Only add to the movementCount if Function is not called by shuffle function
            }
            break;

        case(down):
            if (isValidIndex(i + 1, j))
            {
                table[i][j] = table[i + 1][j];
                table[i + 1][j] = 0;
                if (isHuman) movementCount++;
            }
            break;

        case(right):
            if (isValidIndex(i, j + 1))
            {
                table[i][j] = table[i][j + 1];
                table[i][j + 1] = 0;
                if (isHuman) movementCount++;
            }
            break;

        case(left):
            if (isValidIndex(i, j - 1))
            {
                table[i][j] = table[i][j - 1];
                table[i][j - 1] = 0;
                if (isHuman) movementCount++;
            }
            break;
        }
        this->updateNullPos();          // Update the position of '0' in the table
    }


    bool playerExists() const        // Checks if any file is titled the player's name in the save directory
    {
        std::string fileName = savePath + std::string(name) + ".puzzle";
        std::ifstream file(fileName.c_str());

        bool streamStatus = file.good();    // Check if the file stream is OK
        file.close();
        return streamStatus;
    }

    bool isComplete() const          // opens the saved game & checks if the game has been completed
    {
        bool isComplete = false;

        std::string fileName = savePath + std::string(name) + ".puzzle";
        std::ifstream file(fileName.c_str());

        std::string line;
        int lineNumber = 1;
        while (getline(file, line))         // If successful, getline() returns the number of characters that are read, including \n
        {
            if (lineNumber == 2)
            {
                isComplete = stoi(line);
                break;
            }
            lineNumber++;
        }
        return isComplete;
    }

    void load()        // Loads a player from a file
    {
        /*
        * FILE STRUCTURE
        * Name
        * isComplete
        * tableSize
        * score
        * movementCount
        * gameCount
        * duration
        * first element of the table
        * second .....
        * first elements of the answers table
        * second ....
        */

        std::string fileName = savePath + std::string(name) + ".puzzle";
        std::ifstream file(fileName);

        std::string line;
        int lineNumber = 1;
        while (getline(file, line))         // If successful, getline() returns the number of characters that are read, including \n
        {
            switch (lineNumber)
            {
            case 1: strcpy_s(name, line.c_str()); break;
            case 2: complete = stoi(line); break;
            case 3: tableSize = stoi(line); createTable(); break;
            case 4: score = stoi(line); break;
            case 5: movementCount = stoi(line); break;
            case 6: gameCount = stoi(line); break;
            case 7: duration = stoi(line); break;
            default:
                if (lineNumber <= tableSize * tableSize + 7)
                {
                    int rowIndex = (lineNumber - 8) / tableSize;
                    int colIndex = (lineNumber - 8) % tableSize;
                    int x = stoi(line);
                    table[rowIndex][colIndex] = x;
                }

                else
                {
                    int rowIndex = (lineNumber - (8 + tableSize * tableSize)) / tableSize;
                    int colIndex = (lineNumber - (8 + tableSize * tableSize)) % tableSize;
                    answer[rowIndex][colIndex] = stoi(line);
                }

                break;
            }
            lineNumber++;
        }

        file.close();
    }

    void save() const          // Saves the player to a file
    {
        std::filesystem::create_directories(savePath);

        std::string fileName = savePath + std::string(name) + ".puzzle";
        std::ofstream file;
        file.open(fileName);

        file << name << "\n";
        file << complete << "\n";
        file << tableSize << "\n";
        file << score << "\n";
        file << movementCount << "\n";
        file << gameCount << "\n";
        file << duration << "\n";
        for (int i = 0; i < tableSize; i++)
            for (int j = 0; j < tableSize; j++)
                file << table[i][j] << "\n";

        for (int i = 0; i < tableSize; i++)
            for (int j = 0; j < tableSize; j++)
                file << answer[i][j] << "\n";

        file.close();
    }


    bool hasEnded() const          // returns true if all the answers are correct
    {
        // Check if all the answers are correct
        for (int i = 0; i < tableSize; i++)
            for (int j = 0; j < tableSize; j++)
                if (table[i][j] != answer[i][j])
                    return false;

        complete = true;
        return true;
    }

    void printResult() const               // Prints player's score 
    {
        CLEAR_UP_THE_SCREEN;
        SET_CONSOLE_COLOR(COLOR_CONSOLE_AQUA_BLACK);

        std::cout << "Great Job!" << std::endl;
        std::cout << "Number of movements: " << movementCount << std::endl;
        std::cout << "Time taken: " << std::setprecision(3) << (double)duration / 60 << " minutes" << std::endl;
    }

    ~Player()     // Destructor
    {
        // Memory clean up
        for (int i = 0; i < tableSize; i++)
        {
            delete[] table[i];
            delete[] answer[i];
        }

        delete[] answer;
        delete[] table;
    }
};

int main()
{
    // Set the save directory based on the OS
    # ifdef WINDOWS
    savePath = getenv("appdata") + std::string("\\15-Puzzle\\Saves\\");
    #endif

    # ifdef LINUX
    savePath = ".\\15-Puzzle\\Saves\\";
    # endif

    // Create a new directory in the AppData folder
    std::filesystem::create_directories(savePath);

    // Set random seed
    std::srand((unsigned)std::time(NULL));

    // Variables
    char CONSOLE = 0;

    // Welcome page
    WAIT;
    print::welcomePage();
    WAIT;
    CLEAR_UP_THE_SCREEN;

    Player player;         // Create a new player

    CLEAR_UP_THE_SCREEN;
    SET_CONSOLE_COLOR(COLOR_CONSOLE_BLACK_PURPLE);
    std::cout << "During the game press 'q' to save & exit." << std::endl;
    std::cout << "During the game press 's' to view the scoreboard." << std::endl;
    WAIT;

    // Start the timer
    auto startTime = std::chrono::high_resolution_clock::now();

    // Main loop
    while (true)
    {
        player.printTheTable();

        // Get command
    # ifdef WINDOWS
        CONSOLE = _getch();       // Keyboard input (Windows only) 
    #endif
    # ifdef LINUX
        std::cin >> CONSOLE;
    # endif

        // Handle the command
        switch (CONSOLE)
        {
        case KEY_UP: player.move(up); break;
        case KEY_DOWN: player.move(down); break;
        case KEY_RIGHT: player.move(right); break;
        case KEY_LEFT: player.move(left); break;
        }

        if (CONSOLE == 's')
            print::scoreboard();

        else if (player.hasEnded() || CONSOLE == 'q')
        {
            // Calculate the time frame
            auto stop = std::chrono::high_resolution_clock::now();
            player.duration += std::chrono::duration_cast<std::chrono::microseconds>(stop - startTime).count() / 1000000;    // Calculate the duration in seconds

            // Calculate the score
            player.score += pow(player.tableSize, 2) * (100 / log10((player.duration / 60 + sqrt(player.movementCount))));
            switch (player.difficultyLevel)
            {
            case normal: player.score *= 1.5;
            case hard: player.score *= 3;
            }

            player.printResult();
            player.save();
            WAIT;
            break;
        }
    }

    // Print the scoreboard
    print::scoreboard();
}
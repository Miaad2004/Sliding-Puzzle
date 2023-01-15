# include "../Headers/Sliding-Puzzle.h"

namespace fs = std::filesystem;

extern std::string savePath;

namespace print {

    void printChar(char character, int count)
    {
        for (int i = 0; i < count; i++)
            std::cout << character;
    }

    static void setTextColor(int colorCode)        // Windows only
    {
#ifdef WINDOWS
        HANDLE Console_Handeler = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(Console_Handeler, colorCode);
#endif 
    }


    void welcomePage()
    {
        CLEAR_UP_THE_SCREEN;
        SET_CONSOLE_COLOR(COLOR_CONSOLE_BLACK_RED);

        std::cout << titles::t1 << std::endl;
        std::cout << titles::t2 << std::endl;
        std::cout << titles::t3 << std::endl;
    }

    void scoreboard()      // Loads all the players up from the save directory to an array & prints the sorted array.
    {
        // Get all the files' paths in the save directory
        std::vector <std::string> paths;
        for (const auto& entry : fs::directory_iterator(savePath))    // Source : https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
            paths.push_back(entry.path().string());

        // ================ Load All The Players =================
        struct Player
        {
            char name[50];
            int score;
            Long_uInt duration;
            bool complete;
        };
        std::vector <Player> players(paths.size());

        // Load each file in the directory
        for (int i = 0; i < paths.size(); i++)
        {
            const std::string& path = paths[i];
            std::ifstream file(path);

            std::string line;
            int lineNumber = 1;
            while (getline(file, line))
            {
                switch (lineNumber)
                {
                case 1: strcpy_s(players[i].name, 51, line.c_str()); break;
                case 2: players[i].complete = stoi(line); break;
                case 4: players[i].score = stoi(line); break;
                case 7: players[i].duration = stoi(line); break;
                }
                lineNumber++;
            }

            file.close();
        }

        // Sort the players based on their scores
        for (int i = 0; i < players.size(); i++)
            for (int j = 0; j < (players.size() - 1) - i; j++)
                if (players[j].score < players[j + 1].score)
                {
                    Player temp = players[j];
                    players[j] = players[j + 1];
                    players[j + 1] = temp;
                }

        // ================ Print The Scoreboard =================
        CLEAR_UP_THE_SCREEN;
        SET_CONSOLE_COLOR(COLOR_CONSOLE_AQUA_BLACK);

        // Find the maximum length of the names
        int padding = 5;         // Number of the spaces between columns
        int maxLen = 0;

        for (int i = 0; i < players.size(); i++)
            if (strlen(players[i].name) > maxLen)
                maxLen = strlen(players[i].name) + strlen("1) ");

        // Print the titles
        setTextColor(COLOR_RED_BLACK);
        printChar(' ', (maxLen - strlen("NAME")) / 2);
        std::cout << "NAME";
        printChar(' ', (maxLen - strlen("NAME")) / 2);
        printChar(' ', padding);
        std::cout << "SCORE";
        printChar(' ', padding);
        std::cout << "DURATION(Minutes)";
        printChar(' ', padding);
        std::cout << std::endl;

        // Print the rows
        int counter = 1;     // Keeps track of the number of rows that have been printed 
        for (int i = 0; i < players.size(); i++)
        {
            Player player = players[i];
            if (!player.complete)          // Only print if the game has been completed 
                continue;

            if (counter >= 10)             // Only print the top 10
                break;

            // Change text color each 2 lines
            if (counter % 2 == 0)
                setTextColor(COLOR_GREEN_BLACK);

            else
                setTextColor(COLOR_YELLOW_BLACK);

            // Print
            std::cout << counter << ") ";
            std::cout << player.name;
            printChar(' ', maxLen - strlen(player.name) - strlen("1) "));        // All the texts should have the same length
            printChar(' ', padding);
            std::cout << player.score;
            printChar(' ', padding + strlen("DURATION(Minutes)") / 2);
            std::cout << std::setprecision(3) << (double)player.duration / 60;         // Convert seconds to minutes
            printChar(' ', 5);
            printChar(' ', padding);
            std::cout << std::endl;

            counter++;
        }

        setTextColor(COLOR_DEFAULT);        // Reset color to the default
        WAIT;
    }

    void element(int element, bool isColored)
    {
        // Colored text
        if (element == 0)
            setTextColor(COLOR_CLEAR_GREEN);

        else if (isColored)
            setTextColor(COLOR_CLEAR_YELLOW);

        else
            setTextColor(COLOR_DEFAULT);

        // Print
        if (element < 10)       // If it's a single digit add padding
            std::cout << element << " ";

        else
            std::cout << element;

        setTextColor(COLOR_DEFAULT);      // Reset color to the default
    }

    void row(const int row[], int rowLength, std::set<int> coloredIndexes)
    {
        /* All the numbers MUST have the same size */

        struct Number
        {
            std::vector <std::istringstream> streams;   // 2-digit numbers have multiple streams
            bool hasMultipleDigits = false;
            bool isColored = false;
            int value = 0;                              // The number's numeric value
        };

        //  ============== Create The Streams ==============
        std::vector <Number*> numbers;

        for (int i = 0; i < rowLength; i++)           // For each element in the vector 
        {
            // Create a digit object
            Number* number = new Number;
            number->value = row[i];

            if (number->value >= 10)                  // If the number->value has multiple numbers
                number->hasMultipleDigits = true;    // You can instead of 'digit->' use number->

            if (coloredIndexes.find(i) != coloredIndexes.end())
                number->isColored = true;

            for (char character : std::to_string(number->value))
                number->streams.push_back(std::istringstream(font::numbers[character - '0']));

            numbers.push_back(number);
        }

        // ============== Handle The Streams ==============
        int lineNumber = 1;
        while (lineNumber <= font::maxCharHeight)       // While the given row is completely printed
        {
            for (auto number : numbers)
            {
                // Set the text color
                if (number->value == 0)
                    setTextColor(COLOR_CLEAR_GREEN);

                else if (number->isColored)
                    setTextColor(COLOR_CLEAR_YELLOW);

                else
                    setTextColor(COLOR_DEFAULT);

                // print
                if (!number->hasMultipleDigits)                  // Before printing If it's a single number add half of the padding
                    printChar(' ', font::maxCharWidth / 2);

                // Handle each stream for the current number
                for (int i = 0; i < number->streams.size(); i++)
                {
                    std::string line;
                    getline(number->streams[i], line);
                    std::cout << line;
                }

                if (!number->hasMultipleDigits)                  // After printing If it's a single number add the other half of the padding
                    printChar(' ', font::maxCharWidth / 2);

                printChar(' ', font::maxCharWidth / 2);           // Add padding between the columns
            }

            std::cout << std::endl;
            lineNumber++;
        }
        std::cout << std::endl;    // Add padding between the rows
    }
}
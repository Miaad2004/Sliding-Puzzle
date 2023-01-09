# pragma once
# include <set>

namespace print {
	void welcomePage();
	void scoreboard();
	void element(int element, bool isColored);          // Prints a single element
	void printChar(char character, int count);
	void row(const int row[], int rowLength, std::set <int> coloredIndexes);    // Graphical
}
#include <string>
#include <iostream>
#include <cstdlib>
#include "trie.h"

using namespace std;

int main(int argc, char **argv) {
	string dictionary_path = "../data/Default_dictionary.txt";
	string wiktionary_path = "../data/wiktionary.txt";

	Trie t;

	cout << "Reading dictionary... ";
	t.insert_from_file(wiktionary_path, true);
	cout << "Done." << endl;

	// string input;
	// while (true) {
	// 	cout << "Enter word: ";
	// 	cin >> input;

	// 	for (string s : t.autocorrect(input, 1)) {
	// 		cout << "\t" << s << endl;
	// 	}
	// }

	cout << "Suggestions: " << endl;
	for (string s : t.autocorrect("thier", 2)) {
		cout << "\t" << s << endl;
	}

	// t.autocorrect("thier", 1);

	return 0;
}

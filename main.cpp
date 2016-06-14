#include <string>
#include <iostream>
#include <utility>
#include "trie.h"
#include "ngram.h"
#include "sentence_disambiguation.h"
#include "neural_network.h"

using namespace std;

/* TODO:
	1.n-gram prediction
	2. memory efficient DAWG
	3. grammar checking
*/

int main(int argc, char **argv) {
	// string dictionary_path = "../data/Default_dictionary.txt";
	// string wiktionary_path = "../data/wiktionary.txt";

	// Trie t;

	// cout << "Reading dictionary... ";
	// t.insert_from_file(wiktionary_path, true);
	// cout << "Done." << endl;

	// cout << "Suggestions: " << endl;
	// for (string s : t.autocorrect("mottorc", 2)) {
	// 	cout << "\t" << s << endl;
	// 	for (string s2 : t.autocomplete(s, 10)) {
	// 		cout << "\t\t" << s2 << endl;
	// 	}
	// }

	string test = "hi my, name is piyush - and I have -33.4910.!!";
	for (string s : Tokenizer::tokenize(test)) {
		cout << s << endl;
	}

	return 0;
}

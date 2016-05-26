#include <string>
#include <iostream>
#include <cstdlib>
#include "trie.h"

using namespace std;

int main(int argc, char **argv) {
	string dictionary_path = "../data/Default_dictionary";

	Node root;

	root.insert("smog", 5.0);
	root.insert("buck", 10.0);
	root.insert("sad", 12.0);
	root.insert("spite", 20.0);
	root.insert("spit", 15.0);
	root.insert("spy", 7.0);

	cout << Node::get_max_weight(&root) << endl;
	root.remove("spite");
	cout << Node::get_max_weight(&root) << endl;

	return 0;
}
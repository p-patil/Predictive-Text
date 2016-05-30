#include <string>
#include <cstring>
#include <map>
#include <limits>
#include <sstream>
#include <exception>
#include <queue>
#include <vector>
#include <fstream>
#include <algorithm>
#include <tuple>
#include <iostream>
#include "trie.h"

/* Begin Node class. */

map<Node *, double> Node::max_weights;

Node::Node(void) : end(false), weight(-1) {}

Node::Node(bool e) : end(e), weight(-1) {}

Node::Node(bool e, double w) : end(e), weight(w) {}

Node::Node(const Node &n) { 
	this->end = n.is_end();
	this->weight = n.get_weight();
	this->children = n.get_children();
}

/* Static function. */
double Node::get_max_weight(Node *n) {
	if (Node::max_weights.find(n) == Node::max_weights.end()) { // Node not found
		return - numeric_limits<double>::infinity();
	} else {
		return Node::max_weights.find(n)->second;
	}
}

/* Static function. */
void Node::set_max_weight(Node *n, double w) { Node::max_weights[n] = w; }

/* Static function. */
void Node::remove_max_weight(Node *n) { Node::max_weights.erase(n); }

bool Node::is_end(void) const { return this->end; }

double Node::get_weight(void) const { return this->weight; }

int Node::num_children(void) const { return this->children.size(); }

Node * Node::get_child(char c) const {
	if (!this->contains_key(c)) {
		stringstream error_message;
		error_message << "Node with key '" << c << "' not found";
		throw runtime_error(error_message.str());
	}
	
	return this->children.find(c)->second;
}

map<char, Node *> Node::get_children(void) const { return this->children; }

void Node::set_child(char c, Node *n) { this->children[c] = n; }

bool Node::contains_key(char c) const { return this->children.find(c) != this->children.end(); }

void Node::set_end(bool e) { this->end = e; }

void Node::set_weight(double w) { this->weight = w; }

/* Inserts the word-weight pair into the trie beneath this node, returning whether or not the word was already present. */
bool Node::insert(const string word, double weight) {
	/* First, update max weight. */
	if (weight > Node::get_max_weight(this)) {
		Node::set_max_weight(this, weight);
	}


	/* Base case. */
	if (word.length() == 1) {
		if (this->contains_key(word[0])) { // If node already exists, just update weight
			if (!this->get_child(word[0])->is_end()) {
				this->get_child(word[0])->set_end(true);
			}
			
			if (weight != this->get_child(word[0])->get_weight()) {
				this->get_child(word[0])->set_weight(weight);
			} else {
				return false; // Word-weight pair already exists
			}
		} else { // Word not found, so insert
			Node *n = new Node(true, weight);
			this->set_child(word[0], n);
		}

		/* Update max weight of terminal node. */
		Node::set_max_weight(this->get_child(word[0]), weight);
		return true;
	}

	/* If necessary, create a new node with the first character of the word, and recursively insert. */
	if (!this->contains_key(word[0])) {
		Node *n = new Node(false);
		bool ret = n->insert(word.substr(1), weight);
		this->set_child(word[0], n);

		return ret;
	} else {
		return this->get_child(word[0])->insert(word.substr(1), weight);
	}
}

/* Returns if the word exists below this node. */
bool Node::contains(const string word) { return this->get_weight(word) != -1; }

/* Removes the word from beneath this node, returning if the word existed or not. */
bool Node::remove(const string word) {
	/* Base case. */
	if (word.length() == 1) {
		if (!this->contains_key(word[0])) {
			return false;
		}

		/* Remove the node if it has no children; otherwise, simply set its 'end' flag to false. */
		if (this->get_child(word[0])->num_children() == 0) {
			delete this->get_child(word[0]);
			this->children.erase(word[0]);
			
			/* Update max weights. */
			if (this->num_children() == 0) {
				if (this->is_end()) {
					// If this node marks the end of a word, set its max weight to its own weight since it has no children
					Node::set_max_weight(this, this->get_weight());
				} else { // Otherwise, this node will be deleted in the parent frame, so remove from max weights
					Node::remove_max_weight(this);
				}
			} else { // Set the max weight of this node to the maximum max weight of the children
				double best = - numeric_limits<double>::infinity();
				double temp;
				for (auto const &it : this->children) {
					temp = Node::get_max_weight(it.second);
					if (temp > best) {
						best = temp;
					}
				}

				Node::set_max_weight(this, temp);
			}
		} else {
			this->get_child(word[0])->set_end(false);
			Node::remove_max_weight(this->get_child(word[0]));
		}

		return true;
	}

	/* Recursively remove from child, if word exists. */
	if (!this->contains_key(word[0])) {
		return false;
	}

	bool ret = this->get_child(word[0])->remove(word.substr(1));

	/* If recursively removing the word from the child left the child an orphan, delete the child. */
	if (!this->get_child(word[0])->is_end() && this->get_child(word[0])->num_children() == 0) {
		Node::remove_max_weight(this->get_child(word[0]));
		this->children.erase(word[0]);
	}

	/* Update max weights. */
	if (this->num_children() == 0) {
		if (this->is_end()) {
			Node::set_max_weight(this, this->get_weight());
		} else {
			Node::remove_max_weight(this); // Node will be removed at parent frame anyways
			delete this->get_child(word[0]);
		}
	} else {
		double best = - numeric_limits<double>::infinity();
		double temp;
		for (auto const &it : this->children) {
			temp = Node::get_max_weight(it.second);
			if (temp > best) {
				best = temp;
			}
		}

		Node::set_max_weight(this, best);
	}

	return ret;
}

/* Returns the weight associated with the word in the trie beneath this node, or -1 if the word doesn't exist. */
double Node::get_weight(const string word) const {
	/* Base case. */
	if (word.length() == 1) {
		return this->get_child(word[0])->get_weight();
	}

	/* Recursively search the child. */
	return this->get_child(word[0])->get_weight(word.substr(1));
}

/* Given a weight update function, updates the weight of the given word in the trie beneath this node. */
void Node::update_weight(const string word, double (*update_function)(double)) {
	/* Base case. */
	if (word.length() == 1) {
		double old_weight = this->get_child(word[0])->get_weight();
		this->get_child(word[0])->set_weight(update_function(old_weight));
	} else {
		this->get_child(word[0])->update_weight(word.substr(1), update_function);
	}
}

Node Node::operator =(const Node &n) {
	if (this != &n) { // Guard against self assignment.
		this->end = n.is_end();
		this->weight = n.get_weight();
		this->children = n.get_children();
	}

	return *this;
}

Node::~Node(void) { this->children.~map(); }

/* End Node class. */

ostream& operator <<(ostream &stream, const Node &n) {
	stream << "Node\n";
	
	stream << "\tEnd of word: ";
	if (n.is_end()) {
		stream << "Yes\n";
		stream << "\tWeight: " << n.get_weight();
	} else {
		stream << "No\n";
	}

	if (n.num_children()) {
		stream << "\tChildren:\n";
		for (auto it : n.get_children()) {
			stream << "\t\t" << it.first << "\n";
		}		
	}

	return stream;
}

/* Begin Trie class. */

Trie::Trie(void) { this->root = Node(false); }

bool Trie::insert(const string word) { return this->root.insert(word, 0); }

bool Trie::insert(const string word, double weight) { return this->root.insert(word, weight); }

/* Inserts words from a given file into this trie. Uses given weights if the boolean flag is true.
 * Expected format: First line contains number of words, then one word per line. If weights are 
 * included, weight of word expected on same line as word, separated by whitespace. */
void Trie::insert_from_file(const string filepath, bool has_weights /* = false */, const char *delims /* = " \n\t" */) {
	ifstream dict (filepath);
	string line; // Current line of file
	char *word, *weight_str; // Parsed word and weight
	double weight = 0.0;

	getline(dict, line); // Skip first line which contains number of words
	while (getline(dict, line)) {
		word = strtok((char *) line.c_str(), delims);

		if (has_weights) { // Retrieve the weight
			weight_str = strtok(NULL, delims);
			weight = atoi(weight_str);
		}

		this->insert(word, weight);
	}

	dict.close();
}

void Trie::insert_from_raw_text(const string filepath) {

}

bool Trie::contains(const string word) { return this->root.contains(word); }

bool Trie::remove(const string word) { return this->root.remove(word); }

double Trie::get_weight(const string word) { return this->root.get_weight(word); }

/* Returns the top k matches, ordered by weight, in this Trie which complete the given prefix. */
vector<string> Trie::autocomplete(const string prefix, int k) {
	/* First, iterate down to the node at the end of prefix. */
	Node *initial = &(this->root);
	for (int i = 0; i < prefix.length(); ++i) {
		initial = initial->get_child(prefix[i]);
	}

	/* Initialize priority queue which orders nodes by maximum weight below a given node, for use in modified Dijkstra's 
	 * algorithm below. */
	class NodeComparator {
		public:
			bool operator () (Node *n1, Node *n2) { return Node::get_max_weight(n1) < Node::get_max_weight(n2); }		
	};
	priority_queue<Node *, vector<Node *>, NodeComparator> queue;
	map<Node *, string> words;
	vector<string> ret;

	/* Main loop of the algorithm - at each iteration, pop the node with the highest maximum weight from the priority queue,
	 * and push the node's children to the priority queue. If the node is the end of a word and its max weight matches its
	 * own weight, add it to ret. */
	Node *curr;
	words[initial] = prefix;
	queue.push(initial);
	while (!queue.empty() && ret.size() < k) {
		// Pop the highest-max-weight element
		curr = queue.top();
		queue.pop();

		// If appropriate add to ret
		if (curr->is_end() && Node::get_max_weight(curr) == curr->get_weight()) {
			ret.push_back(words[curr]);
		}

		// Add children to queue
		for (auto const &it : curr->get_children()) {
			words[it.second] = words[curr] + it.first;
			queue.push(it.second);
		}
	}

	return ret;
}

/* Returns the top k matches, ordered by a combination of Levenshtein distance and word weight, which autocorrect the 
 * given word. */
vector<string> Trie::autocorrect(const string word, int max_distance) {
	int first_row[word.length() + 1];
	for (int i = 0; i < word.length() + 1; ++i) {
		first_row[i] = i;
	}

	vector<tuple<string, double, int>> suggestions;
	for (const auto &it : this->root.get_children()) {
		this->autocorrect_helper(&suggestions, word, it.second, "", it.first, first_row, max_distance);
	}

	// No need to free first_row; autocorrect_helper will free it
	return rank_suggestions(word, suggestions);
}

/* Private helper function. Given a node and the key the parent maps to the node, uses the previous row of
   the Levenshtein distance dynamic programming algorithm's table to build the current row in order tostore all
   the words in the trie whose Levensthein distance to the given (possibly misspelled) word which are within the
   specified threshold. */
void Trie::autocorrect_helper(vector<tuple<string, double, int>> *v, string word, Node *n, string curr_word, char letter, int *prev_row, int max_distance) {
	int num_columns = word.length() + 1;
	int *curr_row = new int[num_columns]; // Allocate to heap since recursion depth may be very large

	/* Build the next row of the Levenshtein distance table. The DP algorithm is based on the recurrence relation
	 *     L(i, j) = min(L(i - 1, j) + 1, L(i, j - 1) + 1, L(i - 1, j - 1) + I(s[i - 1] == t[j - 1]))
	 * where, given strings s, t, L(i, j) = Levenshtein distance between substrings s[0 : i], t[0 : j] and 
	 * I(a == b) := 0 if (a == b) and 1 if not. */
	curr_row[0] = prev_row[0] + 1;
	int min_dist = curr_row[0];
	int insert_cost, delete_cost, substitute_cost;
	for (int i = 1; i < num_columns; ++i) {
		insert_cost = curr_row[i - 1] + 1;
		delete_cost = prev_row[i] + 1;
		substitute_cost = prev_row[i - 1] + (word[i - 1] == letter ? 0 : 1);

		curr_row[i] = min(min(insert_cost, delete_cost), substitute_cost);

		if (curr_row[i] < min_dist) {
			min_dist = curr_row[i];
		}
	}

	 /* If the current node is the end of a word, and its Levensthein distance is within the threshold, add it. */
	if (n->is_end() && curr_row[num_columns - 1] <= max_distance) {
		v->push_back(make_tuple(curr_word + letter, n->get_weight(), curr_row[num_columns - 1]));
	}

	/* If there are nodes below this node with distance within the threshold, recursively add them. */
	if (min_dist <= max_distance) {
		for (const auto &it : n->get_children()) {
			autocorrect_helper(v, word, it.second, curr_word + letter, it.first, curr_row, max_distance);
		}
	}

	delete[] curr_row;
}

// TODO: When ranking, account for QWERTY keyboard proximity, so misspellings where the correct letter is close to the
// estimated correct spelling's letter on a QWERTY keyboard are ranked higher.
/* Ranks first by distances, then by weights. */
vector<string> Trie::rank_suggestions(string word, vector<tuple<string, double, int>> suggestions) {
	// First sort by Levenshtein distance, then by weight.
	map<int, map<double, vector<string>>> m;

	string word;
	double weight;
	int dist;
	for (tuple<string, double, int> t : suggestions) {
		word = get<0>(t);
		weight = get<1>(t);
		dist = get<2>(t);

		/* If m doesn't contain the key dist, initialize. */
		if (m.find(dist) == m.end()) {
			m[dist] = map<double, vector<string>>();
		}

		/* If m[dist] doesn't contain the key weight, initialize. */
		if (m[dist].find(weight) == m[dist].end()) {
			m[dist][weight] = vector<string>();			
		}

		m[dist][weight].push_back(word);
	}

	vector<string> ret;
	for (map<int, map<double, vector<string>>>::iterator it1 = m.begin(); it1 != m.end(); ++it1) {
		for (map<double, vector<string>>::iterator it2 = it1->second.begin(); it2 != it1->second.end(); ++it2) {
			for (string s : rank_suggestions_by_keyboard_proximity(word, it2->second)) {
				ret.push_back(s);				
			}
		}
	}

	return ret;
}

vector<string> Trie::rank_suggestions_by_keyboard_proximity(string word, vector<string> suggestions) {
	
}

/* End Trie class. */
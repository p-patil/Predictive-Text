#include <string>
#include <map>
#include <limits>
#include <sstream>
#include <cstdlib>
#include <exception>
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

/* Hash function for a node. */
int Node::hash(void) const { 
	stringstream s;
	for (char c = 0; c < (1 << sizeof(char)); c++) { // Iterate over all possible chars
		if (this->contains_key(c)) {
			s << ((int) c);
		}
	}

	int h;
	s >> h;

	if (this->is_end()) {
		h *= this->get_weight();
	}

	return h;
}

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

		Node::set_max_weight(this, temp);
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

bool Node::operator <(const Node &n) const { return this->hash() > n.hash(); }

Node Node::operator =(const Node &n) {
	if (this != &n) { // Guard against self assignment.
		this->end = n.is_end();
		this->weight = n.get_weight();
		this->children = n.get_children();
	}

	return *this;
}

/* End Node class. */

ostream& operator <<(ostream &stream, const Node &n) {
	stream << "Node " << n.hash() << "\n";
	
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

bool Trie::contains(const string word) { return this->root.contains(word); }

bool Trie::remove(const string word) { return this->root.remove(word); }

double Trie::get_weight(const string word) { return this->root.get_weight(word); }

/* Returns the top k matches, in order, in this Trie which complete the given prefix. */
// string * autocomplete(const string prefix, int k) {

// }

/* End Trie class. */
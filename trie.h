#ifndef TRIE_H
#define TRIE_H

#include <string>
#include <map>
#include <vector>
#include <tuple>
#include <utility>

using namespace std;

class Node {
	private:
		bool end;
		double weight;
		map<char, Node *> children;

		static map<Node *, double> max_weights; // Stores the maximum weight below a node, for each node

	public:
		// Static functions

		static double get_max_weight(Node *);

		static void set_max_weight(Node *, double);

		static void remove_max_weight(Node *);

		// Constructors

		Node(void);

		Node(bool);

		Node(bool, double);

		Node(const Node &);

		// Getters

		bool is_end(void) const;

		/* Getter function to retrieve weight at a node. */
		double get_weight(void) const;

		int num_children(void) const;

		Node * get_child(char) const;

		map<char, Node *> get_children(void) const;

		bool contains_key(char) const;

		// Setters

		void set_child(char, Node *);

		void set_end(bool);

		void set_weight(double);

		// Functionality

		bool insert(const string word, double);

		bool contains(const string word);

		bool remove(const string word);

		/* Function to recursively get weight in trie below this node. */
		double get_weight(const string word) const;

		void update_weight(const string, double (*)(double));

		// Other

		Node operator =(const Node &);

		~Node(void);
};

ostream& operator <<(ostream &, const Node &);

class Trie {
	private:
		static int levenschtein_distance(string, string);

		static void autocorrect_helper(vector<tuple<string, double, int>> *, string, Node *, string, char, int *, int);

		static vector<string> rank_suggestions(const string, vector<tuple<string, double, int>>);

		static vector<string> rank_suggestions_by_keyboard_proximity(const string, vector<string>);

		static double increment_weight(double);

	public:
		Node root; // Top of trie.
		Trie(void);

		bool insert(const string);

		bool insert(const string, double);

		void insert_from_file(const string, bool = false, const char * = " \n\t");

		void insert_from_raw_text(const string);

		bool contains(const string);

		bool remove(const string);

		double get_weight(const string);

		vector<string> autocomplete(const string, int);

		vector<string> autocorrect(const string, int);
};

#endif
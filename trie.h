#include <string>
#include <map>

using namespace std;

class Node {
	private:
		bool end;
		double weight;
		map<char, Node *> children;

		static map<Node *, double> max_weights; // Stores the maximum weight below a node, for each node

	public:
		static double get_max_weight(Node *);

		static void set_max_weight(Node *, double);

		static void remove_max_weight(Node *);

		Node(void);

		Node(bool);

		Node(bool, double);

		Node(const Node &);

		bool is_end(void) const;

		/* Getter function to retrieve weight at a node. */
		double get_weight(void) const;

		int num_children(void) const;

		Node * get_child(char) const;

		map<char, Node *> get_children(void) const;

		int hash(void) const;

		bool contains_key(char) const;

		void set_child(char, Node *);

		void set_end(bool);

		void set_weight(double);

		bool insert(const string word, double);

		bool contains(const string word);

		bool remove(const string word);

		/* Function to recursively get weight in trie below this node. */
		double get_weight(const string word) const;

		bool operator <(const Node &) const;

		Node operator =(const Node &);
};

ostream& operator <<(ostream &, const Node &);

class Trie {
	private:

	public:
		Node root; // Top of trie.
		Trie(void);

		bool insert(const string);

		bool insert(const string, double);

		bool contains(const string);

		bool remove(const string);

		double get_weight(const string);

		string * autocomplete(const string, int);

};
#ifndef NGRAM_H
#define NGRAM_H

#include <string>
#include <vector>
#include <map>

using namespace std;

class Ngram {
	private:
		int n;
		vector<string> words;

		static int string_hash(const string);

		static int ngram_hash(const Ngram);

	public:
		// Constructors

		Ngram();

		Ngram(int, vector<string>);

		// Getters

		int get_n(void) const;

		vector<string> get_words(void) const;

		// Functionality

		Ngram append(const string) const;

		// Other

		bool operator<(const Ngram &) const;
};

class NgramModel {
	private:
		int n; // The model will predict n-grams, and therefore keep track of (n - 1)-grams
		int total; // The total number of (n - 1)-grams seen so far, so that total == sum([counts[g] for g in counts])
		map<Ngram, int> counts; // The absolute frequencies of all n-grams found so far.
		map<Ngram, int> nMinusOneCounts; // The absolute frequencies of all (n - 1)-grams found so far.`

		static void update_counts(map<Ngram, int> *, map<Ngram, int> *);

		static vector<string> get_sentences(const string);

		static map<Ngram, int> * get_counts(int, const vector<string>);

	public:
		NgramModel(int);

		void initialize(const vector<string>);

		double probability(Ngram, string);

		void update_counts(Ngram);
};

#endif
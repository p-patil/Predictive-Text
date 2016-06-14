#include <string>
#include <vector>
#include <map>
#include <cstring>
#include "ngram.h"

using namespace std;

const string start_str = "<s>"; // Placeholder string representing the beginning of a sentence
const string end_str = "</s>"; // Placeholder string representing the end of a sentence

/* Begin Ngram class. */


Ngram::Ngram() {}

Ngram::Ngram(int n, vector<string> words) : n(n) { this->words = vector<string>(words); }

int Ngram::string_hash(const string s) {
	int prime = 31;
	int modulus = 1;
	
	int hash = 0;
	for (int i = 1; i < s.length(); ++i) {
		hash += modulus * s[i];
		modulus *= prime;
	}

	return hash;
}

int Ngram::ngram_hash(const Ngram gram) {
	int prime = 31;
	int hash = 1;

	for (string s : gram.get_words()) {
		hash = hash * prime + string_hash(s);
	}

	return hash;
}

int Ngram::get_n(void) const { return this->n; }

vector<string> Ngram::get_words(void) const { return this->words; }

Ngram Ngram::append(const string s) const {
	vector<string> new_words = vector<string>(this->words);
	new_words.push_back(s);
	return Ngram(this->n + 1, new_words);
}

/* Necessary to implement this operator in order to use Ngrams as keys in a map. Assumes both Ngrams share the same n. */
bool Ngram::operator<(const Ngram &gram) const { return ngram_hash(*this) > ngram_hash(gram); }

/* End Ngram class. */

/* Begin NgramModel class. */

/* Integrates the given counts map to the initial counts map. */
void NgramModel::update_counts(map<Ngram, int> *counts, map<Ngram, int> *update) {
	for (auto it = update->begin(); it != update->end(); ++it) {
		if (counts->find(it->first) == counts->end()) { // n-gram not in model, so add it
			(*counts)[it->first] = 1;
		} else {
			++(*counts)[it->first];
		}
	}
}

vector<string> NgramModel::get_sentences(const string paragraph) {
	// TODO
}

/* Given a list of sentences, returns a map (pointer, to save space) cataloging the frequency counts of each n-gram. */
map<Ngram, int> * NgramModel::get_counts(int n, const vector<string> sentences) {
	map<Ngram, int> *ret = new map<Ngram, int>();

	/* Compute counts, sentence by sentence. */
	vector<string> seq;
	Ngram gram;
	char *word;
	for (string sentence : sentences) {
		/* Add the first n-gram to the counts. */
		seq.push_back(start_str);
		word = strtok((char *) sentence.c_str(), " ;,\t");
		for (int i = 0; i < n - 1; ++i) {
			seq.push_back(word);
			word = strtok(NULL, " ;,\t");
		}

		gram = Ngram(n, seq);
		if (ret->find(gram) == ret->end()) { // n-gram not found, so initialize
			(*ret)[gram] = 1;
		} else { // Otherwise, update
			++(*ret)[gram];
		}

		/* Iterate through the sentence, adding n-grams along the way. */
		while (word != NULL) {
			word = strtok(NULL, " ;,\t");

			/* Rolling update of the current n-gram. */
			seq.erase(seq.begin()); // Erase the first element
			seq.push_back(word); // Add current word to front
			
			/* Add current n-gram to counts. */
			if (ret->find(gram) == ret->end()) {
				(*ret)[gram] = 1;
			} else {
				++(*ret)[gram];
			}
		}

		/* Add the final n-gram to counts. */
		seq.erase(seq.begin());
		seq.push_back(end_str);

		gram = Ngram(n, seq);
		if (ret->find(gram) == ret->end()) {
			(*ret)[gram] = 1;
		} else {
			++(*ret)[gram];
		}

		seq.clear();
	}

	return ret;
}


NgramModel::NgramModel(int n) : n(n), total(0) {}

// TODO: For each file, read in a certain number of lines into a buffer, cutting off at the last sentence and incorporating the beginning
//		 of the next, cut-off sentence into the next buffer, and pass the buffer into get_sentences.
/* Given a list of file paths pointing to various corpora, initializes the model by reading each file. */
void NgramModel::initialize(const vector<string> files) {
	// vector<string> sentences;
	// for (string file : files) {
	// 	sentences = get_sentences(paragraph);
	// 	update_counts(this->counts, get_counts(this->n, sentences));
	// 	update_counts(this->nMinusOneCounts, get_counts(this->n - 1, sentences));
	// }
}

/* Returns the probability that the given word will complete the given (n - 1)-gram. */
double NgramModel::probability(Ngram nMinusOneGram, string word) {
	double numerator = this->counts[nMinusOneGram.append(word)];
	double denominator = this->nMinusOneCounts[nMinusOneGram];

	return numerator / denominator;
}

/* Updates the model given a new occurence of an n-gram. */
void NgramModel::update_counts(Ngram gram) {
	// TODO
}

/* End NgramModel class. */
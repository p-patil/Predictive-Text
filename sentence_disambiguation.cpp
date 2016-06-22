#include <vector>
#include <cctype>
#include <string>
#include <cstring>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <utility>
#include <algorithm>
#include <sstream>
#include "sentence_disambiguation.h"
#include "neural_network.h"

using namespace std;

/* The algorithms used in this file are based on several research papers, referenced below.
 * 1. David Palmer and Marti Hearst. 1994. Adaptive Sentence Boundary Disambiguation. University of California, Berkeley.
 */

vector<int> brown_corpus_lookup(string pos) {
	vector<int> ret;

	// TODO This patches a weird POS tag in the Brown corpus: '' (2 apostorphes). Fix.
	if (pos == "''") {
		ret.push_back(NPC);
		return ret;
	}

	if (brown_corpus.find(pos) != brown_corpus.end()) {
		ret.push_back(brown_corpus.find(pos)->second);
	} else {
		/* Ignore hyphens */
		if (pos.find('-') != string::npos) { // pos contains hyphens
			pos = (string) strtok((char *) pos.c_str(), "-");
		}

		if (pos.find('+') != string::npos) { // pos contains +
			char *tag = strtok((char *) pos.c_str(), "+");
			while (tag != NULL) {
				ret.push_back(brown_corpus.find((string) tag)->second);
				tag = strtok(NULL, "+");
			}
		}
		
		/* Remove asterisk, unless its the only token in the string. */
		if (pos.find('*') != string::npos && pos.length() > 1) {
			pos = (string) strtok((char *) pos.c_str(), "*");
		}

		auto it = brown_corpus.find((string) strtok((char *) pos.c_str(), "-"));
		if (it != brown_corpus.end()) {
			ret.push_back(it->second);
		} else {
			// cerr << "Error: Tag '" << pos << "' not found" << endl;
		}
	}

	return ret;
}

bool is_whitespace(const string s) {
	bool whitespace_flag = true;

	for (int i = 0; i < s.length(); ++i) {
		if (!isspace(s[i])) {
			whitespace_flag = false;
		}
	}

	return whitespace_flag;
}

vector<string> split_by_space(const string s) {
	vector<string> ret;

	char *token = strtok((char *) s.c_str(), " \t");
	do {
		ret.push_back(token);
	} while ((token = strtok(NULL, " \t")) != NULL);

	return ret;
}

pair<string, string> parse_brown_corpus_token(const string token) {
	if (count(token.begin(), token.end(), '/') > 1) {
		/* If token contains multiple forward slashes, define the word to be the substring up to the last slash. */
		string word;
		char *prev = strtok((char *) token.c_str(), "/"), *curr = strtok(NULL, "/");

		while (curr != NULL) {
			word += prev;
			prev = curr;
			curr = strtok(NULL, "/");			
		}

		return make_pair(word, (string) prev);
	} else { // count(token.begin(), token.end(), '/') == 1
		string word = (string) strtok((char *) token.c_str(), "/");
		string pos = (string) strtok(NULL, "/");
		return make_pair(word, pos);
	}
}

/* Begin PartOfSpeechTagger class. */

PartOfSpeechTagger::PartOfSpeechTagger() {}

bool PartOfSpeechTagger::is_dir(const string filepath) {
	struct stat s;
	if (stat(filepath.c_str(), &s) == 0) {
		if (s.st_mode & S_IFDIR) { // filepath is directory
			return true;
		} else if (s.st_mode & S_IFREG) { // filepath is a regular file
			return false;
		} else {
			cout << "Found non-regular file at '" << filepath << "'\n";
		}
	} else { // File error
		cout << "File error when trying to read '" << filepath << "'\n";
	}
}

void PartOfSpeechTagger::update_pos_count(const string word, int pos) {
	if (this->pos_counts.find(word) == this->pos_counts.end()) { // Word not found
		int *descriptor_array = new int[POS_LEN];
		for (int i = 0; i < POS_LEN; ++i) {
			descriptor_array[i] = 0;
		}
		descriptor_array[pos] = 1;

		this->pos_counts[word] = descriptor_array;
	}

	++this->pos_counts[word][pos];
}

void PartOfSpeechTagger::update_pos_count(const string word, vector<int> pos_list) {
	for (int pos : pos_list) {
		this->update_pos_count(word, pos);
	}
}
	
/* Given a string, returns a list of tokens. A token is defined as a sequence of alphabetic characters, a sequence of digits
 * (including at most one decimal point), or a single non-alphanumeric character. */
vector<string> PartOfSpeechTagger::tokenize(const string s) {
	vector<string> ret;

	bool decimal_point_flag = false;
	int start, len;
	for (int i = 0; i < s.length(); ) {
		if (isalpha(s[i])) {
			/* Get starting and ending indices of the longest continuous alphabetic string beginning at index i. */
			start = i;
			while (isalpha(s[++i]));
			len = i - start;

			ret.push_back(s.substr(start, len));
		} else if (isdigit(s[i]) || s[i] == '-') {
			/* Account for negative numbers by checking if s[i] is a hyphen or minus sign. */
			if (s[i] == '-' && !isdigit(s[i + 1])) {
				ret.push_back(s.substr(i++, 1));
				continue;
			}

			/* Get starting and ending indices of the longest continuous numeric string beginning at index i, 
			 * allowing at most one decimal point. */
			start = i;
			while (isdigit(s[++i]) || s[i] == '.') {
				if (s[i] == '.') {
					if (decimal_point_flag) {
						break;
					} else {
						decimal_point_flag = true;
					}
				}
			}
			len = i - start;

			decimal_point_flag = false;
			ret.push_back(s.substr(start, len));

		} else {
			ret.push_back(s.substr(i++, 1));
		}
	}

	return ret;
}

/* Returns an array mapping each part-of-speech in the POS enum to the proportion of times the given word takes on that part-of-speech,
 * out of all appearances of that word in the corpora seen by this tagger thus far. */
vector<double> PartOfSpeechTagger::pos_frequencies(const string word) {
	vector<double> ret;
	if (this->pos_counts.find(word) == this->pos_counts.end()) { // word hasn't been seen
		return ret;
	}

	int *counts = this->pos_counts[word];
	double total = 0.0;
	for (int i = 0; i < POS_LEN; ++i) {
		total += counts[i];
	}

	/* Normalize. */
	for (int i = 0; i < POS_LEN; ++i) {
		ret.push_back(counts[i] / total);
	}

	return ret;
}

/* Scans the given directory (recursively scanning sub-directories) for text files belonging to the Brown corpus. */
void PartOfSpeechTagger::read_brown_corpus(const string filepath) {
	if (!is_dir(filepath)) {
		ifstream file (filepath);
		string line;

		/* Iterate over every line in the file, updating counts along the way. */
		vector<int> parts_of_speech;
		vector<string> tokens;
		pair<string, string> p;
		string word, pos;
		while (getline(file, line)) {
			/* If the string contains only whitespace, skip it. */
			if (is_whitespace(line)) {
				continue;
			}

			/* First break the line into tokens. */
			tokens = split_by_space(line);

			/* Then break each token into a (word, part-of-speech) pair. */
			for (string token : tokens) {
				/* Decompose the token into a word and its corresponding part-of-speech, as per the Brown corpus' syntax. */
				p = parse_brown_corpus_token(token);
				word = get<0>(p), pos = get<1>(p);

				// TODO pos contains a grave accent (ascii 96). Weird. Ignore for now.
				if (pos.find('`') != string::npos) {
					continue;
				}

				/* Get all parts of speech that apply to the word. */
				parts_of_speech = brown_corpus_lookup(pos);

				/*  Update counts accordingly. */
				this->update_pos_count(word, parts_of_speech);
			}
		}

		file.close();
	} else { // File is directory, so recursively search each sub-file
		DIR *root = opendir(filepath.c_str());
		struct dirent *file;

		while ((file = readdir(root)) != NULL) {
			if (file->d_name[0] == '.') { // // Skip current and parent directories
				continue;
			}

			this->read_brown_corpus(filepath + "/" + file->d_name);
		}

		closedir(root);
	}
}

PartOfSpeechTagger::~PartOfSpeechTagger(void) {
	for (auto const &it : this->pos_counts) {
		delete[] it.second;
	}
}

/* End PartOfSpeechTagger class. */

/* Begin Sentence class. */

vector<string> Sentence::get_sentences(const string paragraph, NeuralNetwork net) {

}

// NeuralNetwork Sentence::train_net(TRAINING DATA) {
// 	int length = 3;
// 	int *layer_counts = new int[length];
// 	layer_counts[0] = k * descriptor_length; // Size of input layer
// 	layer_counts[1] = 10; // Size of hidden layer
// 	layer_counts[2] = 1; // Size of output layer

// 	NeuralNetwork ret (layer_counts, length);
// }

/* End Sentence class. */
#ifndef SENTENCE_DISAMBIGUATION_H
#define SENTENCE_DISAMBIGUATION_H

#include <vector>

using namespace std;

class Tokenizer {
	private:
		static bool is_non_alphanumeric(char);

	public:

		static vector<string> tokenize(string);
};

class PartOfSpeechTagger {
	
};

#endif
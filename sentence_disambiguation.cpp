#include <vector>
#include <cctype>
#include <string>
#include <iostream>
#include "sentence_disambiguation.h"

using namespace std;

/* The algorithms used in this file are based on several research papers, referenced below.
 * 1. David Palmer and Marti Hearst. 1994. Adaptive Sentence Boundary Disambiguation. University of California, Berkeley.
 */

/* Begin Tokenizer class. */

bool Tokenizer::is_non_alphanumeric(char c) { return !(isalpha(c) || isdigit(c)); }
	
/* Given a string, returns a list of tokens. A token is defined as a sequence of alphabetic characters, a sequence of digits
 * (including at most one decimal point), or a single non-alphanumeric character. */
vector<string> Tokenizer::tokenize(string s) {
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

/* End Tokenizer class. */
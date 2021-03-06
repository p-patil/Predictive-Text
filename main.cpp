#include <string>
#include <iostream>
#include <utility>
#include <cstring>
#include <dirent.h>
#include <utility>
#include <cmath>
#include <random>
#include <algorithm>
#include <functional>
#include "trie.h"
#include "ngram.h"
#include "sentence_disambiguation.h"
#include "neural_network.h"

using namespace std;

#define PI 3.141592653589793238462643383279502884197169399375105820974

/* TODO:
	1. n-gram prediction
	2. interface (this will be useful in testing)
	3. memory efficient DAWG
	4. weight adjustment - with each correct or incorrect prediction, adjust word weights or something
	5. grammar checking, and incorporate a partially typed sentence's grammatical structure when predicting
			- Learn over time if a user is using a word in a grammatical form different from its definition
			 (eg I'll Facebook you; "Facebook" is a noun from a dictionary definition, but the user uses it as a verb)
	6. autocorrecting multiple words - account for word concatenation (eg predicting "is below" for "isbeliw")
	7. context - use the content of a partially typed sentence as context when predicting the next word
			- eg Don't correct "I'll see you thier" to "I'll see you their", which has may have lower edit distance and higher weight, but
				 rather correct to "I'll see you there" which makes more grammatical and logical sense
			- Copy SwiftKey, which uses a neural network to analyze context. This is much stronger than the n-gram model, eg on the sentence
				"We're going in the right", a bigram model predicts the next word to be "to" whereas SwiftKey's neural network predicts 
				"direction"
					- http://gizmodo.com/swiftkey-has-a-neural-network-keyboard-and-its-creepily-1735430695
					- https://blog.swiftkey.com/neural-networks-a-meaningful-leap-for-mobile-typing/
  Neural network TODO:
    - Parallelize, and look into GPU programming
    - Incorporate momentum, adaptive learning rate, weight decay
        - Look into bold driver algorithm for momentum update, local learning rates
    - Learning optimal network topology
    - Look into hyperparameter turning (eg grid search)
    - Deep learning?

*/

int main(int argc, char **argv) {
	// string dictionary_path = "../data/Default_dictionary.txt";
	// string wiktionary_path = "../data/wiktionary.txt";

	// Trie t;

	// cout << "Reading dictionary... ";
	// t.insert_from_file(wiktionary_path, true);
	// cout << "Done." << endl;

	// cout << "Suggestions: " << endl;
	// for (string s : t.autocorrect("mottorc", 2)) {
	// 	cout << "\t" << s << endl;
	// 	for (string s2 : t.autocomplete(s, 10)) {
	// 		cout << "\t\t" << s2 << endl;
	// 	}
	// }

    srand(time(NULL));
	int sample_size = 1000;
	vector<pair<ARRAY, ARRAY>> samples (sample_size);

	double x, y;
	for (int i = 0; i < sample_size; ++i) {
		x = ((double) rand()) / RAND_MAX;
		y = sin(x);

		ARRAY x_arr, y_arr;
		x_arr.push_back(x);
		y_arr.push_back(y);

		samples[i] = make_pair(x_arr, y_arr);
	}

	int layer_counts[] = {1, 3, 1};
	NeuralNetwork net (layer_counts, 3);
	net.train(samples, 10);

	int test_size = 10;
	double yhat;
	for (int i = 0; i < test_size; ++i) {
		x = ((double) rand()) / RAND_MAX;
		y = sin(x);

		yhat = net.feedforward(ARRAY(1, x))[0];
        cout << "Error: " << abs(y - yhat) << " - expected " << y << ", got " << yhat << endl;
	}
	
	return 0;
}

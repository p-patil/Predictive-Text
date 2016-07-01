#ifndef SENTENCE_DISAMBIGUATION_H
#define SENTENCE_DISAMBIGUATION_H

#include <vector>
#include <map>
#include <utility>

#include "neural_network.h"

using namespace std;

/* Enumerates the part-of-speech tags used. */
enum POS {
	N,		// noun
	CON,	// conjunction
	NUM,	// number
	ADJ, 	// adjective
	DET, 	// determiner
	NPC,	// non-punctuation character
	SEP,	// sentence-ending punctuation
	V,		// verb
	PN,		// pronoun
	CS,		// comma or semicolon
	POSS,	// possessive
	O,		// others
	ART,	// article
	PREP,	// preposition
	LPAR,	// left parentheses
	CDSH,	// colon or dash
	MOD,	// modifier
	PROP,	// proper noun
	RPAR,	// right parentheses
	ABB,	// abbreviation
	POS_LEN
};

/* Maps the full set of part-of-speech tags used in the Brown corpus to the smaller set of tags used here. */
const map<string, int> brown_corpus = {
	{"'", NPC},		// apostorphe
	{".", SEP}, 	// sentence (. ; ? *)
	{"(", LPAR}, 	// left paren
	{")", RPAR}, 	// right paren
	{"*", MOD}, 	// not, n't
	{"--", CDSH}, 	// dash
	{",", CS}, 		// comma
	{":", CDSH}, 	// colon
	{"abl", MOD}, 	// pre-qualifier (quite, rather)
	{"abn", MOD}, 	// pre-quantifier (half, all)
	{"ap$", DET}, 	// possessive post-determiner
	{"abx", MOD}, 	// pre-quantifier (both)
	{"ap", DET}, 	// post-determiner (many, several, next)
	{"at", ART}, 	// article (a, the, no)
	{"be", V}, 		// be
	{"bed", V}, 	// were
	{"bedz", V}, 	// was
	{"beg", V}, 	// being
	{"bem", V}, 	// am
	{"ben", V}, 	// been
	{"ber", V}, 	// are, art
	{"bez", V}, 	// is
	{"cc", CON}, 	// coordinating conjunction (and, or)
	{"cd", NUM}, 	// cardinal numeral (one, two, 2, etc.)
	{"cd$", NUM}, 	// possessive cardinal numeral
	{"cs", CON}, 	// subordinating conjunction (if, although)
	{"do", V}, 		// do
	{"dod", V}, 	// did
	{"doz", V}, 	// does
	{"dt", DET}, 	// singular determiner/quantifier (this, that)
	{"dt$", DET},	// possessive singular determiner/quantifer
	{"dti", DET}, 	// singular or plural determiner/quantifier (some, any)
	{"dts", DET}, 	// plural determiner (these, those)
	{"dtx", CON}, 	// determiner/double conjunction (either)
	{"ex", PN}, 	// existential there
	{"fw", O}, 		// foreign word (hyphenated before regular tag)
	{"hv", V}, 		// have
	{"hvd", V}, 	// had
	{"hvg", V}, 	// having
	{"hvn", V}, 	// had (past participle)
	{"hvz", V}, 	// has
	{"in", PREP}, 	// preposition
	{"jj", ADJ}, 	// adjective
	{"jjr", ADJ}, 	// comparative adjective
	{"jjs", ADJ}, 	// semantically superlative adjective
	{"jj$", ADJ},	// possessive adjective
	{"jjt", ADJ}, 	// morphologically superlative adjective
	{"md", V}, 		// modal auxiliary (can, should, will)
	{"nc", O}, 		// cited word (hyphenated after regular tag)
	{"nn", N}, 		//  singular or mass noun
	{"nn$", POSS}, 	// possessive singular noun
	{"nns", N}, 	// plural noun
	{"nns$", POSS}, // possessive plural noun
	{"np", PROP}, 	// proper noun or part of name phrase
	{"nps", PN},	// plural proper noun
	{"nps$", PN}, 	// possessive plural proper noun
	{"np$", PROP}, 	// possessive proper noun
	{"nr", N}, 		// abbrevial noun (home, today, west)
	{"nrs", N}, 	// plural abbrevial noun
	{"nr$", N},	 	// plural abbrevial noun
	{"od", NUM}, 	// ordinal numeral (first, 2nd)
	{"pn", PN}, 	// nominal pronoun (everybody, nothing)
	{"pn$", PN}, 	// possessive nominal pronoun
	{"pp$", PN}, 	// possessive personal pronoun
	{"pp$$", PN}, 	// second (nominal) possessive pronoun (mine, ours)
	{"ppl", PN}, 	// singular reflexive/intensive personal pronoun (myself)
	{"ppls", PN}, 	// plural reflexive/intensive personal pronoun (ourselves
	{"ppo", PN}, 	// objective personal pronoun (me, him, it, them)
	{"pps", PN}, 	// 3rd. singular nominative pronoun (he, she, it, one)
	{"ppss", PN}, 	// other nominative personal pronoun (I, we, they, you)
	{"prp", PN}, 	// personal pronoun
	{"prp$", PN}, 	// possessive pronoun
	{"ql", MOD}, 	// qualifier (very, fairly)
	{"qlp", MOD}, 	// post-qualifier (enough, indeed)
	{"rb", MOD}, 	// adverb
	{"rbr", MOD}, 	// comparative adverb
	{"rb$", MOD}, 	// possessive adverb
	{"rbt", MOD}, 	// superlative adverb
	{"rn", MOD}, 	// nominal adverb (here, then, indoors)
	{"rp", MOD}, 	// adverb/particle (about, off, up)
	{"to", CON}, 	// infinitive marker to
	{"uh", O}, 		// interjection, exclamation
	{"vb", V}, 		// verb, base form
	{"vbd", V}, 	// verb, past tense
	{"vbg", V}, 	// verb, present participle/gerund
	{"vbn", V}, 	// verb, past participle
	{"vbp", V}, 	// verb, non 3rd person, singular, present
	{"vbz", V}, 	// verb, 3rd. singular present
	{"wdt", DET}, 	// wh- determiner (what, which)
	{"wp$", PN}, 	// possessive wh- pronoun (whose)
	{"wpo", PN}, 	// objective wh- pronoun (whom, which, that)
	{"wps", PN}, 	// nominative wh- pronoun (who, which, that)
	{"wql", MOD}, 	// wh- qualifier (how)
	{"wrb", MOD} 	// wh- adverb (how, where, when)
};

/* Utility functions. */

vector<int> brown_corpus_lookup(string);

bool is_whitespace(const string);

vector<string> split_by_space(const string);

pair<string, string> parse_brown_corpus_token(const string);

class PartOfSpeechTagger {
	private:

		void update_pos_count(const string, int);

		void update_pos_count(const string, vector<int>);

		static bool is_alphanumeric(char);

		static vector<string> tokenize(const string);

		static bool is_dir(const string);


	public:
		map<const string, int *> pos_counts;
		// Constructors

		PartOfSpeechTagger();

		// Functionality

		vector<double> pos_frequencies(const string);

		// Training

		void read_brown_corpus(const string);

		// Other

		~PartOfSpeechTagger(void);
};

class Sentence {
	private:
		static const int k = 3; // k-context - the number of words of context surrounding an end-of-sentence punctuation mark to use
		static const int descriptor_length = POS_LEN; // Length of descriptor array

		static constexpr double lower_threshold = 0.2; // If output of the neural net < lower_threshold then punctuation is not sentence boundary
		static constexpr double upper_threshold = 0.7; // If output of the neural net > upper_threshold then punctuation is sentence boundary

	public:
		static vector<string> get_sentences(const string, NeuralNetwork);

		// static NeuralNetwork train_net(TRAINING DATA);
};

#endif

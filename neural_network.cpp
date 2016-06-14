#include <random>
#include <cmath>
#include <iterator>
#include "neural_network.h"

using namespace std;

/* Begin Neuron class. */

/* Sigmoid activation function: f(x) = 1 / (1 + e^(- lambda * x)) */
// double Neuron::activation_function(double x, double lambda) { return 1 / (1 + exp(-x * lambda)); }
double Neuron::activation_function(double x, double lambda) { return x; }

Neuron::Neuron() {}

/* Copy constructor. */
Neuron::Neuron(const Neuron &n) { *this = n; }

/* Basic constructor; randomly initializes weights. */
Neuron::Neuron(Neuron **synapses, int length) {
	this->num_synapses = length;
	this->synapses = new Neuron * [length];
	this->weights = new double[length];

	default_random_engine generator;
	uniform_real_distribution<double> distribution(-1.0, 1.0);
	
	for (int i = 0; i < length; ++i) {
		this->synapses[i] = synapses[i];
		this->weights[i] = distribution(generator);
	}
}

Neuron::Neuron(Neuron **synapses, double *weights, int length) {
	this->num_synapses = length;
	this->synapses = new Neuron * [length];
	this->weights = new double[length];
	
	for (int i = 0; i < length; ++i) {
		this->synapses[i] = synapses[i];
		this->weights[i] = weights[i];
	}
}

Neuron ** Neuron::get_synapses(void) const { return this->synapses; }

double * Neuron::get_weights(void) const { return this->weights; }

double Neuron::get_weight(int index) const { return this->weights[index]; }

int Neuron::get_num_synapses(void) const { return this->num_synapses; }

/* Returns array allocated to heap with new[]. */
double * Neuron::output(double input) const {
	double *output_vector = new double[this->num_synapses];

	for (int i = 0; i < this->num_synapses; ++i) {
		output_vector[i] = activation_function(input * this->weights[i], 1);
	}

	return output_vector;
}

bool Neuron::is_output_neuron(void) const { return false; }

/* Assignment operator. */
Neuron & Neuron::operator =(const Neuron & n) {
	this->num_synapses = n.get_num_synapses();

	delete[] this->synapses;
	delete[] this->weights;

	this->synapses = new Neuron *[this->num_synapses];
	this->weights = new double[this->num_synapses];

	Neuron **synapses = n.get_synapses();
	double *weights = n.get_weights();
	for (int i = 0; i < this->num_synapses; ++i) {
		this->synapses[i] = synapses[i];
		this->weights[i] = weights[i];
	}
}

bool Neuron::operator ==(const Neuron &n) {
	if (this->num_synapses != n.get_num_synapses()) {
		return false;
	}

	double *weights = n.get_weights();
	for (int i = 0; i < this->num_synapses; ++i) {
		if (this->weights[i] != weights[i]) {
			return false;
		}
	}

	return true;
}

/* Destructor. */
Neuron::~Neuron(void) {
	delete[] this->synapses;
	delete[] this->weights;
}

/* End Neuron class. */

/* Begin BiasNeuron class. */

BiasNeuron::BiasNeuron(Neuron **synapses, int length) : Neuron(synapses, length) {}

BiasNeuron::BiasNeuron(Neuron **synapses, double *weights, int length) : Neuron(synapses, weights, length) {}

double * BiasNeuron::output(double input) const {
	double *output_vector = new double[this->num_synapses];

	for (int i = 0; i < this->num_synapses; ++i) {
		output_vector[i] = -1.0;
	}

	return output_vector;
}

/* End BiasNeuron class. */

/* Begin OutputNeuron class. */

OutputNeuron::OutputNeuron() {}

double * OutputNeuron::output(double input) const { 
	double *output_vector = new double[1];
	output_vector[0] = Neuron::activation_function(input, 1);

	return output_vector;
}

bool OutputNeuron::is_output_neuron(void) const { return true; }

bool OutputNeuron::operator ==(const Neuron &n) { return n.is_output_neuron(); }

/* End OutputNeuron class. */

/* Begin NeuralNetwork class. */

/* Initializes a neural network, with random weights, whose i-th layer has layer_counts[i] neurons. */
NeuralNetwork::NeuralNetwork(int *layer_counts, int length) {
	/* Initialize. */
	this->num_layers = length;
	this->layer_counts = new int[length];
	this->layers = new Neuron **[length];
	this->bias_neurons = new BiasNeuron *[length - 1];

	/* Copy layer counts over. */
	for (int i = 0; i < length; ++i) {
		this->layer_counts[i] = layer_counts[i];
	}

	/* Construct layers in backwards order, connecting synapses along the way. Start with output layer. */
	this->layers[length - 1] = new Neuron *[layer_counts[length - 1]];
	for (int i = 0; i < layer_counts[length - 1]; ++i) {
		this->layers[length - 1][i] = new OutputNeuron();
	}

	for (int i = length - 2; i >= 0; --i) {
		this->layers[i] = new Neuron *[layer_counts[i]];
		for (int j = 0; j < layer_counts[i]; ++j) {
			this->layers[i][j] = new Neuron(this->layers[i + 1], layer_counts[i + 1]);
		}

		this->bias_neurons[i] = new BiasNeuron(this->layers[i + 1], layer_counts[i + 1]);
	}
}

/* Initializes a neural network with the given weights whose i-th layer has layerCounts[i] neurons. The weights matrix
 * should have (length - 1) rows. */
NeuralNetwork::NeuralNetwork(int *layer_counts, double ***weights, double **bias_weights, int length) {
	/* Initialize. */
	this->num_layers = length;
	this->layer_counts = new int[length];
	this->layers = new Neuron **[length];
	this->bias_neurons = new BiasNeuron *[length - 1];

	/* Copy layer counts over. */
	for (int i = 0; i < length; ++i) {
		this->layer_counts[i] = layer_counts[i];
	}

	/* Construct layers in backwards order, connecting synapses along the way. Start with output layer. */
	this->layers[length - 1] = new Neuron *[layer_counts[length - 1]];
	for (int i = 0; i < layer_counts[length - 1]; ++i) {
		this->layers[length - 1][i] = new OutputNeuron();
	}

	for (int i = length - 2; i >= 0; --i) {
		this->layers[i] = new Neuron *[layer_counts[i]];
		for (int j = 0; j < layer_counts[i]; ++j) {
			this->layers[i][j] = new Neuron(this->layers[i + 1], weights[i][j], layer_counts[i + 1]);
		}

		this->bias_neurons[i] = new BiasNeuron(this->layers[i + 1], bias_weights[i], layer_counts[i + 1]);
	}
}

int NeuralNetwork::get_num_layers(void) const { return this->num_layers; }

int NeuralNetwork::get_layer_count(int index) const { return this->layer_counts[index]; }

/* Returned array was allocated to heap with new[]. */
int * NeuralNetwork::get_layer_counts(void) const {
	int *ret = new int[this->get_num_layers()];
	for (int i = 0; i < this->get_num_layers(); ++i) {
		ret[i] = this->get_layer_count(i);
	}

	return ret;
}

Neuron * NeuralNetwork::get_neuron(int i, int j) const { return this->layers[i][j]; }

BiasNeuron * NeuralNetwork::get_bias_neuron(int i) const { return this->bias_neurons[i]; };

/* Run the neural network through its layers on the given inputs. Returned array was allocated to heap with new[]. */
double * NeuralNetwork::feedforward(double *inputs, int length) const {
	double *curr_layer_inputs, *next_layer_inputs, *bias_output, *neuron_output;

	/* Initialize by copying the inputs over, since they are the data corresponding to the first layer. */
	curr_layer_inputs = new double[length];
	for (int i = 0; i < length; ++i) {
		curr_layer_inputs[i] = inputs[i];
	}

	/* Feed the data in, at each layer computing the inputs to the next layer. */
	for (int i = 0; i < this->get_num_layers() - 1; ++i) {
		/* For each neuron in the current layer, increment the input vector for neurons in the next layer with the output 
		 * of the current neuron. */
		next_layer_inputs = new double[this->get_layer_count(i + 1)];
		for (int j = 0; j < this->get_layer_count(i + 1); ++j) {
			next_layer_inputs[j] = 0.0;
		}

		for (int j = 0; j < this->get_layer_count(i); ++j) {
			neuron_output = this->get_neuron(i, j)->output(curr_layer_inputs[j]); // Get the output vector for this neuron.

			for (int k = 0; k < this->get_layer_count(i + 1); ++k) {
				next_layer_inputs[k] += neuron_output[k]; // Increment the weighted sum of inputs for the neurons in the next layer.
			}
		}

		/* Now account for this layer's bias neuron. */
		bias_output = this->get_bias_neuron(i)->output(0); // Value passed to output is irrelevant
		for (int j = 0; j < this->get_layer_count(i); ++j) {
			next_layer_inputs[j] += bias_output[j] * this->get_bias_neuron(i)->get_weight(j);
		}

		/* In the next iteration, the newly constructed next_layer_inputs will be used as the current inputs, so copy over. */
		delete[] curr_layer_inputs;
		curr_layer_inputs = new double[this->get_layer_count(i + 1)];
		for (int j = 0; j < this->get_layer_count(i + 1); ++j) {
			curr_layer_inputs[j] = next_layer_inputs[j];
		}
		delete[] next_layer_inputs;
	}

	return curr_layer_inputs;
}

/* Destructor. */
NeuralNetwork::~NeuralNetwork(void) {
	for (int i = 0; i < this->get_num_layers(); ++i) {
		for (int j = 0; j < this->get_layer_count(i); ++j) {
			delete this->layers[i][j];
		}

		delete[] this->layers[i];
	}

	for (int i = 0; i < this->get_num_layers() - 1; ++i) {
		delete this->bias_neurons[i];
	}

	delete[] this->bias_neurons;
	delete[] this->layer_counts;
}

ostream& operator <<(ostream &stream, const NeuralNetwork &network) {
	for (int i = 0; i < network.get_num_layers() - 1; i++) {
		if (i == 0) {
			stream << "INPUT LAYER\n";
		} else {
			stream << "LAYER " << i << endl;
		}

		stream << "\tBias Neuron - Weights:\n";
		for (int j = 0; j < network.get_layer_count(i + 1); j++) {
			stream << "\t\tNeuron " << j + ": " << network.get_bias_neuron(i)->get_weight(j) << endl;
		}

		for (int j = 0; j <  network.get_layer_count(i); j++) {
			stream << "\tNeuron " << j << " - Weights:\n";
			for (int k = 0; k < network.get_layer_count(i + 1); k++) {
				stream << "\t\tNeuron " << k << ": " << network.get_neuron(i, j)->get_weight(k) << endl;
			}
		}
	}

	stream << "OUTPUT LAYER\n";
	for (int j = 0; j < network.get_layer_count(network.get_num_layers() - 1); j++) {
		stream << "\tOutput Neuron " << j;
	}

	return stream;
}

/* End NeuralNetwork class. */
#include <random>
#include <cmath>
#include <iterator>
#include <algorithm>
#include "neural_network.h"

using namespace std;

/* Begin Neuron class. */

/* Sigmoid activation function: f(x) = 1 / (1 + e^(- lambda * x)) */

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

double Neuron::get_weight(int i) const { return this->weights[i]; }

void Neuron::set_weight(int i, double val) { this->weights[i] = val; }

int Neuron::get_num_synapses(void) const { return this->num_synapses; }

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

/* End BiasNeuron class. */

/* Begin OutputNeuron class. */

OutputNeuron::OutputNeuron() {}

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

void NeuralNetwork::set_zero(ARRAY_2D &arr) {
	for (int i = 0; i < arr.size(); ++i) {
		for (int j = 0; j < arr[i].size(); ++j) {
			arr[i][j] = 0.0;
		}
	}
}

void NeuralNetwork::set_zero(ARRAY_3D &arr) {
	for (int i = 0; i < arr.size(); ++i) {
		for (int j = 0; j < arr[i].size(); ++j) {
			for (int k = 0; k < arr[i][j].size(); ++k) {
				arr[i][j][k] = 0.0;
			}
		}
	}
}

void NeuralNetwork::add_arrays(ARRAY_2D &arr, const ARRAY_2D &summand) {
	for (int i = 0; i < arr.size(); ++i) {
		for (int j = 0; j < arr[i].size(); ++j) {
			arr[i][j] += summand[i][j];
		}
	}
}

void NeuralNetwork::add_arrays(ARRAY_3D &arr, const ARRAY_3D &summand) {
	for (int i = 0; i < arr.size(); ++i) {
		for (int j = 0; j < arr[i].size(); ++j) {
			for (int k = 0; k < arr[i][j].size(); ++k) {
				arr[i][j][k] += summand[i][j][k];
			}
		}
	}
}

/* Sigmoid activation function. */
double NeuralNetwork::activation_function(double x) { return 1 / (1 + exp(-x)); }

/* Derivative of the activation function given an input. */
double NeuralNetwork::activation_function_derivative_input(double x) {
	double y = NeuralNetwork::activation_function(x);
	return y * (1 - y);
}

/* Derivative of the activation function given an output. */
double NeuralNetwork::activation_function_derivative_output(double y) { return y * (1 - y); }

/* Error function used in training: error(x, y) = 1/2 * (mag(x - y)^2) for vectors x, y. */
double NeuralNetwork::error_function(const ARRAY &output, const ARRAY &expected, int length) {
	double squared_magnitude = 0.0, error;
	for (int i = 0; i < length; ++i) {
		error = output[i] - expected[i];
		squared_magnitude += error * error;
	}

	return squared_magnitude / 2;
}

/* Given an actual output and expected output, returns all the deltas (one per non-output neuron) according to the delta rule. */
ARRAY_2D NeuralNetwork::backpropagate(const ARRAY &output, const ARRAY &expected, const ARRAY_2D &neuron_outputs) const {
	int length = this->get_num_layers() - 1;
	ARRAY_2D deltas (length);

	/* Initialize data for output layer. */
	deltas[length - 1] = ARRAY(this->get_layer_count(this->get_num_layers() - 1));
	for (int i = 0; i < this->get_layer_count(this->get_num_layers() - 1); ++i) {
		deltas[length - 1][i] = activation_function_derivative_output(output[i]) * (output[i] - expected[i]);
	}

	/* Backwards propagate to recursively compute each delta:
	 * delta[i][j] = (1 - o_{i, j}) * o_{i, j} * (sum over neurons n in next layer (delta_{i + 1, n} * w))
	 * where w = weight between neuron j in layer i and neuron n in layer i + 1. */
	for (int i = length - 2; i >= 1; --i) {
		deltas[i] = ARRAY(this->get_layer_count(i));

		for (int j = 0; j < this->get_layer_count(i); ++j) {
			deltas[i][j] = 0.0;

			for (int k = 0; k < this->get_layer_count(i + 1); ++k) {
				deltas[i][j] += deltas[i + 1][k] * this->get_weight(i, j, k);
			}

			deltas[i][j] *= NeuralNetwork::activation_function_derivative_output(neuron_outputs[i][j]);
		}
	}

	return deltas;
}

ARRAY_3D NeuralNetwork::compute_derivatives(const ARRAY_2D &outputs, const ARRAY_2D &deltas) {
	ARRAY_3D derivatives (this->get_num_layers() - 1);

	for (int i = 0; i < this->get_num_layers() - 1; ++i) {
		derivatives[i] = ARRAY_2D(this->get_layer_count(i));

		for (int j = 0; j < this->get_layer_count(i); ++j) {
			derivatives[i][j] = ARRAY(this->get_layer_count(i + 1));

			for (int k = 0; i < this->get_layer_count(i + 1); ++k) {
				derivatives[i][j][k] = deltas[i + 1][k] * outputs[i][j];
			}
		}
	}

	return derivatives;
}

ARRAY_2D NeuralNetwork::compute_bias_derivatives(const ARRAY_2D &deltas) {
	ARRAY_2D bias_derivatives (this->get_num_layers() - 1);

	for (int i = 0; i < this->get_num_layers() - 1; ++i) {
		bias_derivatives[i] = ARRAY (this->get_layer_count(i + 1));

		for (int j = 0; j < this->get_layer_count(i + 1); ++j) {
			bias_derivatives[i][j] = deltas[i + 1][j]; // output of bias neurons is always 1
		}
	}

	return bias_derivatives;
}

/* Updates weights by shifting in the direction opposite the average error function gradient. */
void NeuralNetwork::update_weights(const ARRAY_3D &gradient, double learning_rate) {
	double weight, new_weight;
	for (int i = 0; i < this->get_num_layers() - 1; ++i) {
		for (int j = 0;  j < this->get_layer_count(i); ++j) {
			for (int k = 0; k < this->get_layer_count(i + 1); ++k) {
				weight = this->get_weight(i, j, k);
				new_weight = weight - learning_rate * gradient[i][j][k];

				this->set_weight(i, j, k, new_weight);
			}
		}
	}
}

void NeuralNetwork::update_bias_weights(const ARRAY_2D &bias_gradient, double learning_rate) {
	double weight, new_weight;
	for (int i = 0; i < this->get_num_layers() - 1; ++i) {
		for (int j = 0;  j < this->get_layer_count(i + 1); ++j) {
			weight = this->get_bias_weight(i, j);
			new_weight = weight - learning_rate * bias_gradient[i][j];

			this->set_bias_weight(i, j, new_weight);
		}
	}
}

// void NeuralNetwork::gradient_descent(vector<pair<const ARRAY &, const ARRAY &>> samples) {
void NeuralNetwork::gradient_descent(const vector<pair<ARRAY, ARRAY>> &samples) {
	random_shuffle(samples.begin(), samples.end()); // Shuffle so batches are random

	/* Initialize array to keep track of sum of all error function gradients per batch. */
	ARRAY_3D derivatives_sums = ARRAY_3D(this->get_num_layers() - 1);
	ARRAY_2D bias_derivatives_sums = ARRAY_2D(this->get_num_layers() - 1);
	for (int i = 0; i < this->get_num_layers() - 1; ++i) {
		derivatives_sums[i] = ARRAY_2D(this->get_layer_count(i));

		for (int j = 0; j < this->get_layer_count(i); ++j) {
			derivatives_sums[i][j] = ARRAY(this->get_layer_count(i + 1));
		}

		bias_derivatives_sums[i] = ARRAY(this->get_layer_count(i + 1));
	}

	ARRAY output, expected;
	ARRAY_2D neuron_outputs, deltas, bias_derivatives;
	ARRAY_3D derivatives;
	for (int i = 0; i < samples.size(); i += NeuralNetwork::batch_size) {
		/* Reset sums. */
		NeuralNetwork::set_zero(derivatives_sums);
		NeuralNetwork::set_zero(bias_derivatives_sums);

		/* Sum all the error function gradients for (input, output) pairs in this batch. */
		for (int j = i; j < NeuralNetwork::batch_size; ++j) {
			neuron_outputs = this->feedforward_and_get_outputs(get<0>(samples[j]));
			output = neuron_outputs[this->get_num_layers() - 1];
			expected = get<1>(samples[j]);

			/* Backpropagate to compute deltas. */
			deltas = this->backpropagate(output, expected, neuron_outputs);

			/* Compute the gradients. */
			derivatives = this->compute_derivatives(neuron_outputs, deltas);
			bias_derivatives = this->compute_bias_derivatives(deltas); // No need to pass bias outputs since they're always 1

			/* Add to running total. */
			NeuralNetwork::add_arrays(derivatives_sums, derivatives);
			NeuralNetwork::add_arrays(bias_derivatives_sums, bias_derivatives);
		}

		/* Update weights using gradient. */
		this->update_weights(derivatives_sums, NeuralNetwork::learning_rate);
		this->update_bias_weights(bias_derivatives_sums, NeuralNetwork::learning_rate);
	}
}

// Getters

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

BiasNeuron * NeuralNetwork::get_bias_neuron(int i) const { return this->bias_neurons[i]; }

double NeuralNetwork::get_weight(int i, int j, int k) const { return this->get_neuron(i, j)->get_weight(k); }

void NeuralNetwork::set_weight(int i, int j, int k, double val) { this->get_neuron(i, j)->set_weight(k, val); }

double NeuralNetwork::get_bias_weight(int i, int j) const { return this->get_bias_neuron(i)->get_weight(j); }

double NeuralNetwork::set_bias_weight(int i, int j, double val) { this->get_bias_neuron(i)->set_weight(j, val); }

// Functionality

ARRAY NeuralNetwork::feedforward(const ARRAY &inputs) const {
	ARRAY_2D outputs = this->feedforward_and_get_outputs(inputs);
	return outputs[outputs.size() - 1];
}

/* Feeds forward the given input vector as normal but returns an array storing the output of each neuron in the network. */
ARRAY_2D NeuralNetwork::feedforward_and_get_outputs(const ARRAY &inputs) const {
	ARRAY_2D outputs (this->get_num_layers());

	/* Initialize by copying the inputs over, since they are the data corresponding to the first layer. */
	outputs[0] = ARRAY(inputs.size());
	for (int i = 0; i < inputs.size(); ++i) {
		outputs[0][i] = inputs[i]; // Output of the input layer is simply the inputs
	}

	/* Feed the data in, at each layer computing the inputs to the next layer. */
	for (int i = 1; i < this->get_num_layers(); ++i) {
		/* For each neuron in the current layer, increment the input vector for neurons in the next layer with the output 
		 * of the current neuron. */
		outputs[i] = ARRAY(this->get_layer_count(i));

		/* outputs[i][j] = f(W * outputs[i - 1]) - b) where f = activation function, W = weight matrix between previous layer and current 
		 * 				   layer, b = bias from previous layer. */
		for (int j = 0; j < this->get_layer_count(i); ++j) {
			outputs[i][j] = 1 * this->get_bias_weight(i - 1, j); // Output of bias neurons is always 1

			for (int k = 0; k < this->get_layer_count(i - 1); ++k) {
				outputs[i][j] += this->get_weight(i - 1, k, j) * outputs[i - 1][k];
			}

			outputs[i][j] = NeuralNetwork::activation_function(outputs[i][j]);
		}
	}

	return outputs;
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

// ostream& operator <<(ostream &stream, const NeuralNetwork &network) {
// 	for (int i = 0; i < network.get_num_layers() - 1; i++) {
// 		if (i == 0) {
// 			stream << "INPUT LAYER\n";
// 		} else {
// 			stream << "LAYER " << i << endl;
// 		}

// 		stream << "\tBias Neuron - Weights:\n";
// 		for (int j = 0; j < network.get_layer_count(i + 1); j++) {
// 			stream << "\t\tNeuron " << j + ": " << network.get_bias_neuron(i)->get_weight(j) << endl;
// 		}

// 		for (int j = 0; j <  network.get_layer_count(i); j++) {
// 			stream << "\tNeuron " << j << " - Weights:\n";
// 			for (int k = 0; k < network.get_layer_count(i + 1); k++) {
// 				stream << "\t\tNeuron " << k << ": " << network.get_neuron(i, j)->get_weight(k) << endl;
// 			}
// 		}
// 	}

// 	stream << "OUTPUT LAYER\n";
// 	for (int j = 0; j < network.get_layer_count(network.get_num_layers() - 1); j++) {
// 		stream << "\tOutput Neuron " << j;
// 	}

// 	return stream;
// }

/* End NeuralNetwork class. */
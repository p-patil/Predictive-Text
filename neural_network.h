#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

using namespace std;

class Neuron {
	protected:
		int num_synapses;
		Neuron **synapses; // List of neurons in the next layer to which this neuron is connected
		double *weights; // Corresponding weights for the synapses

		static double activation_function(double, double);

	public:
		// Constructors

		Neuron();

		Neuron(const Neuron &);
		
		Neuron(Neuron **, int);

		Neuron(Neuron **, double [], int);

		// Getters
		Neuron ** get_synapses(void) const;

		double * get_weights(void) const;

		double get_weight(int) const;

		int get_num_synapses(void) const;

		// Functionality

		virtual double * output(double) const;

		virtual bool is_output_neuron(void) const;

		// Other
		Neuron & operator =(const Neuron &);

		virtual bool operator ==(const Neuron &);

		~Neuron(void);
};

class BiasNeuron : public Neuron {
	public:
		// Constructors

		BiasNeuron(Neuron **, int);

		BiasNeuron(Neuron **, double *, int);

		// Functionality

		double * output(double) const override;
};

class OutputNeuron : public Neuron {
	public:
		// Constructors

		OutputNeuron();

		// Functionality

		double * output(double) const override;

		bool is_output_neuron(void) const override;

		// Other

		bool operator ==(const Neuron &) override;
};

class NeuralNetwork {
	public:
		int num_layers; // Number of layers in this neural network, including input and output layers
		int *layer_counts; // Keeps track of number of neurons in each layer of the network
		Neuron ***layers; // The layers of this network
		BiasNeuron **bias_neurons; // Maps layers to bias neurons, used to simulate a threshold in the network

		// Constructors

		NeuralNetwork(int *, int);

		NeuralNetwork(int *, double ***, double **, int);

		// Getters

		int get_num_layers(void) const;

		int get_layer_count(int) const;

		int * get_layer_counts(void) const;

		Neuron * get_neuron(int, int) const;

		BiasNeuron * get_bias_neuron(int) const;

		// Functionality

		double * feedforward(double *, int) const;

		// Other

		~NeuralNetwork(void);
};

ostream& operator <<(ostream &, const NeuralNetwork &);

#endif
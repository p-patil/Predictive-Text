#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include <vector>

using namespace std;

typedef vector<double> ARRAY;
typedef vector<ARRAY> ARRAY_2D;
typedef vector<ARRAY_2D> ARRAY_3D;

class Neuron {
	protected:
		int num_synapses;
		Neuron **synapses; // List of neurons in the next layer to which this neuron is connected
		double *weights; // Corresponding weights for the synapses

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

		void set_weight(int, double);

		int get_num_synapses(void) const;

		// Functionality

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
};

class OutputNeuron : public Neuron {
	public:
		// Constructors

		OutputNeuron();

		// Functionality

		bool is_output_neuron(void) const override;

		// Other

		bool operator ==(const Neuron &) override;
};

class NeuralNetwork {
	private:
		static constexpr double learning_rate = 0.7;

		static const int batch_size = 1;

		// Getters and setters

		int get_num_layers(void) const;

		int get_layer_count(int) const;

		int * get_layer_counts(void) const;

		Neuron * get_neuron(int, int) const;

		BiasNeuron * get_bias_neuron(int) const;

		double get_weight(int, int, int) const;

		void set_weight(int, int, int, double);

		double get_bias_weight(int, int) const;

		double set_bias_weight(int, int, double);

		// Helper functions

		static void set_zero(ARRAY_2D &);

		static void set_zero(ARRAY_3D &);

		static void add_arrays(ARRAY_2D &, const ARRAY_2D &);

		static void add_arrays(ARRAY_3D &, const ARRAY_3D &);

		static double activation_function(double);

		static double activation_function_derivative_input(double);

		static double activation_function_derivative_output(double);

		static double error_function(const ARRAY &, const ARRAY &, int);

		ARRAY_2D backpropagate(const ARRAY &, const ARRAY &, const ARRAY_2D &) const;
		
		ARRAY_3D compute_derivatives(const ARRAY_2D &, const ARRAY_2D &);

		ARRAY_2D compute_bias_derivatives(const ARRAY_2D &);

		void update_weights(const ARRAY_3D &, double);

		void update_bias_weights(const ARRAY_2D &, double);

		// void gradient_descent(vector<pair<const ARRAY &, const ARRAY &>>);
		void gradient_descent(const vector<pair<ARRAY, ARRAY>> &);

		ARRAY_2D feedforward_and_get_outputs(const ARRAY &) const;

	public:
		int num_layers; // Number of layers in this neural network, including input and output layers
		int *layer_counts; // Keeps track of number of neurons in each layer of the network
		Neuron ***layers; // The layers of this network
		BiasNeuron **bias_neurons; // Maps layers to bias neurons, used to simulate a threshold in the network


		// Constructors

		NeuralNetwork(int *, int);

		NeuralNetwork(int *, double ***, double **, int);

		// Functionality

		ARRAY feedforward(const ARRAY &) const;
		
		// Other

		~NeuralNetwork(void);
};

// ostream& operator <<(ostream &, const NeuralNetwork &);

#endif
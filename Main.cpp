#include <vector>
#include <time.h>	// srand(time())
#include "GeneticAlg.h"

using namespace std;

// Defining the problem:
// ----------------------------------------------------
struct obj {
	int weight;
	int value;
};
class Knapsack : public Problem {
public:
	int maxWeight = 10;
	vector<obj> objects = {
		//{weight, value}
		{ 15, 15 },
		{ 3, 7 },
		{ 2, 10 },
		{ 5, 5 },
		{ 9, 8 },
		{ 20, 17 }
		/*{ 13, 14 },
		{ 4, 12 },
		{ 7, 11 },
		{ 1, 2 },
		{ 25, 30 },
		{ 14, 16 }*/
	};

	// the weight of all objects can't be greater than the capacity of the knapsack
	bool constraints (const vector<int>& DNA) {
		int weight = 0;
		for (int i = 0; i < (int)DNA.size(); i++) {
			weight += DNA[i] * objects[i].weight;
		}

		return weight <= maxWeight;
	};

	// returns knapsack's value
	int getFitness (const vector<int>& DNA) {
		int fitness = 0;
		for (int i = 0; i < (int)DNA.size(); i++) {
			fitness += DNA[i] * objects[i].value;
		}

		return fitness;
	};

	float getRandGene() {
		return rand() % 2;
	}
};

struct cell {
	int value = 0;
	vector<int> objects;
};

cell* knapSack(int W, vector<obj> & objects, int n)
{
	int i, w;
	cell d[13][51];
	auto max = [](int a, int b) { return a > b ? a : b; };

	for (i = 1; i <= n; i++) {
		for (w = 1; w <= W; w++) {
			if (objects[i - 1].weight <= w) {
				int newVal = objects[i - 1].value + d[i - 1][w - objects[i - 1].weight].value;
				if (newVal > d[i - 1][w].value) {
					d[i][w].value = newVal;
					d[i][w].objects = d[i - 1][w - objects[i - 1].weight].objects;
					d[i][w].objects.push_back(1);
				}
				else {
					d[i][w].value = d[i - 1][w].value;
					d[i][w].objects = d[i - 1][w].objects;
					d[i][w].objects.push_back(0);
				}
				//d[i][w].value = max(objects[i - 1].value + d[i - 1][w - objects[i - 1].weight].value, d[i - 1][w].value);
			}
			else {
				d[i][w] = d[i - 1][w];
				d[i][w].objects.push_back(0);
			}
		}
	}

	cell* solution = new cell(d[n][W]);
	return solution;
}

int main() {
	srand(time(0));

	// Knapsack problem
	Knapsack* k = new Knapsack();
	
	// -------------------------------------------------------
	// Genetic Algorithm
	int pop_size = 5;
	int nr_tries = 1000 / pop_size;	// number of times it tries to come with a better solution
	int error_offset = 1;	// how much the new solution should differ from the previous one to be counted as different (used in Evaluate())
	GeneticAlg gAlg(pop_size, nr_tries, error_offset, k->objects.size(), k);
	for (int Generation = 1; gAlg.Run(); Generation++) {
		cout << "Generation: " << Generation << " has best member with Fitness: " << gAlg.getSolution()->Fitness << endl;
	}

	Member* solution = gAlg.getSolution();
	cout << "Solution with Genetic Algorithm: " << solution->Fitness << endl;
	for (int Gene : solution->DNA)
		cout << Gene << " ";

	cell* DPsolution = knapSack(k->maxWeight, k->objects, k->objects.size());
	cout << "\n\nSolution with Dynamic Programming: " << DPsolution->value << endl;
	for (int gene : DPsolution->objects)
		cout << gene << " ";
	cout << "\n\n";

	system("pause");
	delete k;
	return 0;
}
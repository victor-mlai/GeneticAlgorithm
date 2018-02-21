#include <vector>
#include <time.h>	// srand(time())
#include "Population.h"

using namespace std;

// Defining the problem:
// ----------------------------------------------------
class Knapsack : public Problem {
public:
	int maxWeight = 50;
	vector< pair<int, int> > objects = {
		//{weight, value}
		{ 15, 15 },
		{ 3, 7 },
		{ 2, 10 },
		{ 5, 5 },
		{ 9, 8 },
		{ 20, 17 },
		{ 13, 14 },
		{ 4, 12 },
		{ 7, 11 },
		{ 1, 2 },
		{ 25, 30 },
		{ 14, 16 }
	};

	// the weight of all objects can't be greater than the capacity of the knapsack
	bool constraints (const vector<int>& DNA) {
		int weight = 0;
		for (int i = 0; i < (int)DNA.size(); i++) {
			weight += DNA[i] * objects[i].first;
		}

		return weight <= maxWeight;
	};

	// returns knapsack's value
	int getFitness (const vector<int>& DNA) {
		int fitness = 0;
		for (int i = 0; i < (int)DNA.size(); i++) {
			fitness += DNA[i] * objects[i].second;
		}

		return fitness;
	};

	float getRandGene() {
		return rand() % 2;
	}
};

int knapSack(int W, vector<pair<int, int>> & objects, const int n)
{
	int i, w;
	int d[13][51] = { 0 };
	auto max = [](int a, int b) { return a > b ? a : b; };

	for (i = 1; i <= n; i++) {
		for (w = 1; w <= W; w++) {
			if (objects[i - 1].first <= w) {
				d[i][w] = max(objects[i - 1].second + d[i - 1][w - objects[i - 1].first], d[i - 1][w]);
			}
			else {
				d[i][w] = d[i - 1][w];
			}
		}
	}

	return d[n][W];
}

int main() {
	srand(time(0));

	// Knapsack problem
	Knapsack* k = new Knapsack();
	
	// -------------------------------------------------------
	// Genetic Algorithm
	Population pop(1000, k->objects.size(), k);
	for (int Generation = 1; pop.Run(); Generation++) {
		cout << "Generation: " << Generation << " has best member with Fitness: " << pop.getSolution()->Fitness << endl;
	}

	Member* solution = pop.getSolution();
	cout << "Solution with Genetic Algorithm: " << solution->Fitness << endl;
	for (int Gene : solution->DNA)
		cout << Gene << " ";

	cout << "\n\nSolution with Dynamic Programming: " << knapSack(k->maxWeight,  k->objects, k->objects.size()) << endl;

	system("pause");
	delete k;
	return 0;
}
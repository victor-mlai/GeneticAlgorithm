#pragma once
#include <vector>
#include <iostream>
#include <stdlib.h>	// rand()

using namespace std;

/*
Changes to make the algorithm work on any problem:
	* the data type storred by the DNA 
		(for Knapsack it should be bites,
		 for Travelling Sales Man Problem it should be integers,
		 for a Neural Network: float/double (the data type of the weights))

	Problem::constraints	(none if not needed)
	Problem::getFitness
	Member::randomize_DNA
	GeneticAlg::XO
	GeneticAlg::Mutate_Gene
*/


class Problem {
public:
	// should return true if the DNA respects the constraints
	virtual bool constraints(const vector<int>& DNA) = 0;
	// should return a Fitness >= 0
	virtual int getFitness(const vector<int>& DNA) = 0;
};

class Member {
public:
	Member() {}
	~Member() {}
	Member(int DNA_size) { DNA.resize(DNA_size); }

	// used in Evaluate() to copy in this member all the data from "other"
	void Copy(Member* other) {
		DNA.resize(other->DNA.size());
		for (int i = 0; i < (int)DNA.size(); i++) {
			DNA[i] = other->DNA[i];
		}

		Fitness = other->Fitness;
	}

	void randomize_DNA() {
		for (int i = 0; i < (int)DNA.size(); i++) {
			DNA[i] = rand() % 2;
		}
	}

	int getFitness() const { return Fitness; }

	void setFitness(int newFitness) {
		Fitness = newFitness;
	}

	vector<int> DNA;
	int Fitness;
};

class GeneticAlg
{
public:
	GeneticAlg();
	GeneticAlg(int pop_size, int nr_tries, int err, int DNA_size, Problem* p);	// generate a random population
	~GeneticAlg();

	bool Run();
	Member* getSolution() const { return elite_member; }

private:
	void Evaluate();
	void Select();	// proportional roulette wheel selection
	void Crossover();
	void Mutate_Pop();

	// uniform random cross-over of 2 chromosomes
	void XO(Member * p1, Member * p2, Member * c);
	void Mutate_Gene(Member * m, int gene_index);

	Problem* probl;	// problem
	vector<Member*> pop;	// population members

	vector<Member*> elite_pop;	// elite members
	vector<Member*> crOv_pop;	// cross-over-ed members

private:
	int MutationRate;	// how often we mutate: if set to 25 means we mutate a Gene with a 25% chance
	int elite_Size;
	int DNA_size;

	int error_offset;	// how much the new solution should differ from the previous one to be counted as different (used in Evaluate())
	int nr; // nr of times I got the loc_max Fitness
	int nr_tries;	// number of times it tries to come up with a better solution
	int loc_max;	// the maximum fitness reached so far
	Member* elite_member;	// current best member from pop vector
};

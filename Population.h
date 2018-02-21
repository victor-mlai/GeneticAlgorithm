#pragma once
#include <vector>
#include <iostream>
#include <stdlib.h>	// rand()

using namespace std;

class Problem {
public:
	// should return true if the DNA respects the constraints
	virtual bool constraints(const vector<int>& DNA) = 0;
	// should return a Fitness >= 0
	virtual int getFitness(const vector<int>& DNA) = 0;
	// returns a random Gene
	virtual float getRandGene() = 0;
};

class Member {
public:
	Member() {}
	~Member() {}
	Member(int DNA_size) { DNA.resize(DNA_size); }

	void Copy(Member* other) {
		DNA.resize(other->DNA.size());
		for (int i = 0; i < DNA.size(); i++) {
			DNA[i] = other->DNA[i];
		}

		Fitness = other->Fitness;
	}

	void randomize_DNA(Problem* p) {
		for (int i = 0; i < (int)DNA.size(); i++) {
			DNA[i] = p->getRandGene();
		}
	}

	void setFitness(int newFitness) {
		Fitness = newFitness;
	}

	void setGene(int index, int value) {
		DNA[index] = value;
	}

	vector<int> DNA;
	int Fitness;
};

class Population
{
public:
	Population();
	Population(int pop_size, int DNA_size, Problem* p);	// generate a random population
	~Population();

	bool Run();
	int getNr() const { return nr; }
	Member* getSolution() const { return elite_member; }

private:
	void Evaluate();
	void Select();	// proportional roulette wheel selection
	void Crossover();	// uniform random cross-over of 2 chromosomes
	void Mutate();		// uniform mutation: replacing a chromosome gene by a new one

	Problem* probl;	// problem
	vector<Member*> pop;	// population members
	vector<Member*> elite_pop;	// elite members
	vector<Member*> crOv_pop;	// cross-over-ed members

private:
	int MutationRate;
	int elite_Size;
	int DNA_size;

	int nr; // nr of times I got the loc_max Fitness
	int loc_max;	// the maximum fitness reached so far
	Member* elite_member;	// current best member from pop vector
};

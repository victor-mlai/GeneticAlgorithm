#include "Population.h"
#include <assert.h>

Population::Population()
{
}

Population::~Population()
{
	for (int i = 0; i < pop.size(); i++) {
		delete crOv_pop[i];
		delete pop[i];
	}

	delete elite_member;
}

Population::Population(int pop_size, int DNA_size, Problem* p) : probl(p), DNA_size(DNA_size)
{
	assert(pop_size > 2);

	pop.resize(pop_size);
	for (int i = 0; i < pop_size; i++) {
		pop[i] = new Member(DNA_size);
		pop[i]->randomize_DNA(p);
	}

	auto max = [](int a, int b) { return a > b ? a : b; };
	elite_Size = max(ceil(pop_size / 10), 2);	// the elite population should have at least 2 members
	elite_pop.resize(elite_Size);

	crOv_pop.resize(pop_size);
	for (int i = 0; i < pop_size; i++) {
		crOv_pop[i] = new Member(DNA_size);
	}

	MutationRate = ceil(64.0f / DNA_size);
	nr = 0;
	loc_max = 0;
	elite_member = new Member(DNA_size);
}

bool Population::Run()
{
	Evaluate();
	if (nr > 10)
		return false;

	Select();
	Crossover();
	Mutate();

	pop.swap(crOv_pop);

	return true;
}

void Population::Evaluate()
{
	int max_f = 0;
	Member* max_m = NULL;

	for (Member* m : pop) {
		if (probl->constraints(m->DNA)) {
			int fit = probl->getFitness(m->DNA);
			m->setFitness(fit);

			if (max_f < fit) {
				max_m = m;
				max_f = fit;
			}
		}
		else
			m->setFitness(0);
	}

	if (loc_max < max_f) {
		loc_max = max_f;
		elite_member->Copy(max_m);
		nr = 0;
	}

	nr++;
}

// proportional roulette wheel selection
void Population::Select()
{
	int sumFitness = 0;
	for (Member* m : pop) {
		sumFitness += m->Fitness;
	}

	// selecting mating population
	for (int i = 0; i < elite_Size; ++i) {
		// generating a random fitness sum in [0, sumFitness)
		int randSum = rand() % sumFitness;

		int j = 0;
		while (randSum >= 0.0) {
			randSum -= pop[j]->Fitness;
			j++;
		}

		// selecting element
		elite_pop[i] = pop[j-1];
	}
}

// random cross-over of 2 chromosomes
void Population::Crossover()
{
	for (int i = 0; i < crOv_pop.size(); i++) {
		// choosing randomly 2 chromosomes from elite population
		int half_elite_size = elite_pop.size() / 2;
		int idx1 = rand() % half_elite_size;	// selecting from first half
		int idx2 = rand() % half_elite_size + half_elite_size;	// selecting from 2nd half

		vector<Member*> parents = { elite_pop[idx1] , elite_pop[idx2] };

		/*
			Parents		|	Random selection	|	Child
		p1: 0 1 1 0 1	|	p1: 0     0			| c: 0 1 0 0 1
		p2: 0 1 0 1 1	|	p2:   1 0   1		| 
		*/
		int DNA_size = elite_pop[idx1]->DNA.size();
		for (int j = 0; j < DNA_size; j++) {
			// choosing between either 0th parent or 1st
			int r = rand() % 2;

			crOv_pop[i]->DNA[j] = parents[r]->DNA[j];
		}
	}
}

// uniform mutation
void Population::Mutate()
{
	int crOv_size = crOv_pop.size();
	int DNA_size = crOv_pop[0]->DNA.size();
	for (int i = 0; i < crOv_size; i++) {
		for (int j = 0; j < DNA_size; ++j) {
			if (rand() % 100 <= MutationRate) {
				crOv_pop[i]->DNA[j] = 1 - crOv_pop[i]->DNA[j];
			}
		}
	}
}
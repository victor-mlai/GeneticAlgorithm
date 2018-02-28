#include "GeneticAlg.h"
#include <assert.h>

GeneticAlg::GeneticAlg()
{
}

GeneticAlg::~GeneticAlg()
{
	for (int i = 0; i < pop.size(); i++) {
		delete crOv_pop[i];
		delete pop[i];
	}

	delete elite_member;
}

GeneticAlg::GeneticAlg(int pop_size, int nr_tries, int err, int DNA_size, Problem* p)
	: probl(p), DNA_size(DNA_size), nr_tries(nr_tries), error_offset(err)
{
	assert(pop_size > 2);

	pop.resize(pop_size);
	for (int i = 0; i < pop_size; i++) {
		pop[i] = new Member(DNA_size);
		do {
			pop[i]->randomize_DNA(p);	// randomize until the constraint is respected
		} while (!probl->constraints(pop[i]->DNA));
	}

	auto max = [](int a, int b) { return a > b ? a : b; };
	elite_Size = max(ceil(pop_size / 10), 2);	// the elite population should have at least 2 members
	elite_pop.resize(elite_Size);

	crOv_pop.resize(pop_size);
	for (int i = 0; i < pop_size; i++) {
		crOv_pop[i] = new Member(DNA_size);
	}

	//error_offset = 0;
	MutationRate = ceil(50 / DNA_size);
	nr = 0;
	loc_max = 0;
	elite_member = new Member(DNA_size);
}

bool GeneticAlg::Run()
{
	Evaluate();
	if (nr > nr_tries)	// if I got the same result 3 times, we stop
		return false;

	Select();
	Crossover();
	Mutate_Pop();

	pop.swap(crOv_pop);

	return true;
}

void GeneticAlg::Evaluate()
{
	int max_f = 0;
	Member* max_m = NULL;

	for (Member* m : pop) {
		int fit = probl->getFitness(m->DNA);
		m->setFitness(fit);

		if (max_f < fit) {
			max_m = m;
			max_f = fit;
		}
	}

	if (loc_max < max_f - error_offset) {
		loc_max = max_f;
		elite_member->Copy(max_m);
		nr = 0;
	}

	nr++;
}

// proportional roulette wheel selection
void GeneticAlg::Select()
{
	int sumFitness = 0;
	for (Member* m : pop) {
		sumFitness += m->Fitness;
	}

	assert(sumFitness != 0);

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

// tournament selection
//void GeneticAlg::Select()
//{
//	int popsize = pop.size();
//	int tntsize = 2 * elite_Size;
//
//	// selecting mating population
//	for (int i = 0; i < elite_Size; ++i) {
//		// selecting randomly a first element
//		int bestIdx = rand() % popsize;
//		int bestFit = pop[bestIdx]->getFitness();
//
//		// starting tournament
//		for (int j = 1; j < tntsize; ++j) {
//			int idx = rand() % popsize;
//			int fit = pop[idx]->getFitness();
//
//			if (fit > bestFit) {
//				bestFit = fit;
//				bestIdx = idx;
//			}
//		}
//
//		// selecting element
//		elite_pop[i] = pop[bestIdx];
//	}
//}

// random cross-over of 2 chromosomes
void GeneticAlg::Crossover()
{
	for (int i = 0; i < crOv_pop.size(); i++) {
		// choosing randomly 2 chromosomes from elite population
		int half_elite_size = elite_pop.size() / 2;
		int idx1 = rand() % half_elite_size;	// selecting from first half
		int idx2 = rand() % half_elite_size + half_elite_size;	// selecting from 2nd half

		do {
			XO(elite_pop[idx1], elite_pop[idx2], crOv_pop[i]);
		} while (!probl->constraints(crOv_pop[i]->DNA));
	}
}

// uniform mutation
void GeneticAlg::Mutate_Pop()
{
	int crOv_size = crOv_pop.size();
	int DNA_size = crOv_pop[0]->DNA.size();
	for (int i = 0; i < crOv_size; i++) {
		do {
			for (int j = 0; j < DNA_size; ++j) {
				if (rand() % 100 <= MutationRate) {
					Mutate_Gene(crOv_pop[i], j);
				}
			}
		} while (!probl->constraints(crOv_pop[i]->DNA));
	}
}

/*
receives 2 parents and rewrites the child "c"
	Parents		|	Random selection	|	Child
p1: 0 1 1 0 1	|	p1: 0     0			| c: 0 1 0 0 1
p2: 0 1 0 1 1	|	p2:   1 0   1		|
*/
void GeneticAlg::XO(Member* p1, Member* p2, Member* c) {
	vector<Member*> parents = { p1 , p2 };

	int DNA_size = p1->DNA.size();
	for (int j = 0; j < DNA_size; j++) {
		// choosing between either 0th parent's gene or 1st's
		int r = rand() % 2;

		c->DNA[j] = parents[r]->DNA[j];
	}
}

// flip the bit
void GeneticAlg::Mutate_Gene(Member* m, int gene_index) {
	m->DNA[gene_index] = 1 - m->DNA[gene_index];
}
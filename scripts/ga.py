import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import random

class Individual(object):

    def __init__(self, numbers=None, mutate_prob=0.01):
        if numbers is None:
            self.numbers = np.random.uniform(-3, 3, size=(63))
        else:
            self.numbers = numbers
            # Mutate
            if mutate_prob > np.random.rand():
                mutate_index = np.random.randint(len(self.numbers) - 1)
                self.numbers[mutate_index] = np.random.uniform(-3,3)

class Population(object):

    def __init__(self, pop_size=10, mutate_prob=0.01, retain=0.2, random_retain=0.03):
        """
            Args
                pop_size: size of population
                fitness_goal: goal that population will be graded against
        """
        self.pop_size = pop_size
        self.mutate_prob = mutate_prob
        self.retain = retain
        self.random_retain = random_retain
        self.fitness_history = []
        self.parents = []
        self.done = False

        # Create individuals
        self.individuals = []
        for x in range(pop_size):
            self.individuals.append(Individual(numbers=None,mutate_prob=self.mutate_prob))

    def grade(self, fitness_sum, generation=None):
        """
            Grade the generation by getting the average fitness of its individuals
        """
        pop_fitness = fitness_sum / self.pop_size
        self.fitness_history.append(pop_fitness)

        # Set Done flag if we hit target
        if int(round(pop_fitness)) == 100000:
            self.done = True

        if generation is not None:
            if generation % 5 == 0:
                print("Episode",generation,"Population fitness:", pop_fitness)

    def select_parents(self,fitness_vals):
        """
            Select the fittest individuals to be the parents of next generation (lower fitness it better in this case)
            Also select a some random non-fittest individuals to help get us out of local maximums
        """
        # Sort individuals by fitness (we use reversed because in this case lower fintess is better)
        self.individuals = [x for _,x in sorted(zip(fitness_vals,self.individuals),reverse=True)]
        # Keep the fittest as parents for next gen
        retain_length = self.retain * len(self.individuals)
        self.parents = self.individuals[:int(retain_length)]

        # Randomly select some from unfittest and add to parents array
        unfittest = self.individuals[int(retain_length):]
        for unfit in unfittest:
            if self.random_retain > np.random.rand():
                self.parents.append(unfit)

    def breed(self):
        """
            Crossover the parents to generate children and new generation of individuals
        """
        target_children_size = self.pop_size - len(self.parents)
        children = []
        if len(self.parents) > 0:
            while len(children) < target_children_size:
                father = random.choice(self.parents)
                mother = random.choice(self.parents)
                if father != mother:
                    child_numbers = [ random.choice(pixel_pair) for pixel_pair in zip(father.numbers, mother.numbers)]
                    child = Individual(child_numbers)
                    children.append(child)
            self.individuals = self.parents + children

    def evolve(self,fitness_vals):
        # 1. Select fittest
        self.select_parents(fitness_vals)
        # 2. Create children and new generation
        self.breed()
        # 3. Reset parents and children
        self.parents = []

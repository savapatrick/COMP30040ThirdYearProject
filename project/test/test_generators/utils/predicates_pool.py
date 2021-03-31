import random

from .random_factory import RandomFactory


# this is a singleton class
# constants always start with p and have at least 2 letters
class PredicatesPool:
    def __generate_predicate(self):
        new_predicate = "p" + RandomFactory.generate_random_lowercase(random.randint(1, self.predicate_length_max))
        allowance = 26 ** self.predicate_length_max
        while new_predicate in self.predicates:
            new_predicate = "p" + RandomFactory.generate_random_lowercase(random.randint(1, self.predicate_length_max))
            allowance -= 1
            if allowance < 0:
                self.predicate_length_max += 1
        self.predicates.append(new_predicate)
        return new_predicate

    _unique_instance = None

    def __choice__(self):
        return random.choice(self.predicates)

    def __choices__(self, size):
        return random.choices(self.predicates, k=size)

    def __sample__(self, size):
        return random.sample(self.predicates, k=size)

    def __new__(cls, *args, **kwargs):
        if not cls._unique_instance:
            cls._unique_instance = super(PredicatesPool, cls).__new__(cls)
        return cls._unique_instance

    def __init__(self, _predicate_length_max, size):
        self.predicates = []
        self.predicate_length_max = max(1, _predicate_length_max)
        for i in range(size):
            self.__generate_predicate()
        self.alpha = self.predicates
        self.beta = random.sample(self.alpha, k=random.randint(1, len(self.alpha)))

    def get_predicates(self):
        return self.predicates

    def alpha_choice(self):
        return random.choice(self.alpha)

    def beta_choice(self):
        return random.choice(self.beta)

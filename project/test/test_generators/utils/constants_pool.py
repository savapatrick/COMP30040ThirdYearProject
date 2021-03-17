import random

from random_factory import RandomFactory


# this is a singleton class
# constants always start with c and have at least 2 letters
class ConstantsPool:

    def __generate_constant(self):
        new_constant = "c" + RandomFactory.generate_random_lowercase(random.randint(1, self.constant_length_max))
        allowance = 26 ** self.constant_length_max
        while new_constant in self.constants:
            new_constant = "c" + RandomFactory.generate_random_lowercase(random.randint(1, self.constant_length_max))
            allowance -= 1
            if allowance < 0:
                self.constant_length_max += 1
        self.constants.append(new_constant)
        return new_constant

    _unique_instance = None

    def __new__(cls, *args, **kwargs):
        if not cls._unique_instance:
            cls._unique_instance = super(ConstantsPool, cls).__new__(cls, *args, **kwargs)
        return cls._unique_instance

    def __init__(self, _constant_length_max, size):
        self.constants = []
        self.constant_length_max = max(1, _constant_length_max)
        for i in range(size):
            self.__generate_constant()

    def get_constants(self):
        return self.constants

    def choice(self):
        return random.choice(self.constants)

    def choices(self, size):
        return random.choices(self.constants, k=size)

    def sample(self, size):
        return random.sample(self.constants, k=size)

from .random_factory import RandomFactory
import random


# this is a singleton class
# constants always start with c and have at least 2 letters
class VariablesPool:

    def __generate_variable(self):
        new_variable = "v" + RandomFactory.generate_random_lowercase(random.randint(1, self.variables_length_max))
        allowance = 26 ** self.variables_length_max
        while new_variable in self.variables:
            new_variable = "v" + RandomFactory.generate_random_lowercase(random.randint(1, self.variables_length_max))
            allowance -= 1
            if allowance < 0:
                self.variables_length_max += 1
        self.variables.append(new_variable)
        return new_variable

    _unique_instance = None

    def __new__(cls, *args, **kwargs):
        if not cls._unique_instance:
            cls._unique_instance = super(VariablesPool, cls).__new__(cls)
        return cls._unique_instance

    def __init__(self, _variables_length_max, size):
        self.variables = []
        self.variables_length_max = max(1, _variables_length_max)
        if size >= 1:
            size -= 1
            self.variables.append("vx")
        if size >= 1:
            size -= 1
            self.variables.append("vy")
        for i in range(size):
            self.__generate_variable()

    def get_variables(self):
        return self.variables

    def choice(self):
        return random.choice(self.variables)

    def choices(self, size):
        return random.choices(self.variables, k=size)

    def sample(self, size):
        return random.sample(self.variables, k=size)

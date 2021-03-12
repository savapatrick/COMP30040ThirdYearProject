import random
import string


class RandomFactory:
    @staticmethod
    def generate_random_lowercase(how_many):
        return "".join(random.choice(string.ascii_lowercase) for _ in range(how_many))

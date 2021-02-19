from random import randint
from random import choice
from random import sample
from random import random

import string

PREDICATE_LENGTH_MAX = 8
PREDICATE_ARITY_MAX = 3
CONSTANT_LENGTH_MAX = 8
VARIABLE_LENGTH_MAX = 8
MAX_NUMBER_PREDICATES = 25

# this one should contain double implication
possible_connectors = ["->", "|", "^", "|"]


def generate_random_lowercase(how_many):
    return "".join(choice(string.ascii_lowercase) for _ in range(how_many))


def generate_constant(variables_so_far, constants_so_far):
    new_constant = generate_random_lowercase(randint(1, CONSTANT_LENGTH_MAX))
    while new_constant in variables_so_far or new_constant in constants_so_far:
        new_constant = generate_random_lowercase(randint(1, CONSTANT_LENGTH_MAX))
    constants_so_far.append(new_constant)
    return new_constant


def generate_variable(variables_so_far, constants_so_far):
    new_variable = generate_random_lowercase(randint(1, VARIABLE_LENGTH_MAX))
    while new_variable in variables_so_far or new_variable in constants_so_far:
        new_variable = generate_random_lowercase(randint(1, VARIABLE_LENGTH_MAX))
    variables_so_far.append(new_variable)
    return new_variable


def terms(variables_so_far, constants_so_far, how_many):
    while len(variables_so_far) + len(constants_so_far) <= how_many:
        generate_constant(variables_so_far, constants_so_far)
    which_portion = randint(1, how_many)
    if len(variables_so_far) < which_portion or len(constants_so_far) < how_many - which_portion:
        which_portion = len(variables_so_far)
    assert which_portion <= len(variables_so_far)
    assert how_many - which_portion <= len(constants_so_far)
    return sample(variables_so_far, k=max(0, which_portion)) + sample(constants_so_far, k=max(0, how_many-which_portion))


def generate_predicate(predicates_so_far):
    predicate_name = None
    predicates_dict = dict(predicates_so_far)
    while predicate_name in predicates_dict or predicate_name is None:
        first_letter = choice(string.ascii_uppercase)
        how_many = randint(0, PREDICATE_LENGTH_MAX - 1)
        further_letters = generate_random_lowercase(how_many)
        predicate_name = first_letter + further_letters
    predicates_so_far.append((predicate_name, randint(1, PREDICATE_ARITY_MAX)))


def instantiate_predicate(predicate_and_size, variables_so_far, constants_so_far):
    predicate, size = predicate_and_size
    return predicate + "(" + ",".join(terms(variables_so_far, constants_so_far, size)) + ")"


def formula(predicates_so_far, variabes_so_far, constants_so_far, limit):
    # with probability 10%
    if random() <= 0.2 or limit < 0:
        current_predicate = choice(predicates_so_far)
        new_predicate = instantiate_predicate(current_predicate, variabes_so_far, constants_so_far)
        # with probability 33%
        if random() <= 0.33:
            new_predicate = "~" + new_predicate
        return new_predicate, limit - len(new_predicate)
    else:
        # with probability 50%
        if random() <= 0.5:
            # connectives
            first_formula, new_limit = formula(predicates_so_far, variabes_so_far, constants_so_far, limit)
            second_formula, new_limit = formula(predicates_so_far, variabes_so_far, constants_so_far, new_limit)
            return f"({first_formula}{choice(possible_connectors)}{second_formula})", new_limit - 1
        else:
            # quatifiers
            which_quantifier = '@'
            # with probability 50%
            if random() <= 0.5:
                # existence
                which_quantifier = '?'
            new_variable = generate_variable(variabes_so_far, constants_so_far)
            first_formula, new_limit = formula(predicates_so_far, variabes_so_far, constants_so_far, limit)
            variabes_so_far.remove(new_variable)
            return f"({which_quantifier}{new_variable}({first_formula}))", new_limit - 3 - len(new_variable)


if __name__ == "__main__":
    with open("output.txt", "w") as out:
        size_limit = randint(1, 2500)
        predicates_number = randint(1, MAX_NUMBER_PREDICATES)
        predicates = []
        for _ in range(predicates_number):
            generate_predicate(predicates)
        out.write(formula(predicates, [], [], size_limit)[0])
    exit(0)

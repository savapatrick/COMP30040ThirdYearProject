from utils.variables_pool import VariablesPool
from utils.predicates_pool import PredicatesPool
from utils.equality import Equality
from utils.predicate import Predicate
import argparse
import random

argument_parser = argparse.ArgumentParser()
argument_parser.add_argument("-A", "--alpha", type=int,
                             help="This is how many clauses "
                                  "will occur under doubly nested under universal "
                                  "quantifier.")
argument_parser.add_argument("-ARMIN", "--arity-min", type=int, default=2,
                             help="This is the minimum arity a predicate could have")
argument_parser.add_argument("-ARMAX", "--arity-max", type=int, default=2,
                             help="This is the maximum arity a predicate could have")
argument_parser.add_argument("-B", "--beta", type=int,
                             help="This is how many clauses will occur under an universal "
                                  "quantifier and further under an existential one per "
                                  "conjunction.")
argument_parser.add_argument("-C", "--conjunctions", type=int,
                             help="This is how many conjunctions are going to be.")
argument_parser.add_argument("-E", "--has-equality", default=False,
                             help="Whether we generate formulas with equality or not.",
                             action="store_true")
argument_parser.add_argument("-LMIN", "--min-literals-per-clause", type=int, default=3,
                             help="This is the minimum number of literals occurring per clause.")
argument_parser.add_argument("-LMAX", "--max-literals-per-clause", type=int, default=3,
                             help="This is the maximum number of literals occurring per clause.")
argument_parser.add_argument("-P", "--predicates", type=int,
                             help="This is the cardinality of the pool of predicates "
                                  "from which a random predicate is going to be picked.")

arguments = argument_parser.parse_args()

ARITY_MIN = arguments.arity_min
ARITY_MAX = arguments.arity_max
MIN_LITERALS_PER_CLAUSE = arguments.min_literals_per_clause
MAX_LITERALS_PER_CLAUSE = arguments.max_literals_per_clause


class ScottClauseRandomGenerator:
    def __init__(self, _is_alpha, _number_of_clauses, _has_equality, _variable_manager, _predicate_manager,
                 _conjunctions=None):
        self.is_alpha = _is_alpha
        self.has_equality = _has_equality
        self.variables = _variable_manager.get_variables()
        assert len(self.variables) == 2
        number_of_literals_per_clause = random.randint(MIN_LITERALS_PER_CLAUSE, MAX_LITERALS_PER_CLAUSE)
        if _is_alpha:
            self.alpha = [[Predicate(_predicate_manager.choice(), 2, _variable_manager.choices(2),
                                     random.choice([False, True])) for _ in range(number_of_literals_per_clause)]
                          for _ in range(_number_of_clauses)]
        else:
            if _conjunctions is None:
                raise ValueError("[Scott Clause]: for the beta clauses, the number of conjunctions has to be given")
            self.beta = [[[Predicate(_predicate_manager.choice(), 2, _variable_manager.choices(2),
                                     random.choice([False, True])) for _ in range(number_of_literals_per_clause)]
                          for _ in range(_number_of_clauses)] for _ in range(_conjunctions)]
        if self.has_equality:
            self.equality = Equality(self.variables[0], self.variables[1], not _is_alpha)

    def tp_output(self):
        if self.is_alpha:
            # todo: redo this 
            return f"@{self.variables[0]}@{self.variables[1]}(" \
                   f"{'|'.join([predicate.tp_output() for predicate in self.clause])}) "

    def vampire_output(self):
        pass


if __name__ == "__main__":
    variable_manager = VariablesPool(1, 2)
    predicate_manager = PredicatesPool(1, arguments.predicates)
    alpha = ScottClauseRandomGenerator(True, arguments.alpha, arguments.has_equality,
                                       variable_manager, predicate_manager)
    beta = ScottClauseRandomGenerator(False, arguments.beta, arguments.has_equality, variable_manager,
                                      predicate_manager, _conjunctions=arguments.conjunctions)
    with open("input_th.txt", "w") as th:
        th.write(f"(({alpha.tp_output()}) ^ ({beta.tp_output()}))")
    with open("input_vampire.txt", "w") as vampire:
        vampire.write(f"(({alpha.vampire_output()}) & ({beta.vampire_output()}))")
    exit(0)

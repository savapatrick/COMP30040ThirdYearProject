from utils.variables_pool import VariablesPool
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


class ScottClause:
    def __init__(self, _is_alpha, _has_equality, _variables, _conjunctions=None):
        self.is_alpha = _is_alpha
        self.has_equality = _has_equality
        self.variables = _variables
        if _is_alpha:
            self.clauses = []
            pass
        else:
            if _conjunctions is None:
                raise ValueError("[Scott Clause]: for the beta clauses, the number of conjunctions has to be given")

    def tp_output(self):
        pass

    def vampire_output(self):
        pass


if __name__ == "__main__":
    variable_manager = VariablesPool(1, 2)
    alpha = ScottClause(True, arguments.has_equality, variable_manager.get_variables())
    beta = ScottClause(False, arguments.has_equality, variable_manager.get_variables())
    with open("input_th.txt", "w") as th:
        th.write(f"(({alpha.tp_output()}) ^ ({beta.tp_output()}))")
    with open("input_vampire.txt", "w") as vampire:
        vampire.write(f"(({alpha.vampire_output()}) & ({beta.vampire_output()}))")
    exit(0)

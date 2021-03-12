from utils.predicate import Predicate
from utils.equality import Equality
import argparse

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

if __name__ == "__main__":
    print("works!")
    exit(0)


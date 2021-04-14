import argparse
import random
import re

from utils.equality import Equality
from utils.predicate import Predicate
from utils.predicates_pool import PredicatesPool
from utils.variables_pool import VariablesPool

argument_parser = argparse.ArgumentParser()
argument_parser.add_argument("-A", "--alpha", type=int, default=1,
                             help="This is how many clauses "
                                  "will occur under doubly nested under universal "
                                  "quantifier.")
argument_parser.add_argument("-ARMIN", "--arity-min", type=int, default=2,
                             help="This is the minimum arity a predicate could have")
argument_parser.add_argument("-ARMAX", "--arity-max", type=int, default=2,
                             help="This is the maximum arity a predicate could have")
argument_parser.add_argument("-B", "--beta", type=int, default=1,
                             help="This is how many clauses will occur under an universal "
                                  "quantifier and further under an existential one per "
                                  "conjunction.")
argument_parser.add_argument("-C", "--conjunctions", type=int, default=1,
                             help="This is how many conjunctions are going to be.")
argument_parser.add_argument("-E", "--has-equality", default=False,
                             help="Whether we generate formulas with equality or not.",
                             action="store_true")
argument_parser.add_argument("-LMIN", "--min-literals-per-clause", type=int, default=3,
                             help="This is the minimum number of literals occurring per clause.")
argument_parser.add_argument("-LMAX", "--max-literals-per-clause", type=int, default=3,
                             help="This is the maximum number of literals occurring per clause.")
argument_parser.add_argument("-P", "--predicates", type=int, default=5,
                             help="This is the cardinality of the pool of predicates "
                                  "from which a random predicate is going to be picked.")
argument_parser.add_argument("-TP", "--tp", default=False,
                             help="Whether we generate output for the theorem prover.",
                             action="store_true")
argument_parser.add_argument("-VP", "--vampire", default=False,
                             help="Whether we generate output for vampire.",
                             action="store_true")

arguments = argument_parser.parse_args()

ARITY_MIN = arguments.arity_min
ARITY_MAX = arguments.arity_max
MIN_LITERALS_PER_CLAUSE = arguments.min_literals_per_clause
MAX_LITERALS_PER_CLAUSE = arguments.max_literals_per_clause


class ScottClauseRandomGenerator:
    def __init__(self, _is_alpha, _number_of_clauses, _has_equality, _variable_manager, _predicate_manager, _conjunctions=None):
        self.is_alpha = _is_alpha
        self.has_equality = _has_equality
        self.variables = _variable_manager.get_variables()
        assert len(self.variables) == 2
        number_of_literals_per_clause = random.randint(MIN_LITERALS_PER_CLAUSE, MAX_LITERALS_PER_CLAUSE)
        if _is_alpha:
            self.alpha = [[Predicate(_predicate_manager.choice(), 2, _variable_manager.sample(2),
                                     random.choice([False, True])) for _ in range(number_of_literals_per_clause)]
                          for _ in range(_number_of_clauses)]
        else:
            assert _number_of_clauses == 1
            if _conjunctions is None:
                raise ValueError("[Scott Clause]: for the beta clauses, the number of conjunctions has to be given")
            self.beta = [[[Predicate(_predicate_manager.choice(), 2, _variable_manager.sample(2), False)
                           for _ in range(1)]
                          for _ in range(1)] for _ in range(_conjunctions)]
        if self.has_equality:
            self.equality = Equality(self.variables[0], self.variables[1], not _is_alpha)

    def tp_output(self):
        result = None
        if self.is_alpha:
            equality_part = f" | ({self.equality.tp_output()})" if self.has_equality else ""
            result = f'''@{self.variables[0].lower()}@{self.variables[1].lower()}(
                   {'^'.join(["(" + '|'.join(predicate.tp_output() for predicate in clause) + ")"
                              for clause in self.alpha])}) 
                   {equality_part}'''
        else:
            equality_part = f" ^ ({self.equality.tp_output()})" if self.has_equality else ""
            result = f'''{'^'.join([f"@{self.variables[0].lower()}?{self.variables[1].lower()}("
                                    + '^'.join(["((" + '|'.join(predicate.tp_output() for predicate in clause) +
                                                ")" + equality_part + ")"
                                                for clause in clause_form]) + ")"
                                    for clause_form in self.beta])}'''
        return re.sub(r"\s+", "", result, flags=re.UNICODE)

    def vampire_output(self):
        result = None
        if self.is_alpha:
            equality_part = f" | ({self.equality.vampire_output()})" if self.has_equality else ""
            result = f'''![{self.variables[0].lower().capitalize()}]: ![{self.variables[1].lower().capitalize()}]: (
                   {'&'.join(['(' + '|'.join(predicate.vampire_output() for predicate in clause) + ')'
                              for clause in self.alpha])})
                   {equality_part}'''
        else:
            equality_part = f" & ({self.equality.vampire_output()})" if self.has_equality else ""
            result = f'''{'&'.join([f"![{self.variables[0].lower().capitalize()}]: "
                                    f"?[{self.variables[1].lower().capitalize()}]: ("
                                    + '&'.join(['((' + '|'.join(predicate.vampire_output() for predicate in clause) +
                                                ')' + equality_part + ')'
                                                for clause in clause_form]) + ")"
                                    for clause_form in self.beta])}'''
        return re.sub(r"\s+", "", result, flags=re.UNICODE)


if __name__ == "__main__":
    if arguments.tp or arguments.vampire:
        variable_manager = VariablesPool(1, 2)
        predicate_manager = PredicatesPool(1, arguments.predicates)
        alpha = ScottClauseRandomGenerator(True, arguments.alpha, arguments.has_equality,
                                           variable_manager, predicate_manager)
        beta = ScottClauseRandomGenerator(False, arguments.beta, arguments.has_equality, variable_manager,
                                          predicate_manager, _conjunctions=arguments.conjunctions)
        if arguments.tp:
            with open("input_tp.txt", "w") as th:
                th.write(f"(({alpha.tp_output()}) ^ ({beta.tp_output()}))")
                th.flush()
        if arguments.vampire:
            with open("input_vampire.txt", "w") as vampire:
                vampire.write(f"fof(one, negated_conjecture, ~(({alpha.vampire_output()}) & ({beta.vampire_output()}))).")
                vampire.flush()
            with open("input_vampire_statistics.txt", "w") as vampire:
                vampire.write(f"fof(one, negated_conjecture, (({alpha.vampire_output()}) & ({beta.vampire_output()}))).")
                vampire.flush()
    exit(0)

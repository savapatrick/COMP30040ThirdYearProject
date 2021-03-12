class Predicate:
    def __init__(self, _predicate_name, _arity, _terms, _negated):
        self.predicate_name = _predicate_name
        self.arity = _arity
        self.terms = _terms
        self.negated = _negated

    def tp_output(self):
        pass

    def vampire_output(self):
        pass

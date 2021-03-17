class Predicate:
    def __init__(self, _predicate_name, _arity, _terms, _negated):
        self.predicate_name = _predicate_name
        self.arity = _arity
        self.terms = _terms
        self.negated = _negated

    def tp_output(self):
        symbol = '~' if self.negated else ''
        return f"{symbol}{self.predicate_name.lower().capitalize()}({','.join([term.lower() for term in self.terms])})"

    def vampire_output(self):
        symbol = '~' if self.negated else ''
        return f'''{symbol}{self.predicate_name.lower()}({','.join(
            [term.lower().capitalize() if term.lower().startswith('v') else term.lower() for term in self.terms])})'''

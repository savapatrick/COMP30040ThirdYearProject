class Equality:
    def __init__(self, _variable_one, _variable_two, _negated):
        self.variable_one = _variable_one
        self.variable_two = _variable_two
        self.negated = _negated

    def tp_output(self):
        symbol = '!=' if self.negated else '='
        return f"{self.variable_one.lower()}{symbol}{self.variable_two.lower()}"

    def vampire_output(self):
        symbol = '!=' if self.negated else '='
        return f"{self.variable_one.lower().capitalize()}{symbol}{self.variable_two.lower().capitalize()}"

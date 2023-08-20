class Node():

    def __init__(self, state, parent, gCost, hCost):
        self.state = state
        self.parent = parent
        self.gCost = gCost
        self.hCost = hCost
        self.fCost = self.gCost + self.hCost
# Knapsack problem with a modification that adds the so-called "conflict".
Items may conflict with another item and therefore cannot be placed together in the backpack. The limitation is the capacity of the backpack and the very conflict that may prevent the backpack from reaching its full weight.
## Parameters
W: Backpack capacity.
weights: Vector containing the weights of the items.
values: Vector containing the item values.
conflicts: A vector of vectors representing conflicts between items.

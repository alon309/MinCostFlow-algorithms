# MinCostFlow-algorithms

## Overview
As part of an Algorithms course in a software engineering degree, I implemented 2 algorithms that solve the mincost flow problem:
- Cycle Canceling Algorithm
- Successive Shortest Path Algorithm

The implementation is in the c language and includes the implementation of additional smaller algorithms:
- Dijjstra
- Bellman-Ford
- Ford-Fulkerson

## MinCostFlow Problem
The min cost flow problem is a type of optimization problem that involves finding the cheapest way to transfer a certain amount of flow through a flow network, subject to certain constraints.
Our goal is to minimize the total cost of the flow in the flow network, which is defined as the sum of the costs of the arcs the flow crosses.

## Cycle Canceling Algorithm
- The Cycle Canceling algorithm is based on the idea of iterative improvement.
- start with a valid flow X, on which we will perform the improvements. X can be found by converting the problem to a max flow problem.
- After we have a legal flow X, we will use the residual network G(X) and look for circuits whose total cost is less than 0 (negative circles), for this we will use the Bellman-Ford algorithm.
- When we have found some negative circuit Y, we will increase the flow along the circuit with the minimum residual capacity and thus eliminate it.
- The algorithm stops as soon as G(X) contains no more negative circles

## Successive Shortest Path Algorithm
- Unlike the Cycle canceling algorithm, this algorithm does not start from a legal flow and works so that it is optimal, rather it starts from flow 0 and each time increases flow while maintaining optimality and legality.
- New concepts are defined (node potential, reduced cost, pseudo-flow) and according to them the nodes are divided into 2 sets: E and D
- In each iteration we will find a node from group E and a node from group D, so that there is a path in the residual network between them, and we will add a flow along the "cheapest" path in terms of the reduced cost. The algorithm stops when set E is empty.

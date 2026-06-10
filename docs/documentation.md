# Documentation

## `gcd(a, b)`

this function returns the GCD (Greatest Common Denominator) of two integers a and b

## `lcm(a, b)`

this function returns the LCM (Least Common Multiple) of two integers a and b

## `mod_pow(a, b, m)`

this fuction returns (a^b)%m
it uses a fast exponentiation algorithm to greatly reduce the compute time (from normal O(b) to O(log b)) which can be useful for example for hashing algorithms

## `sieve(n)`

this function returns all of the prime numbers greater or equal to n
it works by using an ancient algorithm called the Sieve of Eratosthenes

## `binary_search(val, x)`

this function returns the index of the very first occurence of the number x in the list val, or -1 if it can not be found.

## `prefix_sum(val)`

this function, given the list val, returns a list containing its prefix sums, which means that on the i-th element of the result contains the sum 
of the i+1 first elements. It is useful for quickly finding the sum of the a thru b-th elements by calculating result[b] - result[a-1]

## `bfs(graph, v)`

this function traverses a graph where:
graph is a list of lists where its i-th element is a list of the neighbors of the i-th vertex, for example:
[[1],[]] means a graph made from 0 and 1 with an edge going from 0 to 1 (but not vice versa).
It does so using a Breadth-First algorithm and returns a list of distances from the given vertex v.

## `dfs(graph, v)`

this function traverses a graph where:
graph is a list of lists where its i-th element is a list of the neighbors of the i-th vertex, for example:
[[1],[]] means a graph made from 0 and 1 with an edge going from 0 to 1 (but not vice versa).
It does so using a Depth-First algorithm and returns a list containing the order of visits from the given vertex v.

## `toposort(graph)`

this function sorts a graph without cycles where:
graph is a list of lists where its i-th element is a list of the neighbors of the i-th vertex, for example:
[[1],[]] means a graph made from 0 and 1 with an edge going from 0 to 1 (but not vice versa).
It does so using topological sorting meaning that a vertex can only be added to the list once all of its predecessors were as well
It returns a topologically sorted list of vertices.

## `lis_length(tab)`

This function returns the length of the Longest Increasing Sequence in a given list tab, using an O(n*log n) approach.

## `kmp_search(text, keyword)`

This function returns the indexes of occurences of the keyword in a given text using Knuth-Morris-Pratts algorithm.
The algorithm works in O(n+m) instead of a naive O(n*m) approach

## `test_your_function(func1, func2, n)`

Tests your function (func1) against this package's version on n randomly generated tests

## `UnionFind`

This class implements a UnionFind data structure (known otherwise as Find and Union)
This structure stores a partition of elements into disjoint sets.
It uses path compression and union by set size to improve performance.

### `UnionFind.__init__(n)`

Creates a UnionFind structure for elements from 0 to n - 1.

### `UnionFind.find(x)`

Returns the representative of the set containing a given number x.

### `UnionFind.union(a, b)`

Merges the sets containing a and b.
It returns True if the sets were successfully merged and False if not (meaning that the elements were already in the same one)

### `UnionFind.connected(a, b)`

Checks whether a and b belong to the same set.
Return True if a and b are in the same set, False otherwise.

### `UnionFind.size(x)`

Returns the size of the set containing a given number x.

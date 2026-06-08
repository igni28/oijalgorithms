from . import cfunkcje


def gcd(a, b):
    return cfunkcje.gcd(a, b)


def lcm(a, b):
    return cfunkcje.lcm(a, b)


def mod_pow(a, b, m):
    return cfunkcje.mod_pow(a, b, m)


def sieve(n):
    return cfunkcje.sieve(n)


def binary_search(val, x):
    return cfunkcje.binary_search(val, x)


def prefix_sum(val):
    return cfunkcje.prefix_sum(val)


def bfs(graph, v):
    return cfunkcje.bfs(graph, v)


def dfs(graph, v):
    return cfunkcje.dfs(graph, v)


def toposort(graph):
    return cfunkcje.toposort(graph)


def lis_length(tab):
    return cfunkcje.lis_length(tab)


def kmp_search(text, keyword):
    return cfunkcje.kmp_search(text, keyword)


UnionFind = cfunkcje.UnionFind
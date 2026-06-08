import unittest

import oijalgorithms


class TestAlgorytmyOIJ(unittest.TestCase):
    def test_gcd(self):
        self.assertEqual(oijalgorithms.gcd(40, 15), 5)
        self.assertEqual(oijalgorithms.gcd(-40, 15), 5)
        self.assertEqual(oijalgorithms.gcd(0, 5), 5)

    def test_lcm(self):
        self.assertEqual(oijalgorithms.lcm(10, 15), 30)
        self.assertEqual(oijalgorithms.lcm(-10, 15), 30)
        self.assertEqual(oijalgorithms.lcm(0, 67), 0)

    def test_mod_pow(self):
        self.assertEqual(oijalgorithms.mod_pow(2, 10, 1000), 24)
        self.assertEqual(oijalgorithms.mod_pow(3, 10, 444), 441)
        self.assertEqual(oijalgorithms.mod_pow(5, 0, 365), 1)

    def test_sieve(self):
        self.assertEqual(oijalgorithms.sieve(1), [])
        self.assertEqual(oijalgorithms.sieve(2), [2])
        self.assertEqual(oijalgorithms.sieve(20), [2, 3, 5, 7, 11, 13, 17, 19])

    def test_binary_search(self):
        self.assertEqual(oijalgorithms.binary_search([1, 3, 3, 3, 9], 3), 1)
        self.assertEqual(oijalgorithms.binary_search([2, 2, 2, 2], 2), 0)
        self.assertEqual(oijalgorithms.binary_search([1, 4, 6, 9], 5), -1)
        self.assertEqual(oijalgorithms.binary_search([], 1), -1)

    def test_prefix_sum(self):
        self.assertEqual(oijalgorithms.prefix_sum([]), [])
        self.assertEqual(oijalgorithms.prefix_sum([2, 4, 1, 9]), [2, 6, 7, 16])
        self.assertEqual(oijalgorithms.prefix_sum([-2, 5, -1]), [-2, 3, 2])

    def test_bfs(self):
        graph = [
            [1, 2],
            [0, 3],
            [0],
            [1],
            [],
        ]
        self.assertEqual(oijalgorithms.bfs(graph, 0), [0, 1, 1, 2, -1])

    def test_dfs(self):
        graph = [
            [1, 2],
            [0, 3],
            [0],
            [1],
        ]
        self.assertEqual(oijalgorithms.dfs(graph, 0), [0, 1, 3, 2])

    def test_toposort(self):
        graph = [
            [1, 2],
            [3],
            [3],
            [],
        ]
        order = oijalgorithms.toposort(graph)
        self.assertEqual(order, [0, 1, 2, 3])

    def test_toposort_cycle(self):
        graph = [[1], [0]]
        with self.assertRaises(RuntimeError):
            oijalgorithms.toposort(graph)

    def test_lis_length(self):
        self.assertEqual(oijalgorithms.lis_length([3, 1, 2, 5, 4]), 3)
        self.assertEqual(oijalgorithms.lis_length([4, 3, 2, 1]), 1)
        self.assertEqual(oijalgorithms.lis_length([1, 2, 2, 3]), 3)

    def test_kmp_search(self):
        self.assertEqual(oijalgorithms.kmp_search("ababcabc", "abc"), [2, 5])
        self.assertEqual(oijalgorithms.kmp_search("aaaa", "aa"), [0, 1, 2])

    def test_union_find(self):
        uf = oijalgorithms.UnionFind(5)
        self.assertEqual(uf.size(0), 1)
        self.assertFalse(uf.connected(0, 1))
        self.assertTrue(uf.union(0, 1))
        self.assertTrue(uf.connected(0, 1))
        self.assertEqual(uf.size(0), 2)
        self.assertFalse(uf.union(0, 1))
        self.assertTrue(uf.union(3, 4))
        self.assertFalse(uf.connected(1, 4))


if __name__ == "__main__":
    unittest.main()

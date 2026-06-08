#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdlib.h>
#include <string.h>

long long gcd_c(long long a, long long b) {
    a = llabs(a);
    b = llabs(b);


    while (b != 0) {
        long long r = a % b;
        a = b;
        b = r;
    }

    return a;
}

long long lcm_c(long long a, long long b) {
    if (a == 0 || b == 0) return 0;

    long long wyn = (a / gcd_c(a, b)) * b;
    return llabs(wyn);
}

long long mod_pow_c(long long a, long long b, long long mod) {
    long long wyn = 1;

    if (mod == 1) return 0;

    a = a % mod;

    while (b > 0) {
        if (b % 2 == 1) wyn = (wyn * a) % mod;
        a = (a * a) % mod;
        b = b / 2;
    }

    return wyn;
}

PyObject* gcd(PyObject* self, PyObject* args) {
    long long a, b;
    
    if (!PyArg_ParseTuple(args, "LL", &a, &b)) return NULL;
    return PyLong_FromLongLong(gcd_c(a, b));
}

PyObject* lcm(PyObject* self, PyObject* args) {
    long long a, b;

    if (!PyArg_ParseTuple(args, "LL", &a, &b)) return NULL;
    return PyLong_FromLongLong(lcm_c(a, b));
}

PyObject* mod_pow(PyObject* self, PyObject* args) {
    long long a, b, m;

    if (!PyArg_ParseTuple(args, "LLL", &a, &b, &m)) return NULL;

    if (b < 0) {
        PyErr_Format(PyExc_RuntimeError, "The exponent should be a non-negative number");
        return NULL;
    }

    if (m <= 0) {
        PyErr_SetString(PyExc_RuntimeError, "The modulo should be positive");
        return NULL;
    }

    return PyLong_FromLongLong(mod_pow_c(a, b, m));
}

int* sieve_c(Py_ssize_t n) {
    int* is_prime = malloc(((size_t)n + 1) * sizeof(int));

    for (Py_ssize_t i = 0; i <= n; i++) {
        is_prime[i] = 1;
    }

    is_prime[0] = 0;is_prime[1] = 0;

    for (Py_ssize_t i = 2; i * i <= n; i++) {
        if (is_prime[i]) {
            for (Py_ssize_t j = i * i; j <= n; j += i) {
                is_prime[j] = 0;
            }
        }
    }

    return is_prime;
}

PyObject* sieve(PyObject* self, PyObject* args) {
    long long n1;
    int* is_prime;
    PyObject* wyn;

    if (!PyArg_ParseTuple(args, "L", &n1)) return NULL;
    if (n1 < 2) return PyList_New(0);

    Py_ssize_t n = (Py_ssize_t)n1;
    is_prime = sieve_c(n);
    wyn = PyList_New(0);

    for (Py_ssize_t i = 2; i <= n; i++) {
        if (is_prime[i]) {
            PyObject* val = PyLong_FromSsize_t(i);
            PyList_Append(wyn, val);
            Py_DECREF(val);
        }
    }

    free(is_prime);
    return wyn;
}

Py_ssize_t binsearch(long long* val, Py_ssize_t n, long long x) {
    Py_ssize_t l = 0;
    Py_ssize_t p = n - 1;
    Py_ssize_t wyn = -1;

    while (l <= p) {
        Py_ssize_t s = l + (p - l) / 2;

        if (val[s] == x) {
            wyn = s;
            p = s - 1;
        } else if (val[s] < x) {
            l = s + 1;
        } else {
            p = s - 1;
        }
    }

    return wyn;
}

PyObject* binary_search(PyObject* self, PyObject* args) {
    PyObject* tab;
    Py_ssize_t n;
    Py_ssize_t wyn;
    long long x;
    long long* val = NULL;

    if (!PyArg_ParseTuple(args, "OL", &tab, &x)) return NULL;

    if (!PyList_Check(tab)) {
        PyErr_Format(PyExc_RuntimeError, "Expected a Python list");
        return NULL;
    }

    n = PyList_GET_SIZE(tab);

    if (n > 0) val = malloc((size_t)n * sizeof(long long));

    for (Py_ssize_t i = 0; i < n; i++) {
        PyObject* v = PyList_GET_ITEM(tab, i);
        val[i] = PyLong_AsLongLong(v);
    }

    wyn = binsearch(val, n, x);
    free(val);

    return PyLong_FromSsize_t(wyn);
}

long long* sumy_prefix(const long long* val, Py_ssize_t n) {
    long long* wyn = malloc((size_t)n * sizeof(long long));

    if (n == 0) return wyn;

    wyn[0] = val[0];

    for (Py_ssize_t i = 1; i < n; i++) {
        wyn[i] = wyn[i - 1] + val[i];
    }

    return wyn;
}

PyObject* prefix_sum(PyObject* self, PyObject* args) {
    PyObject* tab;
    Py_ssize_t n;
    long long* val = NULL;
    long long* prefix = NULL;
    PyObject* wyn;

    if (!PyArg_ParseTuple(args, "O", &tab)) return NULL;

    if (!PyList_Check(tab)) {
        PyErr_Format(PyExc_RuntimeError, "Expected a Python list");
        return NULL;
    }

    n = PyList_GET_SIZE(tab);

    if (n > 0) val = malloc((size_t)n * sizeof(long long));

    for (Py_ssize_t i = 0; i < n; i++) {
        PyObject* x = PyList_GET_ITEM(tab, i);
        val[i] = PyLong_AsLongLong(x);
    }

    prefix = sumy_prefix(val, n);
    free(val);

    wyn = PyList_New(0);

    for (Py_ssize_t i = 0; i < n; i++) {
        PyObject* value = PyLong_FromLongLong(prefix[i]);
        PyList_Append(wyn, value);
        Py_DECREF(value);
    }

    free(prefix);
    return wyn;
}


typedef struct {
    Py_ssize_t n;
    Py_ssize_t* deg;
    Py_ssize_t** sas;
} Graph;

static void free_graph(Graph* g) {
    for (Py_ssize_t i = 0; i < g->n; i++) {
        free(g->sas[i]);
    }
    free(g->sas);
    free(g->deg);
}

static int czy_graf(PyObject* graph, Graph* g) {

    if (!PyList_Check(graph)) {
        PyErr_Format(PyExc_RuntimeError, "Expected a list of lists");
        return 1;
    }

    g->n = PyList_GET_SIZE(graph);
    g->deg = malloc((size_t)g->n * sizeof(Py_ssize_t));
    g->sas = calloc((size_t)g->n, sizeof(Py_ssize_t*));

    for (Py_ssize_t i = 0; i < g->n; i++) {
        PyObject* w = PyList_GET_ITEM(graph, i);

        if (!PyList_Check(w)) {
            free_graph(g);
            PyErr_Format(PyExc_RuntimeError, "Expected a list of lists");
            return 1;
        }

        g->deg[i] = PyList_GET_SIZE(w);
        g->sas[i] = malloc((size_t)g->deg[i] * sizeof(Py_ssize_t));

        for (Py_ssize_t j = 0; j < g->deg[i]; j++) {
            long long u = PyLong_AsLongLong(PyList_GET_ITEM(w, j));

            if (u < 0 || u >= g->n) {
                free_graph(g);
                PyErr_Format(PyExc_RuntimeError, "Expected a Positive value");
                return 1;
            }

            g->sas[i][j] = (Py_ssize_t)u;
        }
    }

    return 0;
} //funkcja zamienia 

void bfs_c(Graph* g, Py_ssize_t s, long long* dist) {
    Py_ssize_t* queue = malloc((size_t)g->n * sizeof(Py_ssize_t));
    Py_ssize_t x = 0;
    Py_ssize_t y = 0;

    for (Py_ssize_t i = 0; i < g->n; i++) {
        dist[i] = -1;
    }

    dist[s] = 0;
    queue[y++] = s;

    while (x < y) {
        Py_ssize_t v = queue[x++];

        for (Py_ssize_t i = 0; i < g->deg[v]; i++) {
            Py_ssize_t u = g->sas[v][i];

            if (dist[u] == -1) {
                dist[u] = dist[v] + 1;
                queue[y++] = u;
            }
        }
    }

    free(queue);
} 

void dfs_c(Graph* g, Py_ssize_t s, Py_ssize_t* kol, Py_ssize_t* count) {
    Py_ssize_t* stack = malloc((size_t)g->n * sizeof(Py_ssize_t));
    int* visited = calloc((size_t)g->n, sizeof(int));
    Py_ssize_t top = 0;

    *count = 0;
    visited[s] = 1;
    stack[top++] = s;

    while (top > 0) {
        Py_ssize_t v = stack[--top];
        kol[(*count)++] = v;

        for (Py_ssize_t i = g->deg[v]; i > 0; i--) {
            Py_ssize_t u = g->sas[v][i - 1];

            if (!visited[u]) {
                visited[u] = 1;
                stack[top++] = u;
            }
        }
    }

    free(stack);
    free(visited);
}

PyObject* bfs(PyObject* self, PyObject* args) {
    PyObject* graph;
    Graph g;
    Py_ssize_t start;
    long long* dist;
    PyObject* wyn;

    if (!PyArg_ParseTuple(args, "On", &graph, &start)) return NULL;

    if (czy_graf(graph, &g)) return NULL;

    if (start < 0 || start >= g.n) {
        free_graph(&g);
        PyErr_Format(PyExc_RuntimeError, "Select a viable starting vertice");
        return NULL;
    }

    dist = malloc(sizeof(long long) * (size_t)g.n );
    bfs_c(&g, start, dist);

    wyn = PyList_New(g.n);

    for (Py_ssize_t i = 0; i < g.n; i++) {
        PyList_SET_ITEM(wyn, i, PyLong_FromLongLong(dist[i]));
    }

    free(dist);
    free_graph(&g);

    return wyn;
}

PyObject* dfs(PyObject* self, PyObject* args) {
    PyObject* graph;
    Graph g;
    Py_ssize_t start;
    Py_ssize_t* kol;
    Py_ssize_t count;
    PyObject* wyn;

    if (!PyArg_ParseTuple(args, "On", &graph, &start)) return NULL;
    if (czy_graf(graph, &g)) return NULL;

    if (start < 0 || start >= g.n) {
        free_graph(&g);
        PyErr_Format(PyExc_RuntimeError, "Select a viable starting vertice");
        return NULL;
    }

    kol = malloc((size_t)g.n * sizeof(Py_ssize_t));
    dfs_c(&g, start, kol, &count);

    wyn = PyList_New(count);

    for (Py_ssize_t i = 0; i < count; i++) {
        PyList_SET_ITEM(wyn, i, PyLong_FromSsize_t(kol[i]));
    }

    free(kol);
    free_graph(&g);

    return wyn;
}




void toposort_c(Graph* g, Py_ssize_t* kol, Py_ssize_t* count) {
    Py_ssize_t* indeg = calloc((size_t)g->n, sizeof(Py_ssize_t));
    Py_ssize_t* queue = malloc((size_t)g->n * sizeof(Py_ssize_t));
    Py_ssize_t head = 0;
    Py_ssize_t tail = 0;

    *count = 0;

    for (Py_ssize_t v = 0; v < g->n; v++) {
        for (Py_ssize_t i = 0; i < g->deg[v]; i++) {
            indeg[g->sas[v][i]]++;
        }
    }

    for (Py_ssize_t i = 0; i < g->n; i++) {
        if (indeg[i] == 0) queue[tail++] = i;
    }

    while (head < tail) {
        Py_ssize_t v = queue[head++];
        kol[(*count)++] = v;

        for (Py_ssize_t i = 0; i < g->deg[v]; i++) {
            Py_ssize_t u = g->sas[v][i];
            indeg[u]--;
            if (indeg[u] == 0) queue[tail++] = u;
        }
    }

    free(indeg);
    free(queue);
}

PyObject* toposort(PyObject* self, PyObject* args) {
    PyObject* graph;
    Graph g;
    Py_ssize_t* kol;
    Py_ssize_t count;
    PyObject* wyn;

    if (!PyArg_ParseTuple(args, "O", &graph)) return NULL;

    if (czy_graf(graph, &g)) return NULL;

    kol = malloc((size_t)g.n * sizeof(Py_ssize_t));
    toposort_c(&g, kol, &count);

    if (count != g.n) {
        free(kol);
        free_graph(&g);
        PyErr_Format(PyExc_RuntimeError, "Expected a graph without a cycle, sorting is impossible");
        return NULL;
    }

    wyn = PyList_New(count);

    for (Py_ssize_t i = 0; i < count; i++) {
        PyList_SET_ITEM(wyn, i, PyLong_FromSsize_t(kol[i]));
    }

    free(kol);
    free_graph(&g);

    return wyn;
}

//lis
size_t lis_length_c(long long* tab, size_t n) {
    long long* helper = malloc(n*sizeof(long long));
    size_t wyn = 1;


    for(size_t i = 0;i<=n-1;i++) {
        long long x = tab[i];
        if(i == 0) {
            helper[0] = x;
            continue;
        }
        
        size_t l = 0;
        size_t p = wyn;
        size_t s = 0;
        while(l < p) {
            s = (p+l)/2;
            if(helper[s] >= x) {
                p = s;
            }
            else {
                l = s+1;
            }
        }
        helper[l] = x;
        if(l == wyn) {
            wyn++;
        }
        

    }

    free(helper);
    return wyn;
}


PyObject* lis_length(PyObject* self, PyObject* args) {
    PyObject* tab;
    Py_ssize_t n;
    long long* val;
    size_t wynik;

    if (!PyArg_ParseTuple(args, "O", &tab)) 
        return NULL;
    if (!PyList_Check(tab)) {
        PyErr_Format(PyExc_RuntimeError, "Expected a Python list");
        return NULL;
}

    n = PyList_GET_SIZE(tab);
    val = malloc((size_t)n * sizeof(long long));

    for (Py_ssize_t i = 0; i < n; i++) {
        val[i] = PyLong_AsLongLong(PyList_GET_ITEM(tab, i));
    }

    wynik = lis_length_c(val, (size_t)n);

    free(val);

    return PyLong_FromSize_t(wynik);
}




size_t* kmp_search_c(char* text, size_t n, char* keyword, size_t m, size_t* count) {
    size_t* helper = calloc(m, sizeof(size_t));
    size_t* positions = malloc((n + 1) * sizeof(size_t));
    size_t j = 0;
    *count = 0;


    for (size_t i = 1; i < m; i++) {
        j = helper[i - 1];

        while (j > 0 && keyword[i] != keyword[j]) {
            j = helper[j - 1];
        }

        if (keyword[i] == keyword[j]) {
            j++;
        }

        helper[i] = j;
    }

    j = 0;

    for (size_t i = 0; i < n; i++) {
        while (j > 0 && text[i] != keyword[j]) {
            j = helper[j - 1];
        }

        if (text[i] == keyword[j]) {
            j++;
        }

        if (j == m) {
            positions[*count] = i - m + 1;
            (*count)++;

            j = helper[j - 1];
        }
    }

    free(helper);
    return positions;
}

PyObject* kmp_search(PyObject* self, PyObject* args) {
    char* text;
    char* keyword;
    Py_ssize_t n_py;
    Py_ssize_t m_py;
    size_t count = 0;
    size_t* positions;
    PyObject* wyn;

    if (!PyArg_ParseTuple(args, "s#s#", &text, &n_py, &keyword, &m_py)) {
        return NULL;
    }

    size_t n = (size_t)n_py;
    size_t m = (size_t)m_py;

    positions = kmp_search_c(text, n, keyword, m, &count);

    wyn = PyList_New(0);

    for (size_t i = 0; i < count; i++) {
        PyObject* val = PyLong_FromSize_t(positions[i]);

        PyList_Append(wyn, val);
        Py_DECREF(val);
    }

    free(positions);

    return wyn;
}









typedef struct {
    PyObject_HEAD
    Py_ssize_t n;
    Py_ssize_t* father;
    Py_ssize_t* set_size;
} UnionFindObject;

PyTypeObject UnionFindType = {
    PyVarObject_HEAD_INIT(NULL, 0)
};

Py_ssize_t uf_find_root(UnionFindObject* self, Py_ssize_t x) {
    if (self->father[x] != x) {
        self->father[x] = uf_find_root(self, self->father[x]);
    }
    return self->father[x];
}

static int uf_check(UnionFindObject* self, Py_ssize_t x) {
    if (x < 0 || x >= self->n) {
        PyErr_Format(PyExc_RuntimeError, "Expected proper indices");
        return 0;
    }
    return 1;
}



static int UnionFind_init(UnionFindObject* self, PyObject* args, PyObject* kwds) {
    Py_ssize_t n;

    if (!PyArg_ParseTuple(args, "n", &n)) return -1;

    if (n <= 0) {
        PyErr_Format(PyExc_ValueError, "Expected a positive value");
        return -1;
    }

    self->n = n;
    
    self->set_size = malloc(sizeof(Py_ssize_t) * (size_t)n);
    self->father = malloc(sizeof(Py_ssize_t) * (size_t)n);

    for (Py_ssize_t i = 0; i < n; i++) {
        self->father[i] = i;
        self->set_size[i] = 1;
    }

    return 0;
}

static void UnionFind_dealloc(UnionFindObject* self) {
    free(self->father);
    free(self->set_size);
    Py_TYPE(self)->tp_free((PyObject*)self);
}




static PyObject* UnionFind_find(UnionFindObject* self, PyObject* args) {
    Py_ssize_t x;

    if (!PyArg_ParseTuple(args, "n", &x)) return NULL;
    if (!uf_check(self, x)) return NULL;

    return PyLong_FromSsize_t(uf_find_root(self, x));
}

static PyObject* UnionFind_union(UnionFindObject* self, PyObject* args) {
    Py_ssize_t a, b;
    Py_ssize_t roota, rootb;

    if (!PyArg_ParseTuple(args, "nn", &a, &b)) return NULL;
    if (!uf_check(self, a) || !uf_check(self, b)) return NULL;

    roota = uf_find_root(self, a);
    rootb = uf_find_root(self, b);

    if (roota == rootb) Py_RETURN_FALSE;

    if (self->set_size[roota] < self->set_size[rootb]) {
        Py_ssize_t tmp = roota;
        roota = rootb;
        rootb = tmp;
    }

    self->father[rootb] = roota;
    self->set_size[roota] += self->set_size[rootb];

    Py_RETURN_TRUE; //returns TRUE when two sets were succesfully connected
} 



PyObject* UnionFind_connected(UnionFindObject* self, PyObject* args) {
    Py_ssize_t a, b;

    if (!PyArg_ParseTuple(args, "nn", &a, &b)) return NULL;
    if (!uf_check(self, a) || !uf_check(self, b)) return NULL;

    if (uf_find_root(self, a) == uf_find_root(self, b)) Py_RETURN_TRUE;
    Py_RETURN_FALSE;
}

PyObject* UnionFind_size(UnionFindObject* self, PyObject* args) {
    Py_ssize_t x;
    Py_ssize_t root;

    if (!PyArg_ParseTuple(args, "n", &x)) return NULL;
    if (!uf_check(self, x)) return NULL;

    root = uf_find_root(self, x);
    return PyLong_FromSsize_t(self->set_size[root]);
}

static PyMethodDef UnionFind_methods[] = {
    {"find", (PyCFunction)UnionFind_find, METH_VARARGS, "Returns the representative of the set"},
    {"union", (PyCFunction)UnionFind_union, METH_VARARGS, "Connects two sets to each other"},
    {"connected", (PyCFunction)UnionFind_connected, METH_VARARGS, "Checks whether two elements are in the same set"},
    {"size", (PyCFunction)UnionFind_size, METH_VARARGS, "Returns the size of the current set"},
    {NULL, NULL, 0, NULL}
};

static PyMethodDef module_methods[] = {
    {"gcd", gcd, METH_VARARGS, "Calculates the GCD of two numbers"},
    {"lcm", lcm, METH_VARARGS, "Calculates the LCM of two numbers"},
    {"mod_pow", mod_pow, METH_VARARGS, "Calculates a^b modulo m."},
    {"sieve", sieve, METH_VARARGS, "Returns all prime numbers smaller than n"},
    {"binary_search", binary_search, METH_VARARGS, "Returns the index of the first appearance of a number "},
    {"prefix_sum", prefix_sum, METH_VARARGS, "Returns a prefix sums table"},
    {"bfs", bfs, METH_VARARGS, "Returns a list of distances from a given point"},
    {"dfs", dfs, METH_VARARGS, "Returns a list of vertices in the visiting order"},
    {"toposort", toposort, METH_VARARGS, "Returns a list of topologically sorted graph vertices"},
    {"lis_length", lis_length, METH_VARARGS, "Returns the length of the LIS (longest increasing subsequence) of a list"},
    {"kmp_search", kmp_search, METH_VARARGS, "Returns the indices of appereances of the given keyword in a string"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef module_def = {
    PyModuleDef_HEAD_INIT,
    "oijalgorithms.cfunkcje",
    "Algorithms for the Junior Polish Olympiad In Informatics implemented in C",
    -1,
    module_methods
};

PyMODINIT_FUNC PyInit_cfunkcje() {
    PyObject* module;

    UnionFindType.tp_name = "oijalgorithms.cfunkcje.UnionFind";
    UnionFindType.tp_basicsize = sizeof(UnionFindObject);
    UnionFindType.tp_flags = Py_TPFLAGS_DEFAULT;
    UnionFindType.tp_doc = "THE Union-Find graph Structure";
    UnionFindType.tp_new = PyType_GenericNew;
    UnionFindType.tp_init = (initproc)UnionFind_init;
    UnionFindType.tp_dealloc = (destructor)UnionFind_dealloc;
    UnionFindType.tp_methods = UnionFind_methods;

    if (PyType_Ready(&UnionFindType) < 0) {
        Py_DECREF(module);
        return NULL;
    }

    module = PyModule_Create(&module_def);
    if (!module) return NULL;

    Py_INCREF(&UnionFindType);
    if (PyModule_AddObject(module, "UnionFind", (PyObject*)&UnionFindType) < 0) {
        Py_DECREF(&UnionFindType);
        Py_DECREF(module);
        return NULL;
    }

    return module;
}

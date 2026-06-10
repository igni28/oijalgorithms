#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
        PyErr_Format(PyExc_RuntimeError, "The modulo should be positive");
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
//generatory testów i testowanie

static long long random_ll(long long a, long long b) {
    return (a + (long long)(rand() % (int)(b - a + 1)));
}

static PyObject* make_tuple1(PyObject* a) {
    if (!a) return NULL;

    PyObject* tuple = PyTuple_New(1);

    if (!tuple) {
        Py_DECREF(a);
       return NULL;
    }

    PyTuple_SET_ITEM(tuple, 0, a);
    return tuple;
}

static PyObject* make_tuple2(PyObject* a, PyObject* b) {
    if (!a || !b) return NULL;

    PyObject* tuple = PyTuple_New(2);

    if (!tuple) {
        Py_DECREF(a);
        Py_DECREF(b);
        return NULL;
    }

    PyTuple_SET_ITEM(tuple, 0, a);
    PyTuple_SET_ITEM(tuple, 1, b);

    return tuple;
}

static PyObject* random_int_list(Py_ssize_t n, long long a, long long b) {
    PyObject* list = PyList_New(n);

    for (Py_ssize_t i = 0; i < n; i++) {
        PyObject* x = PyLong_FromLongLong(random_ll(a, b));
        PyList_SET_ITEM(list, i, x);
    }

    return list;
}

static PyObject* increasing_int_list(Py_ssize_t n) {
    PyObject* list = PyList_New(n);


    long long licznik = (rand() % 10);

    for (Py_ssize_t i = 0; i < n; i++) {
        licznik += rand() % 5;

        PyObject* x = PyLong_FromLongLong(licznik);

        PyList_SET_ITEM(list, i, x);
    }

    return list;
}


static PyObject* random_lower_string(Py_ssize_t n) {
    if (n == 0) {
        return PyUnicode_FromStringAndSize("", 0);
    }

    char* tab = malloc((size_t)n);

    if (!tab) {
        return PyErr_NoMemory();
    }

    for (Py_ssize_t i = 0; i < n; i++) {
        tab[i] = (char)('a' + rand() % 6);
    }

    PyObject* result = PyUnicode_FromStringAndSize(tab, n);
    free(tab);

    return result;
}

static PyObject* generate_args_for_function(const char* name) {

    if (strcmp(name, "gcd") == 0 || strcmp(name, "lcm") == 0) {
        long long a = random_ll(-1000, 1000);
        long long b = random_ll(-1000, 1000);

        return Py_BuildValue("(LL)", a, b);
    }

    if (strcmp(name, "mod_pow") == 0) {
        long long a = random_ll(0, 1000);
        long long b = random_ll(0, 1000);
        long long mod = random_ll(1, 1000);

        return Py_BuildValue("(LLL)", a, b, mod);
    }

    if (strcmp(name, "sieve") == 0) {
        Py_ssize_t n = (Py_ssize_t)random_ll(0, 10000);

        return Py_BuildValue("(n)", n);
    }

    if (strcmp(name, "lis_length") == 0 || strcmp(name, "prefix_sum") == 0) {
        Py_ssize_t n = (Py_ssize_t)random_ll(1, 1000);
        PyObject* list = random_int_list(n, -10, 100);

        return make_tuple1(list);
    }

    if (strcmp(name, "binary_search") == 0) {
        Py_ssize_t n = (Py_ssize_t)random_ll(0, 1000);

        PyObject* list = increasing_int_list(n);
        PyObject* x = PyLong_FromLongLong(random_ll(-10, 1010));

        return make_tuple2(list, x);
    }

    if (strcmp(name, "kmp_search") == 0) {
        PyObject* text = random_lower_string((Py_ssize_t)random_ll(1, 10000));
        PyObject* pattern = random_lower_string((Py_ssize_t)random_ll(1, 100));

        return make_tuple2(text, pattern);
    }

    PyErr_Format(PyExc_RuntimeError, "No generator found for The function '%s'", name);
    return NULL;
}

static PyObject* test_your_function(PyObject* self, PyObject* args) {
    PyObject* func1;
    PyObject* func2;
    Py_ssize_t n;
    if (!PyArg_ParseTuple(args, "OOn", &func1, &func2, &n)) {
        return NULL;
    }

    if (!PyCallable_Check(func1) || !PyCallable_Check(func2)) {
        PyErr_Format(PyExc_RuntimeError, "Expected two functions");
        return NULL;
    }

    if (n < 1) {
        PyErr_Format(PyExc_RuntimeError, "n must be a positive integer");
        return NULL;
    }
    srand((unsigned int)time(NULL));

    PyObject* functionname = PyObject_GetAttrString(func2, "__name__");

    if (!functionname) {
        return NULL;
    }

    const char* name = PyUnicode_AsUTF8(functionname);

    for (Py_ssize_t i = 0; i < n; i++) {
        PyObject* test_args = generate_args_for_function(name);

        PyObject* result1 = PyObject_CallObject(func1, test_args);
        PyObject* result2 = PyObject_CallObject(func2, test_args);

        int equal = PyObject_RichCompareBool(result1, result2, Py_EQ);

        if (equal < 0) {
            Py_DECREF(test_args);
            Py_DECREF(result1);
            Py_DECREF(result2);
            Py_DECREF(functionname);
            return NULL;
        }

        
        

        if (!equal) {
            PyObject* info = Py_BuildValue(
                "{s:O,s:n,s:s,s:O,s:O,s:O}",
                "ok", Py_False,
                "test_number", i + 1,
                "function", name,
                "args", test_args,
                "result1", result1,
                "result2", result2
            );

            Py_DECREF(test_args);
            Py_DECREF(result1);
            Py_DECREF(result2);
            Py_DECREF(functionname);

            return info;
        }

        //debugg
        //PyObject* result = Py_BuildValue(
        //"{s:O,s:n,s:s}",
        //"BLASAAAAAAAAAAAAH", Py_True,
        //"tests", n,
        //"function", name
        //);
        //Py_DECREF(functionname);
        //return result;

        Py_DECREF(test_args);
        Py_DECREF(result1);
        Py_DECREF(result2);
    }

    PyObject* result = Py_BuildValue(
        "{s:O,s:n,s:s}",
        "ok", Py_True,
        "tests", n,
        "function", name
    );

    Py_DECREF(functionname);

    return result;
}









//FInd and Union
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
        PyErr_Format(PyExc_RuntimeError, "Expected a positive value");
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
    {"test_your_function", test_your_function, METH_VARARGS, "Tests your function against the ones above on n randomly generated tests"},
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

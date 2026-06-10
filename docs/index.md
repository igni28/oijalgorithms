# oijalgorithms

oijalgorithms is a Python package written in C containing algorithms often used in the Junior Polish Olympiad in Informatics as well as its High School counterpart. It was made for learning by writing algorithms in Python or using them as a shortcut instead of implementing them yourself.

## Installation

The package requires Python 3 and a C compiler.

### Download from GitHub

```bash
git clone https://github.com/igni28/oijalgorithms.git
cd oijalgorithms
python -m pip install -e .
```


### Check if it works

Run Python and import the package:

```python
import oijalgorithms
```

Then check if it works, for example by calling the gcd function:
```python
print(oijalgorithms.gcd(20, 15))
```

Expected output:
 
```text
5
```
There are, of course more complicated algorithms included like this one:
```python
print(oijalgorithms.kmp_search("haystackneedlehayhayhayneedlehay","needle"))
```
Which helps when you need to find keywords in a text efficiently, returning:
```python
[8,23]
```
There is also a built-in tester if you decide to learn and write these algorithms yourself (which is recommended):
```python
def your_prefix_sums(values):
    result = []
    current_sum = 0

    for value in values:
        current_sum += value
        result.append(current_sum)

    return result

print(oijalgorithms.test_your_function(your_prefix_sums, 
        oijalgorithms.prefix_sum, 1000))
```
Your code has just been tested on a 1000 random lists! see below for results:
```python
{'ok': True, 'tests': 1000, 'function': 'prefix_sum'}
```

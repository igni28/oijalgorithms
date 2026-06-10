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

On Windows, you can use:

```bash
py -m pip install -e .
```

### Check if it works

Run Python and import the package:

```python
import oijalgorithms

print(oijalgorithms.gcd(20, 15))
```

Expected output:

```text
5
```

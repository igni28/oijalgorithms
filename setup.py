import setuptools
import numpy as np

numpy_include_path = np.get_include()

setuptools.setup(
    name="oijalgorithms",
    packages=setuptools.find_packages(),
    include_dirs=[numpy_include_path],  # "-I"
    #extra_compile_args=["-O3", "-DNDEBUG=1"],
    #define_macros=[
       # ("NDEBUG",1),
    #],
    ext_modules=[
        setuptools.Extension("oijalgorithms.cfunkcje", sources=["src/cfunkcje.c"]),
    ]
)

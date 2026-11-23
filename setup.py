from setuptools import Extension, setup

# Create the Python module from the relevant C files
module = Extension("symnmfmodule",sources=["symnmfmodule.c","symnmf_operations.c","support_functions.c"])

# Setup for building the C extension package
setup(name='symnmfmodule',
      version='1.0',
      description='Python wrapper for custom C extension of Symnmf',
      ext_modules=[module])
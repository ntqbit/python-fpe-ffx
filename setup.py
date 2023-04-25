from setuptools import setup, find_packages

setup(
    name='fpe ffx',
    version='1.0',
    packages=find_packages(include=['fpe_ffx', 'fpe_ffx.*'])
)
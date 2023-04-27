from setuptools import setup, Extension,  find_packages

setup(
    name='fpe ffx',
    version='1.0',
    packages=find_packages(include=['fpe_ffx', 'fpe_ffx.*']),
    ext_modules=[
        Extension(
            "_fpe_ffx",
        include_dirs=['lib/'],
            sources=[
                "src/fpe_ffx.c",
                "src/ffx.c",
                "src/round_functions/aes_round_function.c",
                "src/round_functions/xtea_round_function.c",
                "src/lib/aes/aes.c",
                "src/lib/xtea/xtea.c"
            ]
        )
    ]
)

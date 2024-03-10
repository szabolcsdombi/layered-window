from setuptools import Extension, setup
from wheel.bdist_wheel import bdist_wheel

stubs = {
    'packages': ['layered_window-stubs'],
    'package_data': {'layered_window-stubs': ['__init__.pyi']},
    'include_package_data': True,
}

class bdist_wheel_abi3(bdist_wheel):
    def get_tag(self):
        python, abi, plat = super().get_tag()

        if python.startswith('cp'):
            return 'cp311', 'abi3', plat

        return python, abi, plat

ext = Extension(
    name='layered_window',
    sources=['layered_window.cpp'],
    libraries=['user32', 'gdi32', 'msimg32', 'dwmapi'],
    define_macros=[('Py_LIMITED_API', 0x030B0000)],
    py_limited_api=True,
)

with open('README.md') as readme:
    long_description = readme.read()

setup(
    name='layered-window',
    version='0.9.0',
    ext_modules=[ext],
    license='MIT',
    platforms=['windows'],
    python_requires='>=3.11',
    description='Layered Window with True Transparency for Windows',
    long_description=long_description,
    long_description_content_type='text/markdown',
    author='Szabolcs Dombi',
    author_email='szabolcs@szabolcsdombi.com',
    url='https://github.com/szabolcsdombi/layered-window/',
    project_urls={
        'Source': 'https://github.com/szabolcsdombi/layered-window/',
    },
    cmdclass={'bdist_wheel': bdist_wheel_abi3},
    **stubs,
)

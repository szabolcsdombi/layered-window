from setuptools import Extension, setup

ext = Extension(
    name='layered_window',
    sources=['layered_window.cpp'],
    libraries=['user32', 'gdi32', 'msimg32', 'dwmapi'],
)

setup(
    name='layered-window',
    version='0.9.0',
    ext_modules=[ext],
)

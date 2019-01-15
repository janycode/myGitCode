# -*- coding: utf-8 -*-
#!/usr/bin/env python3

from distutils.core import setup, Extension

MOD = 'Extest'
setup(name=MOD, ext_modules=[Extension(MOD, sources=['Extest2.c'])])

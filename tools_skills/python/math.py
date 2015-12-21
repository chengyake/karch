#!/usr/bin/env python
# -*- coding: utf-8 -*-


a=100
a+=1


kw={'a':a,}
def test():
    kw['a']+=10
    print kw


test()
test()




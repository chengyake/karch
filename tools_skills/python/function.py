#!/usr/bin/env python
# -*- coding: utf-8 -*-


print "\n\n-------------function--------------" #don't repeat
def my_abs(x):
    if x >= 0:
        pass
        #return x
    else:
        return -x
print my_abs(-9)




#tuple list set...
def calc(*numbers):
    sum = 0
    for n in numbers:
        sum = sum + n * n
    return sum

s = set([1, 2, 3])
print calc(10,9)
print calc(*s)




#dict
def person(name="yake", age=20, **kw):
    print 'name:', name, 'age:', age, 'other:', kw

d = {'1': 95, '2': 75, '3': 85}
person("yake", 27, city="beijing")
person("yake", 27, **d)
person(None, 28)


#参数定义的顺序必须是：必选参数、默认参数、可变参数和关键字参数。



#高阶函数
f=person
f("cyk", 18)
f=calc

#map
print map(f, [1, 2, 3, 4, 5, 6, 7, 8, 9])


#reduce
def fn(x, y):
    return x * 10 + y
print reduce(fn, [2, 3, 5, 7, 9])


#filter
def is_odd(n):
    return n % 2 == 1

print filter(is_odd, [1, 2, 4, 5, 6, 9, 10, 15])


#sorted
def reversed_cmp(x, y):
    if x > y:
        return -1
    if x < y:
        return 1
    return 0
print sorted([36, 5, 12, 9, 21], reversed_cmp)



#返回函数




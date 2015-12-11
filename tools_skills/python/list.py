#!/usr/bin/env python
# -*- coding: utf-8 -*-

print "------------list---------------" #array

num=[1, 2, 3]
print "this list is:\t\t\t",num,"length:\t", len(num)

num[2]=2
print "after change,this list is:\t",num,"length:\t", len(num)

#print num[-1]
num.append("yake")
print "after change,this list is:\t",num,"length:\t", len(num)

#num[a:b:c] between a and b per c
print "after change,this list is:\t",num[:3],"length:\t", len(num)  #切片

num.pop(-1)
print "after change,this list is:\t",num,"length:\t", len(num)

num_n = [x * x for x in range(1, 11)] #列表生成式
print "after change,this list is:\t",num_n,"length:\t", len(num_n)





x, y = ("tu", "ple")
#print "%s%s" % (x, y)
string=x+y
print "\n\n-------------%s--------------" % string    #fixed list

num=(1, 2, 2)
print "this list is:\t\t\t",num,"length:\t", len(num)

g = (x * x for x in range(10))  #生成器
print "this list is:\t\t\t",g,"length:\t"





print "\n\n-------------dict--------------" #key-value

d = {'1': 95, '2': 75, '3': 85}
print "this list is:\t\t\t",d,"length:\t", len(d)






print "\n\n-------------set--------------" #don't repeat
s = set([1, 2, 3])
print "this list is:\t\t\t",s,"length:\t", len(s)



#迭代 bianli
for x, y in [(1, 1), (2, 4), (3, 9)]:
    print x, y





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
print calc(10,9)
print calc(*s)


#dict
def person(name="yake", age=20, **kw):
    print 'name:', name, 'age:', age, 'other:', kw

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




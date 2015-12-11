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




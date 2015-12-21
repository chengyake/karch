#!/usr/bin/env python
# -*- coding: utf-8 -*-




class Animal(object):
    def run(self):
        print 'Animal is running...'



a=Animal()
a.run()


class Dog(Animal):
    def run(self):
        print 'Dog is running...'
    def eat(self):
        print 'Eating meat...'


class Cat(Animal):
    def run(self):
        print 'Cat is running...'
    def eat(self):
        print 'Eating meat...'


def run_twice(animal):
    animal.run()
    animal.run()


run_twice(Dog())
run_twice(Cat())




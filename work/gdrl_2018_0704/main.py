#! /usr/bin/python3
import copy
import argparse
import random
import numpy as np
from gdrl import DL
from showProcess import ShowProcess

#individual
num_all  = 40
num_best = 5
num_drop = num_best*2

num_exchange = 25   #list sample
num_mutation = 5

probility_exchange = 0.01 #inter cell, genes sample
probility_mutation = 0.001

class genetic_algo:
    
    def __init__(self):
        self.gdrl = DL()
        self.genes_score = {}
        self.genes = np.random.randint(2,size=(num_all, self.gdrl.layer_num, self.gdrl.layer_nodes, self.gdrl.layer_nodes))
        return

    def calc_score(self):
        #pb = ShowProcess(num_all, "ok")
        for i in range(num_all):
            #pb.show_process(i)
            self.genes_score[i] = self.gdrl.train(self.genes[i])
        return

    def sort_drop_and_save(self):
        self.idx_sorted = sorted(self.genes_score.items(), key=lambda d:d[1])#[(idx2,v2), (idx1,v1)]
        self.idx_best = self.idx_sorted[-num_best:]
        self.idx_drop = self.idx_sorted[:num_drop]
        for i in range(num_best):
            self.genes[self.idx_drop[i][0]] = self.genes[self.idx_best[i][0]]
            self.genes[self.idx_drop[int(num_drop/2)+i][0]] = self.genes[self.idx_best[i][0]]

        np.save("best.npy", self.genes[self.idx_sorted[0][0]])

        print("yakelog: score:",self.idx_sorted)
        print("yakelog: score:",self.idx_sorted[0][1], self.idx_sorted[-1][1])
        return
    
    def _exchange_genes(self, idx1, idx2):
        is_sw = np.random.randint(1,int(1/probility_exchange)+1, size=(self.gdrl.layer_num, self.gdrl.layer_nodes, 1))
        is_sw[is_sw>1]=0
        #is_sw = expand_dims(is_sw, axis=-1)
        tmp = copy.deepcopy(self.genes[idx1])
        self.genes[idx1]=np.where(is_sw>0, self.genes[idx2], self.genes[idx1])
        self.genes[idx2]=np.where(is_sw>0, tmp, self.genes[idx2])
        return


    def exchange_genes(self):
        ex_list = random.sample(self.idx_sorted[num_drop:-num_best], num_exchange)
        ex_list.extend(self.idx_drop[:int(num_drop/2)])
        random.shuffle(ex_list)
        for i in range(int(len(ex_list)/2)):
            self._exchange_genes(ex_list[i*2][0], ex_list[i*2+1][0])
        return

    def _genes_mutation(self, idx):
        is_mu = np.random.randint(1,int(1/probility_mutation)+1, size=(self.gdrl.layer_num, self.gdrl.layer_nodes, self.gdrl.layer_nodes))
        is_mu[is_mu>1]=0
        self.genes[idx]+=is_mu
        self.genes[self.genes>1]=0
        return


    def genes_mutation(self):
        mu_list = random.sample(self.idx_sorted[int(num_drop/2):-num_best], num_mutation)
        mu_list.extend(self.idx_drop[int(num_drop/2):])
        for i in range(len(mu_list)):
            self._genes_mutation(mu_list[i][0])
        return



times_loop = 10

if __name__ == '__main__':

    ga = genetic_algo()
    for i in range(times_loop):
        ga.calc_score()
        ga.sort_drop_and_save()
        ga.exchange_genes()
        ga.genes_mutation()





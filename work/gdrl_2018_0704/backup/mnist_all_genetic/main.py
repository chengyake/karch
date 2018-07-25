#! /usr/bin/python3
import os
import sys
import copy
import argparse
import random
import numpy as np
from gdrl import DL


#score must be stable and reappearance
#keep fixed number of connections will make score stable at begin



num_mul=1

#individual
num_all  = 100*num_mul
num_best = 5*num_mul
num_drop = num_best*2

num_exchange = 15*num_mul   #list sample
num_mutation = 30*num_mul


class genetic_algo:
    
    genes=[]
    genes_score = {}
    probility_exchange1 = 0.001 #inter cell, genes sample
    probility_exchange2 = 0.1
    probility_mutation1 = 0.001
    probility_mutation2 = 0.1

    def __init__(self):
        self.gdrl = DL()
        
        if self.restore_genes()==False:
            self.genes = np.random.random((num_all, self.gdrl.layer_num, self.gdrl.layer_nodes, self.gdrl.layer_nodes))
            self.genes = self.genes.astype(np.float32)
        return

    def restore_genes(self):
        if os.path.exists("genes.npz"):
            self.genes = np.load("genes.npz")["genes"]
            return True
        return False

    def save_genes(self):
        np.savez("genes.npz", genes = self.genes)
        return
    
    def get_best_genes(self):
        return np.load("best.npz")["best"]


    def calc_score(self):
        for i in range(num_all):
            self.genes_score[i] = self.gdrl.train(self.genes[i])
            sys.stdout.write("yakelog %d\t:%f\n" %(i, self.genes_score[i]))
            sys.stdout.flush()
        return

    def sort_drop_adjust_and_save(self):
        self.idx_sorted = sorted(self.genes_score.items(), key=lambda d:d[1])#[(idx2,v2), (idx1,v1)]
        self.idx_best = self.idx_sorted[-num_best:]
        self.idx_drop = self.idx_sorted[:num_drop]
        for i in range(num_best):
            self.genes[self.idx_drop[i][0]] = self.genes[self.idx_best[i][0]]
            self.genes[self.idx_drop[int(num_drop/2)+i][0]] = self.genes[self.idx_best[i][0]]

        np.savez("best.npz", best = self.genes[self.idx_sorted[-1][0]])
        sys.stdout.write("yakelog: score: %f, %f, %f\n" % (self.idx_sorted[0][1],self.idx_sorted[int(num_all/2)][1],self.idx_sorted[-1][1]))
        sys.stdout.flush()
        return
    
    def _exchange_genes(self, idx1, idx2, prob):
        is_sw = np.random.randint(1,int(1/prob)+1, size=(self.gdrl.layer_num, 1, self.gdrl.layer_nodes))
        tmp = copy.deepcopy(self.genes[idx1])
        self.genes[idx1] = np.where(is_sw==1, self.genes[idx2], self.genes[idx1])
        self.genes[idx2] = np.where(is_sw==1, tmp, self.genes[idx2])
        return


    def exchange_genes(self):
        ex_list = random.sample(self.idx_sorted[num_drop:-num_best], num_exchange)
        ex_list.extend(self.idx_drop[:int(num_drop/2)])
        random.shuffle(ex_list)
        for i in range(int(len(ex_list)/2)):
            if ex_list[i*2][1] > ex_list[i*2+1][1]:
                s = ex_list[i*2][1]
            else:
                s = ex_list[i*2+1][1]
            prob = self.probility_exchange2-(self.probility_exchange2-self.probility_exchange1)*(self.idx_sorted[-1][1]-s)/(self.idx_sorted[-1][1] - self.idx_sorted[0][1])
            self._exchange_genes(ex_list[i*2][0], ex_list[i*2+1][0], prob)
        return

    def _genes_mutation(self, idx, prob=0.1):
        is_mu = np.random.randint(1,int(1/prob)+1, size=(self.gdrl.layer_num, self.gdrl.layer_nodes, self.gdrl.layer_nodes))
        mu_data = np.random.random((self.gdrl.layer_num, self.gdrl.layer_nodes, self.gdrl.layer_nodes))
        self.genes[idx] = np.where(is_mu==1, mu_data, self.genes[idx])
        return


    def genes_mutation(self):
        mu_list = random.sample(self.idx_sorted[int(num_drop/2):-num_best], num_mutation)
        mu_list.extend(self.idx_drop[int(num_drop/2):])
        for i in range(len(mu_list)):
            prob = self.probility_mutation2-(self.probility_mutation2-self.probility_mutation1)*(self.idx_sorted[-1][1]-mu_list[i][1])/(self.idx_sorted[-1][1] - self.idx_sorted[0][1])
            self._genes_mutation(mu_list[i][0], prob)
        return



times_loop = 1000000

if __name__ == '__main__':

    ga = genetic_algo()
    for i in range(times_loop):
        ga.calc_score()
        ga.sort_drop_adjust_and_save()
        ga.exchange_genes()
        ga.genes_mutation()
        ga.save_genes()



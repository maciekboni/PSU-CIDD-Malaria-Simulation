# -*- coding: utf-8 -*-
"""
Created on Fri Oct  1 11:11:23 2021

@author: nguyentd
"""


import yaml;
import pandas as pd; 

stream = open('input_DxG.yml', 'r');
data = yaml.load(stream);
stream.close();

genotype_db= pd.read_csv('genotypeID.txt', sep='\t')

#%%
g_db={}
#for locus_i, locus_v in enumerate(data['genotype_info']['loci']):
#    for allele_i, allele_v in enumerate(data['genotype_info']['loci'][locus_i]['alleles']):
#        print(locus_i, allele_v['value'])
g_id = 0
for i0 in range(0,2):
    for i1 in range(0,8):
        for i2 in range(0,2):
            for i3 in range(0,2):
                for i4 in range(0,2):
                    gene = "%d%d%d%d%d"%(i0,i1,i2,i3,i4)
                    g_db[g_id]= gene
                    g_id += 1

#%%
def match_genotype(g, k):
    print(g, k)
    for i, x in enumerate(g):
        if k[i] == '.':
            continue
        if x != k[i]:
            return False 
    return True
        



def get_ec50(g_id, ec_50_map):
    for ec_k, ec_v in ec_50_map.items():
        if match_genotype(g_db[g_id], ec_k ):
            return ec_v
        
    return -1

#%%
    
import copy;

ec50_table =  copy.deepcopy(genotype_db)
for dr_k, dr_map in data['drug_db'].items():
    if dr_k > 6:
        continue
    df = pd.DataFrame(
        [get_ec50(g_id, dr_map['EC50']) ] for g_id in genotype_db['ID']
    )
    ec50_table[dr_map['name']]= df[0]

#%%

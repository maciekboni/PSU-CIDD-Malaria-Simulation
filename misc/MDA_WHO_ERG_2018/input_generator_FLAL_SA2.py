# -*- coding: utf-8 -*-
"""
Created on Tue Aug  7 14:23:33 2018

@author: NguyenTran
"""

import yaml;
import numpy as np;
from math import log;
import copy;
import pandas as pd

number_MDA_round = [0,1,2,3,4];
#%%
#    
#from pyDOE import lhs
#
#

#
#for mda_round in number_MDA_round:
#
#    # number of samples per mda scenarios
#    sample_sizes = 50000;
#    paramspace = lhs(5, samples= sample_sizes)
#    
#    popsizes = [26850, 201368];
#    betas = [0.0538, 0.07];
#    importations = [0, 0.2];
#    itcs = [0.6, 0.8];
#    cr_factors  = [0.7, 1.3];
#    
#    paramspace[:, 0] *= (popsizes[1] - popsizes[0])
#    paramspace[:, 0] += popsizes[0]
#    paramspace[:, 0] = np.round(paramspace[:, 0])
#    
#    paramspace[:, 1] *= (betas[1] - betas[0])
#    paramspace[:, 1] += betas[0]
#    
#    paramspace[:, 2] *= (importations[1] - importations[0])
#    paramspace[:, 2] += importations[0]
#    
#    paramspace[:, 3] *= (itcs[1] - itcs[0])
#    paramspace[:, 3] += itcs[0]
#    
#    paramspace[:, 4] *= (cr_factors[1] - cr_factors[0])
#    paramspace[:, 4] += cr_factors[0]
#    
#    df = pd.DataFrame(data=paramspace, columns=["popsize", "beta","importation","itc","cr_factor"]);
#    
#    df.to_csv("SA2_paramspace_%d.csv"%mda_round,index=False);
#%%

stream = open('input_FLAL_v2.yml', 'r');
data = yaml.load(stream);
stream.close();

data['starting_date'] = '2008/1/1';
data['ending_date'] = '2042/1/1';
data['start_of_comparison_period']= '2022/1/1';

data['seasonal_info']['enable'] = 'false';

#1 location
location_info =  [[0, 0, 0]];
number_of_locations = len(location_info);
data['location_db']['location_info']= location_info;   

#
sd_prob_individual_present_at_mda = [0.3, 0.3, 0.3]
data['sd_prob_individual_present_at_mda'] = sd_prob_individual_present_at_mda



#%%
def cal_new_cr( old_cr, cr_factor):
    base_annual_cr = 1 - (1- old_cr)**365;
    if base_annual_cr*cr_factor > 1:
        new_cr = 1- (1- 0.99) ** (1/365)
    else:
        new_cr = 1- (1- base_annual_cr*cr_factor) ** (1/365)
#    print(old_cr,cr_factor,new_cr)
    return new_cr


for mda_round in number_MDA_round:
    input_id=0;
    df = pd.read_csv("SA2_paramspace_%d.csv"%mda_round);
    for r_index, row in df.iterrows():
        new_data = copy.deepcopy(data)
        #population size 
        data['location_db']['population_size_by_location'] = [round(row.popsize.item())];
        
        new_data['location_db']['beta_by_location'] = np.full(number_of_locations, row.beta.item()).tolist()
        
        for index,event in enumerate(data['events']):
            if event['name'] == 'single_round_MDA':
                new_data['events'][index]['info'] = data['events'][index]['info'][0:mda_round]                    
        
            if event['name'] == 'introduce_parasites_periodically':
                new_data['events'][index]['info'][0]['parasite_info'][0]['duration'] = 10000
                new_data['events'][index]['info'][0]['parasite_info'][0]['number_of_cases'] = round(row.importation.item()*10000)
            
            if event['name'] == 'change_treatment_coverage':
                if row.itc == 0:                                    
                    new_data['events'][index]['info']= []                        
                else:
                    new_data['events'][index]['info'][0]['p_treatment_for_less_than_5_by_location_to'] = np.full(number_of_locations, row.itc.item()).tolist()
                    new_data['events'][index]['info'][0]['p_treatment_for_more_than_5_by_location_to'] = np.full(number_of_locations, row.itc.item()).tolist()
                    
        
        for locus_index,locus in enumerate(new_data['genotype_info']['loci']):
            for allele_index, allele in enumerate(locus['alleles']):
                new_data['genotype_info']['loci'][locus_index]['alleles'][allele_index]['daily_cost_of_resistance'] = cal_new_cr(data['genotype_info']['loci'][locus_index]['alleles'][allele_index]['daily_cost_of_resistance'], row.cr_factor.item())
        
        output_filename = 'FLAL_SA2/%d/%d.yml'%(mda_round, input_id);
        output_stream = open(output_filename, 'w');
        yaml.dump(new_data, output_stream); 
        output_stream.close();
        input_id += 1


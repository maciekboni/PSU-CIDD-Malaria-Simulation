# -*- coding: utf-8 -*-
"""
Created on Tue Aug  7 14:23:33 2018

@author: NguyenTran
"""

import yaml;
import numpy as np;
from math import log;
import copy;

#import inflect;
#p = inflect.engine();

def kFormatter(num):
    return str(num) if num <=999 else str(round(num/1000)) +'k';


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

#3RMDA
number_MDA_round = [0,1,2,3,4];

#
sd_prob_individual_present_at_mda = [0.3, 0.3, 0.3]
data['sd_prob_individual_present_at_mda'] = sd_prob_individual_present_at_mda

#for index,event in enumerate(data['events']):
#    if event['name'] == 'single_round_MDA':
#        data['events'][index]['info'] = data['events'][index]['info'][0:number_MDA_round]

popsizes = {
    26850: '40K',
#    53700: '80K',
    67125: '100K',
#    134250: '200K',
    201368: '300K',
        }

pfprs = {
#        0.1849: 'PFPR15',        
#        0.077: 'PFPR5',
        0.06413: 'PFPR3',
        0.0585: 'PFPR2',
        0.0538: 'PFPR1'
        }

itcs = {
        0.8: '0p8', 
#        0.75: '0p75' , 
        0.7: '0p7',
#        0.65: '0p65',
        0: '0p0'
        }

importations = {
#        0: '0p0', 
        0.001: '0p001' ,
        0.01: '0p01',
        0.1: '0p1',
        0.2: '0p2'
        }

cr_factors= {
#    0.7: '0p7',
    0.8: '0p8',
    0.9: '0p9',
    1.0: '1p0',
    1.1: '1p1',
    1.2: '1p2',
#    1.3: '1p3'
    }

# base_annual_cr = 1 - (1- 0.0005)**365;

# new_cr = 1- (1- base_annual_cr*np.array([0.7,0.8,0.9,1.0,1.1,1.2,1.3])) ** (1/365)

def cal_new_cr( old_cr, cr_factor):
    base_annual_cr = 1 - (1- old_cr)**365;
    if base_annual_cr*cr_factor > 1:
        new_cr = 1- (1- 0.99) ** (1/365)
    else:
        new_cr = 1- (1- base_annual_cr*cr_factor) ** (1/365)
#    print(old_cr,cr_factor,new_cr)
    return new_cr


for mda_round in number_MDA_round:
    for popsize, popsize_str in popsizes.items():
        for beta,pfpr_str in pfprs.items():
            for importation, importation_str in importations.items():
                for itc, itc_str in itcs.items():
                    for cr_factor, cr_str in cr_factors.items():
                        new_data = copy.deepcopy(data)
                        #population size 
                        data['location_db']['population_size_by_location'] = [popsize];
                        
                        new_data['location_db']['beta_by_location'] = np.full(number_of_locations, beta).tolist()
                        
                        for index,event in enumerate(data['events']):
                            if event['name'] == 'single_round_MDA':
                                new_data['events'][index]['info'] = data['events'][index]['info'][0:mda_round]                    
                        
                            if event['name'] == 'introduce_parasites_periodically':
                                new_data['events'][index]['info'][0]['parasite_info'][0]['duration'] = 1000    
                                new_data['events'][index]['info'][0]['parasite_info'][0]['number_of_cases'] = round(importation*1000)
                            
                            if event['name'] == 'change_treatment_coverage':
                                if itc == 0:                                    
                                    new_data['events'][index]['info']= []                        
                                else:
                                    new_data['events'][index]['info'][0]['p_treatment_for_less_than_5_by_location_to'] = np.full(number_of_locations, itc).tolist()
                                    new_data['events'][index]['info'][0]['p_treatment_for_more_than_5_by_location_to'] = np.full(number_of_locations, itc).tolist()
                                    
                        
                        for locus_index,locus in enumerate(new_data['genotype_info']['loci']):
                            for allele_index, allele in enumerate(locus['alleles']):
                                new_data['genotype_info']['loci'][locus_index]['alleles'][allele_index]['daily_cost_of_resistance'] = cal_new_cr(data['genotype_info']['loci'][locus_index]['alleles'][allele_index]['daily_cost_of_resistance'], cr_factor)
                        
                        output_filename = 'FLAL_SA1/%s/ONELOC_%s_%dRMDA_%s_OPPUNIFORM_FLAL_imp_%s_itc_%s_crfactor_%s.yml'%(popsize_str, popsize_str, mda_round,pfpr_str,importation_str,itc_str, cr_str);
                        output_stream = open(output_filename, 'w');
                        yaml.dump(new_data, output_stream); 
                        output_stream.close();

#for index,beta in enumerate(betas):
#    data['location_db']['beta_by_location'] = np.full(number_of_locations, beta).tolist()
#    output_filename = 'beta/input_beta_%d.yml'%index;
#    output_stream = open(output_filename, 'w');
#    yaml.dump(data, output_stream);
#    output_stream.close();

#
#
#print(kFormatter(9000));
#print(p.number_to_words( number_of_locations, threshold=10));

#output_filename = 'ONELOC_300K_3RMDA_PFPR15_OPPUNIFORM_FLAL.yml';
#
#output_filename = 'input_test.yml';
#
#output_stream = open(output_filename, 'w');
#yaml.dump(data, output_stream);
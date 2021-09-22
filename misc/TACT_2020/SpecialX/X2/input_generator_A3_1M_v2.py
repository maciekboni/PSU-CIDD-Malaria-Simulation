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


stream = open('input_base_A3_mu_0p001983_v2.yml', 'r');
data = yaml.load(stream);
stream.close();

 # COMMENT IT LATER
# data['ending_date'] = '2022/1/1';

location_info =  [[0, 0, 0]];
number_of_locations = len(location_info);
data['location_db']['location_info']= location_info;

#population size 
popsize = 1000000
data['location_db']['population_size_by_location'] = [popsize];       

tcs = {
       0.25: {
           'tc_str': '0p25',
           'pfprs': {
               0.13: 'PFPR10p0',
               0.049: 'PFPR01p0',
               0.044: 'PFPR00p1'
               }           
           },
       0.5: {
           'tc_str': '0p50',
           'pfprs': {
               0.15: 'PFPR10p0',
               0.058: 'PFPR01p0',
               0.051: 'PFPR00p1'
               }           
           },
       0.75: {
           'tc_str': '0p75',
           'pfprs': {
               0.17: 'PFPR10p0',
               0.069: 'PFPR01p0',
               0.06: 'PFPR00p1'
               }           
           },
       }

tact = {
#        2: 'AL',
        4: 'ALAQ',
#        5: 'AMP'
        }

AL_SpecialX_Efficacies = {
        90: { 'diff' : 0.05 },
        80: { 'diff' : 0.21 },
        50: { 'diff' : 0.63 },
        }


for tc, tc_details in tcs.items():
    tc_str = tc_details['tc_str'];
    pfprs = tc_details['pfprs'];
    for beta,pfpr in pfprs.items():
        for tact_id, tact_str in tact.items():
            for AL_EF, diff_map in AL_SpecialX_Efficacies.items():
                print(beta, tc_str, tact_id)
                new_data = copy.deepcopy(data)
                new_data['location_db']['beta_by_location'] = np.full(number_of_locations, beta).tolist()
                
                new_data['location_db']['p_treatment_for_less_than_5_by_location'] = np.full(number_of_locations, tc).tolist()
                new_data['location_db']['p_treatment_for_more_than_5_by_location'] = np.full(number_of_locations, tc).tolist()
                
                for index,event in enumerate(data['events']):                    
                    if event['name'] == 'modify_nested_mft_strategy':
                        new_data['events'][index]['info'][0]['strategy_id'] = tact_id
                
                for ec50_key, ec50_value in new_data['drug_db'][1]['EC50'].items():
                    if ec50_key[1] in '89ABCDEF':
                        print(ec50_key)
                        new_data['drug_db'][1]['EC50'][ec50_key] = ec50_value+ diff_map['diff']
                            
                output_filename = 'A3/TACT_%s_TC_%s_TACT_%s_AL_EF_%d.yml'%(pfpr,tc_str, tact_str, AL_EF);
                output_stream = open(output_filename, 'w');
                yaml.dump(new_data, output_stream); 
                output_stream.close();

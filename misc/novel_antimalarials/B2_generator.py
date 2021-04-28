# -*- coding: utf-8 -*-
"""
Created on Wed Mar 31 00:02:30 2021

@author: nguyentd
"""

import yaml;
import numpy as np;
from math import log;
import copy;

#import inflect;
#p = inflect.engine();

def kFormatter(num):
    return str(num) if num <=999 else str(round(num/1000)) +'k';


stream = open('input_novel_antimalarials_1d.yml', 'r');
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
       # 0.25: {
       #     'tc_str': '0p25',
       #     'pfprs': {
       #         0.13: 'PFPR10p0',
       #         0.049: 'PFPR01p0',
       #         0.044: 'PFPR00p1'
       #         }           
       #     },
       # 0.5: {
       #     'tc_str': '0p50',
       #     'pfprs': {
       #         # 0.15: 'PFPR10p0',
       #         0.057: 'PFPR01p0',
       #         # 0.051: 'PFPR00p1'
       #         }           
       #     },
        0.75: {
            'tc_str': '0p75',
            'pfprs': {
                # 0.16: 'PFPR10p0',
                0.066: 'PFPR01p0',
                # 0.0585: 'PFPR00p1'
                }           
            },
       }

fl = {
        # 0: 'DP',
        2: 'AL'
        }

tf_switch ={
    0.1 : '0p10',
    0.15 : '0p15',
    0.2 : '0p20',
    }

replacement_fractions = {
    0.0 : '0p0',
    0.1 : '0p1',
    0.2 : '0p2',
    0.4 : '0p4',
    0.6 : '0p6',
    0.8 : '0p8',
    1.0 : '1p0',
    }

KAF_EC50s = {
    0.75: '0p75',
    0.90: '0p90',
    1.25: '1p25',
    }

for tc, tc_details in tcs.items():
    tc_str = tc_details['tc_str'];
    pfprs = tc_details['pfprs'];
    for beta,pfpr in pfprs.items():
        for fl_id, fl_str in fl.items():
            for tf, tf_str in tf_switch.items():
                for rf, rf_str in replacement_fractions.items():
                    for ec50, ec50_str in KAF_EC50s.items():
                        print(beta, tc_str, fl_str, tf_str, rf_str)
                        new_data = copy.deepcopy(data)
                        new_data['location_db']['beta_by_location'] = np.full(number_of_locations, beta).tolist()
                        
                        new_data['location_db']['p_treatment_for_less_than_5_by_location'] = np.full(number_of_locations, tc).tolist()
                        new_data['location_db']['p_treatment_for_more_than_5_by_location'] = np.full(number_of_locations, tc).tolist()
                        
                        new_data['strategy_db'][5]['strategy_ids'][0] = fl_id
                        new_data['strategy_db'][5]['tf_threshold'] = tf
                        new_data['strategy_db'][5]['replacement_fraction'] = rf
                      
                        new_data['drug_db'][7]['EC50']['....1'] = ec50
                        
                        output_filename = 'B2/NovelDrug_PFPR_%s_TC_%s_FL_%s_TF_%s_Q_%s_EC50_%s.yml'%( pfpr,tc_str, fl_str, tf_str, rf_str, ec50_str);
                        
                        output_stream = open(output_filename, 'w');
                        yaml.dump(new_data, output_stream); 
                        output_stream.close();

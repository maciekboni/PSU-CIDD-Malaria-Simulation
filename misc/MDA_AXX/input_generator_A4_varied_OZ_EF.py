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


stream = open('input_FLAL_v2_mu_0p001983.yml', 'r');
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

#201368

#population size 
popsize = 26850
data['location_db']['population_size_by_location'] = [popsize];       

#3RMDA
number_MDA_round = [
#        0,1,2,3,
        4
        ];

#
sd_prob_individual_present_at_mda = [0.3, 0.3, 0.3]
data['sd_prob_individual_present_at_mda'] = sd_prob_individual_present_at_mda

#for index,event in enumerate(data['events']):
#    if event['name'] == 'single_round_MDA':
#        data['events'][index]['info'] = data['events'][index]['info'][0:number_MDA_round]
pfprs = {
#        0.1849: 'PFPR15',        
#        0.077: 'PFPR5',
        0.06413: 'PFPR3',
        0.0585: 'PFPR2',
        0.0538: 'PFPR1'}

improved_tc = {
#        True: '_itc' , 
        False: ''
        }

importations = {
        True: '_imp' , 
#        False: ''
        }

#### Main (A) Drug AL
#### B Drug: AL / (OZ+FQ / KAF-LUM)
mda_therapy_ids = {
        15: [5, 'OZFQ'],
        }

mda_stategy= ['AAB', 'ABA', 'ABB']


oz_efficacies = {
       40: {
               'ec50' : 1.37,
           },
       50: {
               'ec50' : 1.29,
           },
       60: {
               'ec50' : 1.21,
           },
       70: {
               'ec50' : 1.12,
           },
       80: {
               'ec50' : 1.02,
           },
        }


for mda_round in number_MDA_round:
    for beta,pfpr_str in pfprs.items():
        for _,itc in improved_tc.items():
            for _,imp in importations.items():
                for mda_therapy_id,mda_therapy in mda_therapy_ids.items():   
                    for oz_eff, ec50_dict in oz_efficacies.items():   
                        for strategy in mda_stategy:
                            
                            new_data = copy.deepcopy(data)
                            new_data['location_db']['beta_by_location'] = np.full(number_of_locations, beta).tolist()
                            
                            for index,event in enumerate(data['events']):
                                if event['name'] == 'single_round_MDA':
                                    new_data['events'][index]['info'] = data['events'][index]['info'][0:mda_round]                    
                            
                            if itc == '':
                                for index,event in enumerate(data['events']):
                                    if event['name'] == 'change_treatment_coverage':
                                        new_data['events'][index]['info']= []
                            if imp == '':
                                for index,event in enumerate(data['events']):
                                    if event['name'] == 'introduce_parasites_periodically':
                                        new_data['events'][index]['info']= []     
                            
                            for index,event in enumerate(data['events']):                    
                                if event['name'] == 'modify_nested_mft_strategy':
                                    if strategy == 'AAB':
                                        new_data['mda_therapy_id'] = 7
                                        new_data['events'][index]['info'][0]['strategy_id'] = mda_therapy[0]
                                    else:
                                        if strategy == 'ABA':
                                            new_data['mda_therapy_id'] = mda_therapy_id
                                            new_data['events'][index]['info'][0]['strategy_id'] = 1
                                        else:
                                            new_data['mda_therapy_id'] = mda_therapy_id
                                            new_data['events'][index]['info'][0]['strategy_id'] = mda_therapy[0]
                            
                            new_data['drug_db'][8]['EC50']['....1'] = ec50_dict['ec50']
                            new_data['drug_db'][9]['EC50']['...1.'] = ec50_dict['ec50']
                            
                            output_filename = 'A4_varied_OZ_EF/ONELOC_40k_%dRMDA_%s_OPPUNIFORM_FLAL_%s_%s%s%s_OZFQ_EF_%d.yml'%(
                                    mda_round,pfpr_str,strategy, mda_therapy[1], itc,imp,oz_eff);
                            output_stream = open(output_filename, 'w');
                            yaml.dump(new_data, output_stream); 
                            output_stream.close();
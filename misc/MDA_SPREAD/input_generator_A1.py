# -*- coding: utf-8 -*-
"""
Created on Tue Aug  7 14:23:33 2018

@author: NguyenTran
"""

import yaml;
import numpy as np;
from math import log;
import copy;
import datetime;

#import inflect;
#p = inflect.engine();

def kFormatter(num):
    return str(num) if num <=999 else str(round(num/1000)) +'k';


stream = open('input_A1.yml', 'r');
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
number_MDA_round = [0,1,2,3,4];

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

importations = {True: '_imp' , 
               False: ''}

weeks_between_rounds = [5, 8, 12] 
mda_start_date = "2022/1/1"

for w in weeks_between_rounds:
    for beta,pfpr_str in pfprs.items():
        for _,itc in improved_tc.items():
            for _,imp in importations.items():
                new_data = copy.deepcopy(data)
                new_data['location_db']['beta_by_location'] = np.full(number_of_locations, beta).tolist()
                
                for index,event in enumerate(data['events']):
                    if event['name'] == 'single_round_MDA':
                        
                        for i,r in enumerate(data['events'][index]['info']):
                            delta = datetime.timedelta(days=i*w*7)
                            mda_date = datetime.datetime.strptime(mda_start_date, "%Y/%m/%d").date() + delta
#                            print(mda_date.strftime("%Y/%m/%d"))
                            new_data['events'][index]['info'][i]['day'] = mda_date.strftime("%Y/%m/%d")                    
                    if event['name'] == 'change_treatment_coverage':
                        if itc == '':
                            new_data['events'][index]['info']= []
                
                    if event['name'] == 'introduce_parasites_periodically_v2':
                        if imp == '':
                            new_data['events'][index]['info']= []     
                
                output_filename = 'A1/ONELOC_40k_%s_SEP_%d_WEEKS_OPPUNIFORM_FLAL%s%s.yml'%(pfpr_str, w, itc,imp);
                output_stream = open(output_filename, 'w');
                yaml.dump(new_data, output_stream); 
                output_stream.close();
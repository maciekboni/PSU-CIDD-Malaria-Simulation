# -*- coding: utf-8 -*-
"""
Created on Mon Nov  1 21:42:34 2021

@author: nguyentd
"""


import yaml;
import numpy as np;
from math import log;
import copy;
import operator;
 
#import inflect;
#p = inflect.engine();

def kFormatter(num):
    return str(num) if num <=999 else str(round(num/1000)) +'k';


stream = open('input_DxG.yml', 'r');
data = yaml.load(stream);
stream.close();

sd_scales= {
        0.05: '0p05',
        0.1: '0p10', 
        0.2: '0p20'
        }

for sd_scale, sd_scale_str in sd_scales.items():
    for i in range(0,10):
        new_data = copy.deepcopy(data)
        
        for drug in data['drug_db']:
    #        print(drug)
            maxEC50 = max(data['drug_db'][drug]['EC50'].items(), key=operator.itemgetter(1))[1]
            
            minEC50 = min(data['drug_db'][drug]['EC50'].items(), key=operator.itemgetter(1))[1]
             
            if minEC50 == maxEC50:
                sd = minEC50 * sd_scale
                newMin = sd * np.random.randn() + minEC50
                newMax = sd * np.random.randn() + maxEC50
            else:
                sd = minEC50 * sd_scale
                newMin = sd * np.random.randn() + minEC50
                
                sd = maxEC50 * sd_scale
                newMax = sd * np.random.randn() + maxEC50
                
                while newMin > newMax:
                    #redraw
                    sd = minEC50 * sd_scale
                    newMin = sd * np.random.randn() + minEC50
                    
                    sd = maxEC50 * sd_scale
                    newMax = sd * np.random.randn() + maxEC50
            
#            print(minEC50,maxEC50, newMin, newMax);
            for ec50_k, ec50_v in data['drug_db'][drug]['EC50'].items():
                if minEC50 == maxEC50:
                    new_ec50 = newMin
                else: 
                    new_ec50 = (newMax - newMin)* (ec50_v - minEC50)/ ( maxEC50 - minEC50 ) + newMin
#                new_ec50 = (newMax - newMin)(ec50_v - minEC50)/ ( maxEC50 - minEC50 ) + newMin
                new_data['drug_db'][drug]['EC50'][ec50_k] = new_ec50
                print(ec50_k, ec50_v, new_ec50)
        
    
        output_filename = 'B1/min_max_scaled/sd_%s/EC50_profile_%d.yml'%(sd_scale_str, i); 
        output_stream = open(output_filename, 'w');
        yaml.dump(new_data, output_stream); 
        output_stream.close();

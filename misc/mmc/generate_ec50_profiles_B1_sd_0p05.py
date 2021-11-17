# -*- coding: utf-8 -*-
"""
Created on Mon Nov  1 21:42:34 2021

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


stream = open('input_DxG.yml', 'r');
data = yaml.load(stream);
stream.close();


for i in range(0,10):
    new_data = copy.deepcopy(data)
    
    for drug in data['drug_db']:
        print(drug)
        for ec50_k, ec50_v in data['drug_db'][drug]['EC50'].items():
            sd = ec50_v * 0.05
            new_ec50 = sd * np.random.randn() + ec50_v
            new_data['drug_db'][drug]['EC50'][ec50_k] = new_ec50
            print(ec50_k, ec50_v, new_ec50)
    
    
    output_filename = 'B1/cd_0p05/EC50_profile_%d.yml'%(i);
    output_stream = open(output_filename, 'w');
    yaml.dump(new_data, output_stream); 
    output_stream.close();

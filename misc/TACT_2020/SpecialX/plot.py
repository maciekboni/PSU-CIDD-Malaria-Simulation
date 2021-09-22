# -*- coding: utf-8 -*-

import pandas as pd


df = pd.read_csv('result.csv',sep='\t', header=None)


columns = [ '','','','','','','','','EC50', '','','','Efficacy'] 
df.columns = columns
 
#%%

import seaborn as sns

sns.regplot(data=df, x='EC50', y='Efficacy')
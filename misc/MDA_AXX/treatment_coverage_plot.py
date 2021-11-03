# -*- coding: utf-8 -*-
"""
Created on Thu Oct 14 15:37:08 2021

@author: nguyentd
"""

dates = pd.date_range(start= "1 1 2012", end= "1 1 2032", freq="MS")
coverage = [0.5]*(10*12+1) 

coverage.extend([0.8]*6)
coverage.extend([0.5]*(20*12 - 10*12 - 6))

#%%
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import datetime as dt
#sns.set_theme(style="darkgrid")

plt.close('all')
ax = sns.lineplot(dates, coverage,linewidth =4)
dr = pd.date_range(start= "1 1 2022", end= "1 1 2032", freq="5YS")
ax.set_xlim(dt.date(2020, 1, 1), dt.date(2032, 1, 1))
ax.set_xticks(dr)
ax.set_xticklabels(range(2022,2033,5))

ax.set_ylabel("Treatment Coverage",linespacing=1.5)

ax.set_xlabel("Time",linespacing=1.5)
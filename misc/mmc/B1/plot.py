

import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import matplotlib
import numpy as np

palette = sns.color_palette()[0:3]
fs = 12
font = {'family' : 'normal',
        'weight' : 'normal',
        'size'   : fs }

matplotlib.rc('font', **font)
plt.rc('text', usetex=False)

sns.set( font_scale=2.5)
sns.set_style("whitegrid", {'ytick.left': True })

dpi=72
fig_w = 3240/dpi
fig_h = fig_w*3/4
sns.set(rc={'figure.figsize':(fig_w,fig_h)})


drugs = ["AL", "ASAQ", "DHAPPQ"]

sds = ["0p05", "0p10", "0p20"]

plt.close("all")

for drug in drugs:
    eff_table = pd.DataFrame()     
    for sd in sds:
        eff_orig = pd.read_csv("EfficacyTable_Original.csv", encoding='utf-16') 
        eff_orig = pd.melt(eff_orig, id_vars=['ID'], value_vars=[drug], var_name='Drugs', value_name='Efficacy')
        eff_orig['Drugs'] = '%s_Orig'%(drug)
        eff_orig['sd'] = sd
        eff_table = eff_table.append(eff_orig, ignore_index=True)
        for i in range(0,10):
            fn = "sd_%s/EfficacyTable_%d.csv"%(sd,i)
            eff = pd.read_csv(fn, encoding='utf-16')
            eff = pd.melt(eff, id_vars=['ID'], value_vars=[drug], var_name='Drugs', value_name='Efficacy')
            eff['sd'] = sd
            eff_table = eff_table.append(eff, ignore_index=True)
        
        
#    plt.close("all")
    eff_table = eff_table[eff_table.ID%2==0]
    hue_order= ["%s"%drug, "%s_Orig"%drug]
    
    eff_table =eff_table.sort_values('Drugs', key=np.vectorize(hue_order.index))
    
    g = sns.relplot(x="ID", y="Efficacy", hue="Drugs", col="sd",
                col_order = ["0p05", "0p10", "0p20"],
#                hue_order= ["%s"%drug, "%s_Orig"%drug],
                data=eff_table, );
    g.fig.subplots_adjust(top=0.9)
    g.fig.suptitle(drug)

    
##%%
#
#
#
#
#
#eff_table = pd.read_csv("EfficacyTable_Original.csv", encoding='utf-16')
#
#eff_table = pd.melt(eff_table, id_vars=['ID'], value_vars=['AL'], var_name='Drugs', value_name='Efficacy')
#eff_table['Drugs'] = 'AL_Orig'
##%%
#for i in range(0,10):
#    fn = "sd_0p20/EfficacyTable_%d.csv"%i
#    eff = pd.read_csv(fn, encoding='utf-16')
#    eff = pd.melt(eff, id_vars=['ID'], value_vars=['AL'], var_name='Drugs', value_name='Efficacy')
#    eff_table = eff_table.append(eff, ignore_index=True)
#    print(fn)
#
##%%
#
#import seaborn as sns
#import matplotlib.pyplot as plt
#plt.close("all")
#
#sns.relplot(x="ID", y="Efficacy", hue="Drugs", data=eff_table);
#




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
fig_w = 1920/dpi
fig_h = fig_w*3/4
sns.set(rc={'figure.figsize':(fig_w,fig_h)})

drugs = ["AL", "ASAQ", "DHAPPQ"]

fn = "EfficacyTable_B2_v1.csv"
eff = pd.read_csv(fn, encoding='utf-16')
eff = pd.melt(eff, id_vars=['ID'], value_vars=drugs, var_name='Drugs', value_name='Efficacy')

sns.relplot(x="ID", y="Efficacy", hue="Drugs", col="Drugs", data=eff)


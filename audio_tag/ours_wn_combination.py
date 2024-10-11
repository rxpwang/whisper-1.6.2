import numpy as np
import pandas as pd

df_ours = pd.read_csv("audio-raw_medium_epoch_230.csv", header=None)
ours = df_ours.values
ours = ours.squeeze()
#print(ours)
#print(len(ours))
for i in [0.5, 1]:
    df_wn = pd.read_csv("wn_tag_"+str(i)+".csv", header=None)
    wn = df_wn.values
    wn = wn.squeeze()
    ours_wn_suffix = ours.tolist() + wn.tolist()
    ours_wn_prefix = wn.tolist() + ours.tolist()
    print(len(ours_wn_suffix))
    print(len(ours_wn_prefix))
    df_ours_wn_suffix = pd.DataFrame(ours_wn_suffix)
    df_ours_wn_prefix = pd.DataFrame(ours_wn_prefix)
    df_ours_wn_suffix.to_csv("ours_wn_suffix_"+str(i)+".csv", index=False, header=False)
    df_ours_wn_prefix.to_csv("ours_wn_prefix_"+str(i)+".csv", index=False, header=False)

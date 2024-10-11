import numpy as np
import pandas as pd

for i in [0.5, 1]:
    zero_tag = np.random.normal(0, 1, int(i * 16000))
    zero_tag_df = pd.DataFrame(zero_tag)
    zero_tag_df.to_csv("wn_tag_"+str(i)+".csv", index=False, header=False)
    #print("Zero tag saved to wn_tag_4.csv")

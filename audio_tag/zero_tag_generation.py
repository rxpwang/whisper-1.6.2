import numpy as np
import pandas as pd

zero_tag = np.zeros(64000)
zero_tag_df = pd.DataFrame(zero_tag)
zero_tag_df.to_csv("zero_tag_4.csv", index=False, header=False)
print("Zero tag saved to zero_tag_4.csv")

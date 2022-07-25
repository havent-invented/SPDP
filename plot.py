import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import numpy as np
Mas = pd.read_csv("./log1.csv").to_numpy()
x = Mas[:,0]
y = Mas[:,1]
z = Mas[:,-1]
Mas_pd = pd.read_csv("./log2.csv")
Mas_pd.columns = ["size", "cpu_count", "singlecoreTime", "Time"]
x = []
y = []
z = []
for (i,j), val in zip(Mas_pd.groupby(['size', 'cpu_count']).Time.mean().keys(), Mas_pd.groupby(['size', 'cpu_count']).Time.mean()):  
    x.append(i)
    y.append(j)
    z.append(val)
import plotly.express as px


fig = px.scatter_3d(x=x, y=y, z=z, size_max=1)
fig.update_layout(scene = dict(xaxis_title="Size", yaxis_title="Cpu count", zaxis_title="Time") )

fig.update_traces(marker=dict(size=3,))
fig.show()
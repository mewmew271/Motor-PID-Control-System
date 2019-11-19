import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv("system_response.csv")
#print(data.head())

plt.plot(data['time'],data['no_control_response'])
plt.plot(data['time'],data['control_response'])
plt.xlabel('Timesteps')
plt.ylabel('value')
plt.savefig('system_response.png')

plt.show()

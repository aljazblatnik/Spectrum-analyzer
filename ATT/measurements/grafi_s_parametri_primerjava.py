# narise grafe na podlagi S parametrov iz datoteke .S2P HP - VNA utility
# pip install scikit-rf

from skrf import Network
import matplotlib.pyplot as plt
from matplotlib import style

files = [
    'prototip1/0dB.S2P',
    'prototip2/0dB.S2P',
    'prototip1/10dB.S2P',
    'prototip2/10dB.S2P',
    'prototip1/20dB.S2P',
    'prototip2/20dB.S2P',
    'prototip1/30dB.S2P',
    'prototip2/30dB.S2P'
]

for file in files:
    ring_slot = Network(file)
    ring_slot.plot_s_db(m=1,n=0)

plt.title('PE43711B')
plt.legend(loc='lower left')
plt.grid(visible=True, axis='both', which='both')
plt.show()
# narise grafe na podlagi S parametrov iz datoteke .S2P HP - VNA utility
# pip install scikit-rf

from skrf import Network
import matplotlib.pyplot as plt
from matplotlib import style

files = [
    '0dB.S2P',
    '0_25dB.S2P',
    '0_5dB.S2P',
    '1dB.S2P',
    '2dB.S2P',
    '4dB.S2P',
    '8dB.S2P',
    '16dB.S2P',
    '30dB.S2P',
]
"""
files = [
    '0dB.S2P',
    '10dB.S2P',
    '20dB.S2P',
    '30dB.S2P'
]
"""
for file in files:
    ring_slot = Network(file)
    ring_slot.plot_s_db(m=1,n=0)

plt.title('PE43711B')
plt.legend(loc='lower right')
plt.grid(visible=True, axis='both', which='both')
plt.show()
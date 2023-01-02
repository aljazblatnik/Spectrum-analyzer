# narise grafe na podlagi S parametrov iz datoteke .S2P HP - VNA utility
# pip install scikit-rf

from skrf import Network
import matplotlib.pyplot as plt
from matplotlib import style

ring_slot = Network('0dB.S2P')
ring_slot. frequency.unit = 'ghz'
ring_slot.plot_s_db(m=1,n=0, label = '0dB')

ring_slot = Network('10dB.S2P')
ring_slot. frequency.unit = 'ghz'
ring_slot.plot_s_db(m=1,n=0, label = '10dB')

ring_slot = Network('20dB.S2P')
ring_slot. frequency.unit = 'ghz'
ring_slot.plot_s_db(m=1,n=0, label = '20dB')

ring_slot = Network('30dB.S2P')
ring_slot. frequency.unit = 'ghz'
ring_slot.plot_s_db(m=1,n=0, label = '30dB')

plt.title('PE43711B')
plt.axis([0000000000,6000000000,-40,0])
plt.legend(loc='lower right')
plt.grid(visible=True, axis='both', which='both')
plt.xlabel("Frekvenca [GHz]")
plt.ylabel("S21 [dB]")
plt.show()
plt.show()

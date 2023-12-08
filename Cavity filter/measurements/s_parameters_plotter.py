# narise grafe na podlagi S parametrov iz datoteke .S2P HP - VNA utility
# pip install scikit-rf

from skrf import Network
import matplotlib.pyplot as plt
from matplotlib import style

ring_slot = Network('#6-full.s2p')
ring_slot. frequency.unit = 'ghz'
ring_slot.plot_s_db(m=1,n=0, label = 'S21')
#ring_slot.plot_s_db(m=0,n=0, label = 'S11')
#ring_slot.plot_s_db(m=0,n=0)

plt.title('Wideband response')
plt.axis([0000000000,25000000000,-90,10])
plt.legend(loc='upper right')
plt.grid(visible=True, axis='both', which='both')
plt.xlabel("Frequency [GHz]")
plt.ylabel("Amplitude [dB]")
plt.show()
plt.show()

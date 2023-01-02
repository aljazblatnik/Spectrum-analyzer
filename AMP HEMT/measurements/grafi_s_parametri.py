# narise grafe na podlagi S parametrov iz datoteke .S2P HP - VNA utility
# pip install scikit-rf

from skrf import Network
import matplotlib.pyplot as plt
from matplotlib import style


# MERITVE
ring_slot = Network('43r1.s2p')
ring_slot. frequency.unit = 'ghz'
ring_slot.plot_s_db(m=0,n=0, label = 'S11')
ring_slot.plot_s_db(m=1,n=1, label = 'S22')
ring_slot.plot_s_db(m=1,n=0, label = 'S21')

# SIMULACIJA
ring_slot = Network('simulacija_43R.s2p')
ring_slot. frequency.unit = 'ghz'
ring_slot.plot_s_db(m=1,n=0, ls='--', label = 'S21 - sim')
#ring_slot.plot_s_db(m=0,n=0, label = 'S11')

plt.title('S parametri')
#plt.axis([400000000,2000000000,-3,0])
plt.axis([2000000000,10000000000,-30,20])
plt.legend(loc='lower right')
plt.grid(visible=True, axis='both', which='both')
#plt.minorticks_on()
plt.xlabel("Frekvenca [GHz]")
plt.ylabel("Amplituda [dB]")
plt.show()

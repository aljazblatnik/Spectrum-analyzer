import pyvisa as visa
import csv
rm = visa.ResourceManager()
print("Najdene naprave na vodilu: ")
print(rm.list_resources())
inst = rm.open_resource('GPIB0::17::INSTR')

while(True):
    filename = str(input("\nIme datoteke (0 - konec meritev): "))
    filename = filename + ".csv"
    if(filename == "0.csv"):
        break
    print("Zacenjam meritev ...")
    amplitude = inst.query("A?")
    phase = inst.query("B?")
    frequency = inst.query("X?")

    # change to list
    amplitude = [x.strip().rstrip() for x in amplitude.split(',')]
    phase = [x.strip().rstrip() for x in phase.split(',')]
    frequency = [x.strip().rstrip() for x in frequency.split(',')]

    # odpremo .csv datoteko
    print("Shrenjujem datoteko ", filename)
    f = open(filename, 'wt')
    writer = csv.writer(f)
    for index, value in enumerate(frequency):
        writer.writerow((float(value),float(amplitude[index]),float(phase[index])))
    f.close()
    print("Pripravljam za novo meritev ...")

print("Zapiram program.")
rm.close()

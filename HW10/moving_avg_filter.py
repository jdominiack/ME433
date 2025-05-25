import csv
import matplotlib.pyplot as plt # for plotting
import numpy as np # for sine function

t = [] # column 0
data1 = [] # column 1
filter = []
X = 3
temp = []
file = 'sigD'

with open(file + '.csv') as f:
    # open the csv file
    reader = csv.reader(f)

    counter = 0

    for row in reader:
        # read the rows 1 one by one
        t.append(float(row[0])) # leftmost column
        data1.append(float(row[1])) # second column

        temp.append(float(row[1]))
        counter+=1

        if counter == X:
            #temp = np.array(temp)
            avg = sum(temp)/len(temp)
            filter.append(avg)
            counter = 0

t = np.array(t) 

tFilter = np.linspace(t[0], t[-1], int(len(t)/X))

Fs = len(t)/t[-1] # sample rate
Fsf = len(tFilter)/tFilter[-1]

# for i in range(t):
#     # print the data to verify it was read
#     print(str(t[i]) + ", " + str(data1[i]) + ", " + str(data2[i]))

#Unfiltered:
Ts = 1.0/Fs; # sampling interval
ts = np.arange(0,t[-1],Ts) # time vector
y = data1 # the data to make the fft from
n = len(y) # length of the signal
k = np.arange(n)
T = n/Fs
frq = k/T # two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range
Y = np.fft.fft(y)/n # fft computing and normalization
Y = Y[range(int(n/2))]

#Filtered:
Tsf = 1.0/Fsf; # sampling interval
tsf = np.arange(0,tFilter[-1],Tsf) # time vector
yf = filter # the data to make the fft from
nf = len(yf) # length of the signal
kf = np.arange(nf)
Tf = nf/Fsf
frqf = kf/Tf # two sides frequency range
frqf = frqf[range(int(nf/2))] # one side frequency range
Yf = np.fft.fft(yf)/nf # fft computing and normalization
Yf = Yf[range(int(nf/2))]

print(str(len(filter)))

fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(t,data1,'k')
ax1.plot(tFilter, filter, 'r')
ax1.set_xlabel('Time (Sec)')
ax1.set_ylabel('Amplitude')
ax1.set_title(file + ' vs. Time, X = ' + str(X))
ax2.loglog(frq,abs(Y),'k') # plotting the unfiltered fft
ax2.loglog(frqf,abs(Yf),'r') # plotting the filtered fft
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.show()


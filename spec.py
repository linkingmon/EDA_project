import matplotlib.pyplot as plt
import numpy as np

for number in range(97, 123):
    filename = 'special_res/res{}.txt'.format(chr(number))
    filename_t = 'special/case{}.txt'.format(chr(number))
    plt.figure()
    with open(filename, 'r') as f:
        L = f.read().splitlines()[2:]
        plt.subplot(1, 2, 1)
        for i in range(len(L)):
            ll = L[i].split(' ')
            if ll[0] == 'POLYGON':
                arr = np.array([int(t) for t in ll[1:len(ll)-1]])
                arr = np.reshape(arr, (len(arr)//2, -1))
                for j in range(arr.shape[1]-1):
                    plt.plot(arr[:, j], arr[:, j+1], 'b')
            elif ll[0] == 'HOLE':
                arr = np.array([int(t) for t in ll[1:len(ll)-1]])
                arr = np.reshape(arr, (len(arr)//2, -1))
                for j in range(arr.shape[1]-1):
                    plt.plot(arr[:, j], arr[:, j+1], 'r')
            else:
                pass
    with open(filename_t, 'r') as f:
        L = f.read().splitlines()[2:]
        plt.subplot(1, 2, 2)
        cnt = 0
        for i in range(len(L)):
            ll = L[i].split(' ')
            if ll[0] == 'POLYGON':
                color = 'b'
                if(cnt == 1):
                    color = 'm'
                cnt += 1
                arr = np.array([int(t) for t in ll[1:len(ll)-1]])
                arr = np.reshape(arr, (len(arr)//2, -1))
                for j in range(arr.shape[1]-1):
                    plt.plot(arr[:, j], arr[:, j+1], color)
            else:
                pass
    plt.savefig('special_res/res{}.png'.format(chr(number)))

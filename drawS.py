import matplotlib.pyplot as plt
import numpy as np

filename_t = 'split/total4_SH.out'
filename = 'split/split4_SH.out'
with open(filename, 'r') as f:
    L = f.read().splitlines()
    # plt.hold(True)
    # print(L)
    operation = 'None'
    plt.figure()
    plt.subplot(1,2,1)
    for i in range(len(L)):
        ll = L[i].split(' ')
        if ll[0]=='RECT':
            arr = np.array([[ll[1], ll[3], ll[3], ll[1], ll[1]], 
                            [ll[2], ll[2], ll[4], ll[4], ll[2]]], dtype='int')
            
            plt.plot(arr[0], arr[1], 'm')

        # if ll[0] == 'POLYGON':
        #     arr = np.array([int(t) for t in ll[1:len(ll)-1]])
        #     arr = np.reshape(arr, (len(arr)//2, -1))
        #     color = 'b'
        #     for j in range(arr.shape[1]-1):
        #         plt.plot(arr[:, j], arr[:, j+1], color, alpha=0.5)

with open(filename_t, 'r') as f:
    L = f.read().splitlines()[2:]
    plt.subplot(1,2,2)
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
    plt.show()

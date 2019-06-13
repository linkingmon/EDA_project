import matplotlib.pyplot as plt
import numpy as np

filename = 'split/split4_SH.out'
with open(filename, 'r') as f:
    L = f.read().splitlines()
    plt.figure()
    # plt.hold(True)
    # print(L)
    operation = 'None'
    for i in range(len(L)):
        ll = L[i].split(' ')
        if ll[0]=='RECT':
            arr = np.array([[ll[1], ll[3], ll[3], ll[1], ll[1]], 
                            [ll[2], ll[2], ll[4], ll[4], ll[2]]], dtype='int')
            
            plt.plot(arr[0], arr[1], 'r')

        if ll[0] == 'POLYGON':
            arr = np.array([int(t) for t in ll[1:len(ll)-1]])
            arr = np.reshape(arr, (len(arr)//2, -1))
            color = 'b'
            for j in range(arr.shape[1]-1):
                plt.plot(arr[:, j], arr[:, j+1], color, alpha=0.5)
    # plt.show()
    plt.savefig('split/split4_SH_res.png')
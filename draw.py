import matplotlib.pyplot as plt
import numpy as np

# filename = 'sample_in.txt'
filename = 'OpenCase_1.txt'
with open(filename, 'r') as f:
    L = f.read().splitlines()[2:]
    plt.figure()
    # plt.hold(True)
    # print(L)
    operation = 'None'
    for i in range(len(L)):
        ll = L[i].split(' ')
        # print(ll)
        if ll[0] == 'DATA':
            print('Operation:', ll[1])
            operation = ll[1]
        if ll[0] == 'POLYGON':
            arr = np.array([int(t) for t in ll[1:len(ll)-1]])
            arr = np.reshape(arr, (len(arr)//2, -1))
            color = 'b' if operation == 'MERGE' else 'r'
            for j in range(arr.shape[1]-1):
                plt.plot(arr[:, j], arr[:, j+1], color)
    plt.show()
    # plt.savefig('sample.png')

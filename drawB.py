import matplotlib.pyplot as plt
import numpy as np

cnt = 1
# filename = 'sample_in.txt'
for char in range(0, 16):
    filename = 'ALL{}.txt'.format(chr(char+65))
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
                # print('Operation:', ll[1])
                operation = ll[1]
                print(operation)
            if ll[0] == 'POLYGON':
                arr = np.array([int(t) for t in ll[1:len(ll)-1]])
                arr = np.reshape(arr, (len(arr)//2, -1))
                color = 'b' if operation == 'MERGE' else 'r'
                if(operation != 'MERGE'):
                    print(arr)
                for j in range(arr.shape[1]-1):
                    plt.plot(arr[:, j], arr[:, j+1], color)
            elif ll[0] == 'HOLE':
                arr = np.array([int(t) for t in ll[1:len(ll)-1]])
                arr = np.reshape(arr, (len(arr)//2, -1))
                color = 'm' if operation == 'MERGE' else 'r'
                print(arr)
                for j in range(arr.shape[1]-1):
                    plt.plot(arr[:, j], arr[:, j+1], color)
            if ll[0] == 'END':
                pass
        plt.show()
        # plt.savefig('result/sample.png'.format(cnt))

import matplotlib.pyplot as plt
import numpy as np

cnt = 1
filename_t = 'split/total4_SH.out'
filename = './OP1.txt'
with open(filename, 'r') as f:
    L = f.read().splitlines()[2:]
    plt.figure()
    # plt.hold(True)
    # print(L)
    plt.subplot(1, 2, 1)
    operation = 'None'
    for i in range(len(L)):
        ll = L[i].split(' ')
        # print(ll)
        if ll[0] == 'DATA':
            # print('Operation:', ll[1])
            operation = ll[1]
        if ll[0] == 'POLYGON':
            arr = np.array([int(t) for t in ll[1:len(ll)-1]])
            arr = np.reshape(arr, (len(arr)//2, -1))
            color = 'b' if operation == 'MERGE' else 'r'
            for j in range(arr.shape[1]-1):
                plt.plot(arr[:, j], arr[:, j+1], color)
        if ll[0] == 'END':
            # plt.savefig('result2/sample{}.png'.format(cnt))
            cnt += 1
with open(filename_t, 'r') as f:
    L = f.read().splitlines()[2:]
    # plt.hold(True)
    # print(L)
    plt.subplot(1, 2, 2)
    operation = 'None'
    for i in range(len(L)):
        ll = L[i].split(' ')
        # print(ll)
        if ll[0] == 'DATA':
            # print('Operation:', ll[1])
            operation = ll[1]
        if ll[0] == 'POLYGON' or ll[0] == 'HOLE':
            arr = np.array([int(t) for t in ll[1:len(ll)-1]])
            arr = np.reshape(arr, (len(arr)//2, -1))
            color = 'b' if operation == 'MERGE' else 'r'
            for j in range(arr.shape[1]-1):
                plt.plot(arr[:, j], arr[:, j+1], color)
        if ll[0] == 'END':
            # plt.savefig('result2/sample{}.png'.format(cnt))
            plt.show()
            plt.clf()
            cnt += 1

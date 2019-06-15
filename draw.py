import matplotlib.pyplot as plt
import numpy as np

cnt = 1
# filename = 'poly_with_hole.txt'
# filename = 'test_merge.txt'
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
            # print('Operation:', ll[1])
            operation = ll[1]
        if ll[0] == 'POLYGON':
            arr = np.array([int(t) for t in ll[1:len(ll)-1]])
            arr = np.reshape(arr, (len(arr)//2, -1))
            color = 'b' if operation == 'MERGE' else 'r'
            for j in range(arr.shape[1]-1):
                plt.plot(arr[:, j], arr[:, j+1], color)
        if ll[0] == 'END':
            plt.show()
            # plt.savefig('result/sample{}.png'.format(cnt))
            plt.clf()
            cnt += 1

for zz in range(cnt-1):
    filename = 'result/Merge{}.txt'.format(zz+1)
    try:
        with open(filename, 'r') as f:
            L = f.read().splitlines()[2:]
            plt.figure()
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
                    plt.savefig('result/res{}.png'.format(zz+1))
                    plt.clf()
    except:
        print("NO file", filename)

for zz in range(cnt-1):
    filename = 'result/Total{}.txt'.format(zz+1)
    try:
        with open(filename, 'r') as f:
            L = f.read().splitlines()[2:]
            plt.figure()
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
                    plt.savefig('result/overall{}.png'.format(zz+1))
                    plt.clf()
    except:
        print("NO file", filename)

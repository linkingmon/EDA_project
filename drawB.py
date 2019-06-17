import matplotlib.pyplot as plt
import numpy as np

cnt = 1
# filename = 'sample_in.txt'
for char in range(18, 20):
    filename = 'BUG/ALL{}.txt'.format(char)
    with open(filename, 'r') as f:
        L = f.read().splitlines()[2:]
        plt.figure()
        # plt.hold(True)
        # print(L)
        operation = 'None'
        poly_cnt = 0
        for i in range(len(L)):
            ll = L[i].split(' ')
            # print(ll)
            if ll[0] == 'DATA':
                # print('Operation:', ll[1])
                operation = ll[1]
                print(operation)
            if ll[0] == 'POLYGON':
                poly_cnt += 1
                arr = np.array([int(t) for t in ll[1:len(ll)-1]])
                arr = np.reshape(arr, (len(arr)//2, -1))
                color = 'b' if operation == 'MERGE' else 'r'
                if(operation != 'MERGE'):
                    print(arr)
                for j in range(arr.shape[1]-1):
                    plt.plot(arr[:, j], arr[:, j+1], color)
            elif ll[0] == 'HOLE':
                poly_cnt += 1
                arr = np.array([int(t) for t in ll[1:len(ll)-1]])
                arr = np.reshape(arr, (len(arr)//2, -1))
                color = 'm' if operation == 'MERGE' else 'r'
                print(arr)
                for j in range(arr.shape[1]-1):
                    plt.plot(arr[:, j], arr[:, j+1], color)
            if ll[0] == 'END':
                pass
        plt.text(3000000, 3500000, 'Poly count: {}'.format(poly_cnt-1), fontsize=12)
        plt.show()
        # plt.savefig('result/sample.png'.format(cnt))

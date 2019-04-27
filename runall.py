import os

tt = [
    # '16_1',
    # '20_3',
    # '20_5',
    # '30_10',
    # '50_10',
    # '100_50',
    # '10000_10',
    # '100000_50',
    # '100000_100',
    # '1000000_50',
    # '1000000_100',
    # '10000000_1',
    # '10000000_100',
    '10000000_10000',
]

for t in tt:
    os.system('echo ' + t + ' && time mpirun -np 1 ./ppm testcase/testcase_' + t + ' > seqjaja/' + t)

for t in tt:
    # print (t)
    os.system('echo ' + t + ' && time diff brute/' + t + ' seqjaja/' + t + ' > diffs/' + t)
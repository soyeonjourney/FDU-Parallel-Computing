import random
import string
import sys
import os

INPUT_FILE = 'input.txt'

def random_string(_len: int) -> str:
    alphabet = string.ascii_letters + string.digits
    return ''.join(random.choice(alphabet) for _ in range(_len))

def generate_lcs_testfile(test_num: int, max_len: int):
    for i in range(1, test_num + 1):
        M = random.randint(int(max_len / 3), max_len)
        N = random.randint(int(max_len / 3), max_len)
        A = random_string(M)
        B = random_string(N)
        _input = ''.join([A, '\n', B, '\n'])

        with open(INPUT_FILE, 'w') as f:
            f.write(_input)

    return

if __name__ == '__main__':
    test_num = int(sys.argv[1])
    max_len = int(sys.argv[2])

    generate_lcs_testfile(test_num, max_len)
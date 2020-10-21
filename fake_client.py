import time
import random

while True:
    value = random.randint(1, 200)
    print('Sleeping for', value, 'seconds')
    time.sleep(0.5)
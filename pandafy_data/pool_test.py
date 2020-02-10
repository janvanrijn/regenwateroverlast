import numpy as np

import threading
import time
import logging

logging.basicConfig(level=logging.DEBUG,
                    format='(%(threadName)-10s) %(message)s',
                    )

def daemon(i):
    results[i] = i
    return

n = 3
results = [-1]*n
print(results)
threads = []
for i in range(n):
    threads.append(threading.Thread(name='daemon', target=daemon(i)))

for i in range(len(threads)):
    threads[i].start()

for i in range(len(threads)):
    threads[i].join()
print(results)

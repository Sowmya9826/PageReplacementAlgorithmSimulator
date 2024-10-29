gcc driver.c algorithms.c page_utils.c  -o program
./program FIFO > FIFO                       
./program LFU > LFU  
./program LRU > LRU
./program MFU > MFU
./program RANDOM > RANDOM

echo "MFU"
tail -n 2 MFU
echo "\n"
echo "FIFO"
tail -n 2 FIFO
echo "\n"
echo "RANDOM"
tail -n 2 RANDOM
echo "\n"
echo "LRU"
tail -n 2 LRU
echo "\n"
echo "LFU"
tail -n 2 LFU
echo "\n"

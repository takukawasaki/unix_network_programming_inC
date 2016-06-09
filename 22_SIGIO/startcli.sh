#!/bin/sh

for i in $(seq 0 10); do

    (sleep 1; echo "halel") |     /home/taku/Dropbox/C/network_programming_linux/22_SIGIO/udpcli 127.0.0.1 &
    (sleep 1; echo "halel") |     /home/taku/Dropbox/C/network_programming_linux/22_SIGIO/udpcli 127.0.0.1 &
    (sleep 1; echo "halel") |     /home/taku/Dropbox/C/network_programming_linux/22_SIGIO/udpcli 127.0.0.1 &
    (sleep 1; echo "halel") |     /home/taku/Dropbox/C/network_programming_linux/22_SIGIO/udpcli 127.0.0.1 &
    (sleep 1; echo "halel") |     /home/taku/Dropbox/C/network_programming_linux/22_SIGIO/udpcli 127.0.0.1 &
    (sleep 1; echo "halel") |     /home/taku/Dropbox/C/network_programming_linux/22_SIGIO/udpcli 127.0.0.1 &
    wait
done




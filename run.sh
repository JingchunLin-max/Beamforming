#!/usr/bin/bash 

# Created on 2016-09-18
# Author: Zhang Binbin

# ./apply-delay-and-sum --tdoa-window=4000 --beam-window=4000 --margin=16 \
#     2ch01_test.wav 2ch01_test_delay_and_sum.wav

# ./apply-gsc --num-k=128 --alpha=0.01 \
#     2ch01_test.wav 2ch01_test_gsc.wav

./apply-mvdr --frame-len=0.025 --frame-shift=0.01 --fft-point=2048 \
    --energy-thresh=1.5e-7 \
    --sil-to-speech-trigger=3 \
    --speech-to-sil-trigger=10 \
    sound_main_ch05.wav sound_test_main_ch05_mvdr.wav






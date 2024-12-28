#!/bin/bash
whisper-qt.app/Contents/MacOS/whisper-qt    \
    -m ../../models/ggml-small.bin \
    samples/bernie30s.wav \
    -kc -dtw small -ac -1 \
    -at ../../audio_tag/small_0.5s_avg.csv
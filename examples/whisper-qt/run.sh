#!/bin/bash
mkdir whisper-qt.app/Contents/Resources
cp ../../ggml-metal.metal whisper-qt.app/Contents/Resources/
cp ../../ggml-common.h whisper-qt.app/Contents/Resources/
whisper-qt.app/Contents/MacOS/whisper-qt    \
    -m ../../models/ggml-medium.bin \
    samples/bernie30s.wav \
    -kc -dtw medium -ac -1 \
    -at ../../audio_tag/medium_0.5s_avg.csv --step 10
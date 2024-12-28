# Whisper Streaming Demo

## Prerequisites
```
brew install sdl2
brew install qt
```

tested on qt version: 
```
brew info qt
==> qt: stable 6.7.3 (bottled), HEAD
```

## The Qt Demo

The code is under `examples/whisper-qt`.

```
cd examples/whisper-qt
```

To build:
```
./build.sh
```

To run:
```
./run.sh
```

https://github.com/user-attachments/assets/fa5a8b44-bbf4-4e70-813d-d77a62d65ad2

## The Console Demo

### Compilation
```
make clean
make
make whisper_streaming_cpp_optimized
```

### Command to run the demo
(Filename doesn't matter, the system doesn't work on an exact audio file but gets audio from the microphone)

```
./whisper_streaming_cpp_optimized -m models/ggml-small.bin samples/bernie30s.wav -kc -dtw small -ac -1 -at audio_tag/small_0.5s_avg.csv
```

A larger model and clearer/more steady input may work better.

The current demo tested with recorded speech played by the phone beside the MacBook works well.

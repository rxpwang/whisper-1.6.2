# stream

This is a naive example of performing real-time inference on existing audio files to simulate the whisper stream functionality.
```bash
./stream -m ./models/ggml-base.en.bin -t 8 --step 500 --length 5000
./stream_simulation -m models/ggml-base.bin samples/bernie4min.wav --step 3000 -kc
```
--step to set the step length
--keep-context / -kc to keep the transcript context

https://user-images.githubusercontent.com/1991296/194935793-76afede7-cfa8-48d8-a80f-28ba83be7d09.mp4

## Sliding window mode with VAD

Setting the `--step` argument to `0` enables the sliding window mode:

```bash
 ./stream -m ./models/ggml-small.en.bin -t 6 --step 0 --length 30000 -vth 0.6
```

In this mode, the tool will transcribe only after some speech activity is detected. A very
basic VAD detector is used, but in theory a more sophisticated approach can be added. The
`-vth` argument determines the VAD threshold - higher values will make it detect silence more often.
It's best to tune it to the specific use case, but a value around `0.6` should be OK in general.
When silence is detected, it will transcribe the last `--length` milliseconds of audio and output
a transcription block that is suitable for parsing.

## Building

The `stream` tool depends on SDL2 library to capture audio from the microphone. You can build it like this:

```bash
# Install SDL2
# On Debian based linux distributions:
sudo apt-get install libsdl2-dev

# On Fedora Linux:
sudo dnf install SDL2 SDL2-devel

# Install SDL2 on Mac OS
brew install sdl2

make stream
```

Ensure you are at the root of the repo when running `make stream`. Not within the `examples/stream` dir
as the libraries needed like `common-sdl.h` are located within `examples`. Attempting to compile within
`examples/steam` means your compiler cannot find them and it gives an error it cannot find the file.

```bash
whisper.cpp/examples/stream$ make stream
g++     stream.cpp   -o stream
stream.cpp:6:10: fatal error: common/sdl.h: No such file or directory
    6 | #include "common/sdl.h"
      |          ^~~~~~~~~~~~~~
compilation terminated.
make: *** [<builtin>: stream] Error 1
```

## Web version

This tool can also run in the browser: [examples/stream.wasm](/examples/stream.wasm)

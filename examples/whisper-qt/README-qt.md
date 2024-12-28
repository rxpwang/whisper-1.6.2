# the qt version -- by FL Dec 2024
## in whisper-1.6.2 directory
make libwhisper.a
make lib_whisper_stream.a

## qmake
install: 
brew install qt

## in whisper-1.6.2/examples/whisper-qt directory 
to build: 
qmake 
make

to run with default settings: 
test-qt.app/Contents/MacOS/test-qt

to run with our settings:
./run.sh

## cmake 
cannot get it work yet. TBD

####
the original build command

c++ -I. -I./examples -O3 -DNDEBUG -std=c++11 -fPIC -D_XOPEN_SOURCE=600 -D_DARWIN_C_SOURCE -pthread -DGGML_USE_METAL examples/whisper_streaming_cpp_optimized/whisper_streaming_cpp_optimized.cpp examples/common.cpp examples/common-ggml.cpp examples/grammar-parser.cpp examples/common-sdl.cpp ggml.o ggml-alloc.o ggml-backend.o ggml-quants.o whisper.o ggml-metal.o -o whisper_streaming_cpp_optimized `sdl2-config --cflags --libs`  -framework Accelerate -framework Foundation -framework Metal -framework MetalKit

# whisper_streaming_cpp

This is our optimized version of whisper_streaming on top of whisper.cpp
It follows the approaches of whisper_streaming, holds and actively maintains audio_buffer, transcript_buffer and prompt.
Beyond that, it also implement the encoding context reduction technique, decoding beam reduce technique, and encoding / prompting / decoding pipelining on CPU / GPU (ongoing).

params related:
-kc for prompt context.
-ng for no gpu execution.
-dtw medium for token-level timestamp, which is required for our method.
--step 3000 for setting step length to 3000, which is the default step size. We can also set other step size in milisecond.
-ac -1 for no zero padding for audio.
-at "audio_tag_file_path" for adding our audio tag at the end of the audio.

```bash
# execution with all the optimization
./whisper_streaming_cpp_optimized -m models/ggml-medium.bin samples/bernie4min.wav -ac -1 -at "audio_tag_file_path" -kc -dtw medium --step 3000 > streaming_log.log 2>&1
```

## Building

```bash
make clean
make
make whisper_streaming_cpp_optimized
```


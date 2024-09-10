# whisper_streaming_cpp

This is an implementation of whisper_streaming on top of whisper.cpp.
It follows the approaches of whisper_streaming, holds and actively maintains audio_buffer, transcript_buffer and prompt.

```bash
# with prompt on GPU
./whisper_streaming_cpp -m models/ggml-medium.bin samples/bernie4min.wav -kc -dtw medium > streaming_log.log 2>&1
# with prompt on CPU
./whisper_streaming_cpp -m models/ggml-medium.bin samples/bernie4min.wav -kc -ng -dtw medium > streaming_log.log 2>&1
# without prompt on GPU
./whisper_streaming_cpp -m models/ggml-medium.bin samples/bernie4min.wav -dtw medium > streaming_log.log 2>&1
```

## Building

```bash
make clean
make
make whisper_streaming_cpp
```


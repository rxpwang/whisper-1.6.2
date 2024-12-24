## tmp readme for demo
# install sdl
brew install sdl2
# compilation
make clean
make
make whisper_streaming_cpp_optimized
# command to run the demo (filename doesn't matter, system don't work on exact audio file but get audio from microphone)
./whisper_streaming_cpp_optimized -m models/ggml-small.bin samples/bernie30s.wav -kc -dtw small -ac -1 -at audio_tag/small_0.5s_avg.csv

# larger model and clearer / more steady input may work better.
# current demo tested with recorded speech played by the phone besides the macbook works well.

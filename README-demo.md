## tmp readme for demo
# command to run the demo
./whisper_streaming_cpp_optimized -m models/ggml-small.bin samples/bernie30s.wav -kc -dtw small -ac -1 -at audio_tag/small_0.5s_avg.csv

# larger model and clearer / more steady input may work better.
# current demo tested with recorded speech played by the phone besides the macbook works well.

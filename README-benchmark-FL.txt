From RW, 10/22/24

Pipeline with token level latency measurement experiments

# whisper_streaming_cpp experiments
 
# if you don't have folder benchmarking_results
# mkdir benchmarking_results
 
#######################################

# for serialized implementation
git checkout pipeline_serialized

make clean -4
make -4
make whisper_streaming_cpp_optimized -4

./whisper_streaming_cpp_optimized_pipeline_serialized_benchmarking.sh
 
#######################################

# for pipeline implementation

git checkout pipeline

make clean
make -j4
make whisper_streaming_cpp -j4
make whisper_streaming_cpp_optimized  -j4

./whisper_streaming_cpp_benchmarking.sh
# (next - this)
./whisper_streaming_cpp_optimized_pipeline_benchmarking.sh
 

#!/bin/bash

# Parameters from your command
model="ggml-medium.bin"
sample="bernie4min.wav"
dtw="medium"

# Get the number of all CPU cores on macOS, use all but 2 cores
total_threads=$(($(sysctl -n hw.physicalcpu)-2))
# Fallback to a default value if the command fails
if [ -z "$threads" ]; then
    threads="12"
fi

# Array of different step lengths to loop through
runs=5
step=100
max_decoder=5
# CPU can use at least `max_decoder` threads
for cpu_threads in $(seq $max_decoder $(($total_threads-1))); do
    # Run each config for `runs` times
    for i in $(seq 1 $runs); do
        gpu_threads=$(($total_threads - $cpu_threads))
        echo "Run ${i} cpu threads: ${cpu_threads}, gpu threads: ${gpu_threads}"
        # Create the log file name based on the parameters, including the current step length
        log_file="benchmarking_results/pipeline-v1_${model}_${sample}_dtw-${dtw}_step-${step}_ct-${cpu_threads}_gt-${gpu_threads}_with_audio_tag_hallucination_fixed_run-${i}.log"
        log_file_no_audio_tag="benchmarking_results/pipeline-v1_${model}_${sample}_dtw-${dtw}_step-${step}_ct-${cpu_threads}_gt-${gpu_threads}_run-${i}.log"
        
        # Run the command and redirect the output to the log file
        ./whisper_streaming_cpp_optimized -m models/$model samples/$sample -kc -dtw $dtw -ac -1 -at audio_tag/medium_0.5s_avg.csv --step $step -ct $cpu_threads -gt $gpu_threads > $log_file 2>&1
        echo "Output has been logged to ${log_file}"
        ./whisper_streaming_cpp_optimized -m models/$model samples/$sample -kc -dtw $dtw --step $step -ct $cpu_threads -gt $gpu_threads > $log_file_no_audio_tag 2>&1
        echo "Output has been logged to ${log_file_no_audio_tag}"
    done
done

if [[ "$OSTYPE" == "darwin"* ]]; then
    system_profiler SPHardwareDataType > benchmarking_results/00-systeminfo.txt
fi
# Append the current git commit hash to the system info file
echo "Current Git Commit:" >> benchmarking_results/00-systeminfo.txt
git rev-parse HEAD >> benchmarking_results/00-systeminfo.txt
#!/bin/bash

# if not at the root whisper directory, go one level up
if [[ "$(pwd)" != *"whisper"* ]]; then
    cd ..
fi

# Parameters from your command
model_type="base"
model="ggml-${model_type}.bin"
sample="bernie4min.wav"
dtw=${model_type}

# Get the number of all CPU cores on macOS, use all but 2 cores
total_threads=$(($(sysctl -n hw.physicalcpu)-2))
# Fallback to a default value if the command fails
if [ -z "$threads" ]; then
    threads="12"
fi

# Array of different step lengths to loop through
runs=5
step=100
cpu_threads=5 # use at least 5 threads for CPU functions
gpu_threads=$(($total_threads-$cpu_threads))
# Swap the values if gpu_threads is greater than cpu_threads
if [ $gpu_threads -gt $cpu_threads]; then
    $tmp=$gpu_threads
    $gpu_threads=$cpu_threads
    $cpu_threads=$tmp
fi

base_directory="benchmarking_results"
exp_name="max_decode_round_search"
# CPU can use at least `max_decoder` threads
result_dir="${base_directory}/${exp_name}/${model_type}"
mkdir -p $result_dir
for max_decoding_round in ${0.25 0.33 0.5 0.67 0.75}; do
    # Run each config for `runs` times
    for i in $(seq 1 $runs); do
        echo "Run ${i} cpu threads: ${cpu_threads}, gpu threads: ${gpu_threads}, max decoding round: ${max_decoding_round}"
        # Create the log file name based on the parameters, including the current step length
        log_file="pipeline-v1_${model}_${sample}_dtw-${dtw}_step-${step}_ct-${cpu_threads}_gt-${gpu_threads}_with_tag_run-${i}.log"
        log_file_no_audio_tag="pipeline-v1_${model}_${sample}_dtw-${dtw}_step-${step}_ct-${cpu_threads}_gt-${gpu_threads}_no_tag_run-${i}.log"
 
        # Run the command and redirect the output to the log file
        ./whisper_streaming_cpp_optimized -m models/$model samples/$sample -kc -dtw $dtw -ac -1 -at audio_tag/base_0.5s_avg.csv --step $step -ct $cpu_threads -gt $gpu_threads -mdr $max_decoding_round > $result_dir/$log_file 2>&1
        echo "Output has been logged to ${result_dir}/${log_file}"

        echo "Waiting for 30 seconds before the next run..." 
        sleep 30
        
        ./whisper_streaming_cpp_optimized -m models/$model samples/$sample -kc -dtw $dtw --step $step -ct $cpu_threads -gt $gpu_threads -mdr $max_decoding_round > $result_dir/$log_file_no_audio_tag 2>&1
        echo "Output has been logged to ${result_dir}/${log_file_no_audio_tag}"

        echo "Waiting for 60 seconds before the next run..."
        sleep 60
    done
done

if [[ "$OSTYPE" == "darwin"* ]]; then
    system_profiler SPHardwareDataType > benchmarking_results/00-systeminfo.txt
fi
# Append the current git commit hash to the system info file
echo "Current Git Commit:" >> benchmarking_results/00-systeminfo.txt
git rev-parse HEAD >> benchmarking_results/00-systeminfo.txt
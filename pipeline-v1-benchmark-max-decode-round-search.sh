#!/bin/bash

# if not at the root whisper directory, go one level up
if [[ "$(pwd)" != *"whisper"* ]]; then
    cd ..
fi

# Parameters from your command
model_type="medium"
repetitions=5

if [ $# == 1 ]; then
    model_type=$1
elif [ $# == 2 ]; then
    model_type=$1
    repetitions=$2
fi

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
step=10
cpu_threads=5 # use at least 5 threads for CPU functions
gpu_threads=$(($total_threads-$cpu_threads))
# Swap the values if gpu_threads is greater than cpu_threads
if [ $gpu_threads -gt $cpu_threads ]; then
    $tmp=$gpu_threads
    $gpu_threads=$cpu_threads
    $cpu_threads=$tmp
fi

base_directory="benchmarking_results"
exp_name="max_decode_round_search"
# CPU can use at least `max_decoder` threads
result_dir="${base_directory}/${exp_name}/${model_type}"
mkdir -p $result_dir
max_decoding_round_factor=(0.25 0.33 0.5 0.67 0.75)
for max_decoding_round in "${max_decoding_round_factor[@]}"; do
    # Run each config for `runs` times
    for i in $(seq 1 $repetitions); do
        echo "Run ${i} cpu threads: ${cpu_threads}, gpu threads: ${gpu_threads}, max decoding round: ${max_decoding_round}"
        # Create the log file name based on the parameters, including the current step length
        log_file="pipeline-v1_${model_type}_${sample}_step-${step}_ct-${cpu_threads}_gt-${gpu_threads}_mdr-${max_decoding_round}_with_tag_run-${i}.log"
        log_file_no_audio_tag="pipeline-v1_${model_type}_${sample}_step-${step}_ct-${cpu_threads}_gt-${gpu_threads}_mdr-${max_decoding_round}_no_tag_run-${i}.log"
 
        # Run the command and redirect the output to the log file
        ./whisper_streaming_cpp_optimized -m models/$model samples/$sample -kc -dtw $dtw -ac -1 -at audio_tag/${model_type}_0.5s_avg.csv --step $step -ct $cpu_threads -gt $gpu_threads -mdr $max_decoding_round > $result_dir/$log_file 2>&1
        echo "Output has been logged to ${result_dir}/${log_file}"

        echo "Waiting for 30 seconds before the next run..." 
        sleep 30
        
        ./whisper_streaming_cpp_optimized -m models/$model samples/$sample -kc -dtw $dtw --step $step -ct $cpu_threads -gt $gpu_threads -mdr $max_decoding_round > $result_dir/$log_file_no_audio_tag 2>&1
        echo "Output has been logged to ${result_dir}/${log_file_no_audio_tag}"

        if [ "$i" -lt "$repetitions" ]; then
            echo "Waiting for 30 seconds before the next run..."
            sleep 30
        fi
    done
done

if [[ "$OSTYPE" == "darwin"* ]]; then
    system_profiler SPHardwareDataType > ${result_dir}/00-systeminfo.txt
fi
# Append the current git commit hash to the system info file
echo "Current Git Commit:" >> ${result_dir}/00-systeminfo.txt
git rev-parse HEAD >> ${result_dir}/00-systeminfo.txt
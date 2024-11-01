#!/bin/bash

# Parameters from your command
model_type="medium"
model="ggml-${model_type}.bin"
sample="bernie4min.wav"
dtw=${model_type}

# Get the number of P-core count on macOS
threads=$(($(sysctl -n hw.perflevel0.physicalcpu)-1))

# Array of different step lengths to loop through
step=100
# Number of repetitions
repetitions=5
base_directory="benchmarking_results"
exp_name="main"
# CPU can use at least `max_decoder` threads
result_dir="${base_directory}/${exp_name}/${model_type}"
mkdir -p $result_dir
# Loop over each step length
for i in $(seq 1 $repetitions); do
    # Create the log file name based on the parameters, including the current step length
    echo "Run ${i}"
    log_file="${result_dir}/pipeline-v1_${model}_${sample}_dtw-${dtw}_step-${step}_t-${threads}_with_tag_run-${i}.log"
    log_file_no_audio_tag="${result_dir}/pipeline-v1_${model}_${sample}_dtw-${dtw}_step-${step}_t-${threads}_no_tag_run-${i}.log"
 
    # Run the command and redirect the output to the log file
    ./whisper_streaming_cpp_optimized -m models/$model samples/$sample -kc -dtw $dtw -ac -1 -at audio_tag/${model_type}_0.5s_avg.csv --step $step -t $threads > $log_file 2>&1
    echo "Output has been logged to $log_file"
    # sleep 30s in between
    sleep 30
    ./whisper_streaming_cpp_optimized -m models/$model samples/$sample -kc -dtw $dtw --step $step -t $threads > $log_file_no_audio_tag 2>&1
    echo "Output has been logged to $log_file_no_audio_tag"
    # Pause for 1 minute between repetitions, unless it's the last one
    if [ "$i" -lt "$repetitions" ]; then
        echo "Waiting for 1 minute before the next run..."
        sleep 60
    fi
done

if [[ "$OSTYPE" == "darwin"* ]]; then
    system_profiler SPHardwareDataType > benchmarking_results/00-systeminfo_${model}_whisper_streaming_cpp_pipeline.txt
fi
# Append the current git commit hash to the system info file
echo "Current Git Commit:" >> benchmarking_results/00-systeminfo_${model}_whisper_streaming_cpp_pipeline.txt
git rev-parse HEAD >> benchmarking_results/00-systeminfo_${model}_whisper_streaming_cpp_pipeline.txt
#!/bin/bash

# if not at the root whisper directory, go one level up
if [[ "$(pwd)" != *"whisper"* ]]; then
    cd ..
fi

model_type="medium"
dataset_name="ted-long-form-test-cut"

if [ $# == 1 ]; then
    model_type=$1
elif [ $# == 2 ]; then
    model_type=$1
    dataset_name=$2
fi

echo "Run evaluation on ${model_type} model"
# Parameters from your command
model="ggml-${model_type}.bin"
dtw=${model_type}

# Get the number of P-core count on macOS
threads=$(($(sysctl -n hw.physicalcpu)-1))
cpu_threads=5
gpu_threads=$(($threads-$cpu_threads))
if [ $gpu_threads -gt $cpu_threads ]; then
    # swap the values
    tmp=$cpu_threads
    cpu_threads=$gpu_threads
    gpu_threads=$tmp
fi

echo "CPU threads: ${cpu_threads}, GPU threads: ${gpu_threads}"

mdr=0.5
echo "Max decoding round: ${mdr} of reference transcript length"

# Array of different step lengths to loop through
step=100
# Number of repetitions
result_base_directory="benchmarking_results"
data_base_directory="benchmarking_data"
exp_name="fig1"
# CPU can use at least `max_decoder` threads
result_dir="${result_base_directory}/${exp_name}/${model_type}"
data_dir="${data_base_directory}/${dataset_name}"
mkdir -p $result_dir
# Loop over each step length
log_file="${result_dir}/pipeline-v1_${model_type}_${dataset_name}_step-${step}_ct-${cpu_threads}_gt-${gpu_threads}_mdr-${mdr}_with_tag.log"
log_file_no_audio_tag="${result_dir}/pipeline-v1_${model_type}_${dataset_name}_step-${step}_ct-${cpu_threads}_gt-${gpu_threads}_mdr-${mdr}_no_tag.log"

# clear the log file from previous run
rm -f $log_file
rm -f $log_file_no_audio_tag

total_samples=$(ls $data_dir | grep "wav" | wc -l)

i=1
for sample in $(ls $data_dir | grep "wav"); do
    # Create the log file name based on the parameters, including the current step length
    progress=$((i*100/total_samples))
    printf "\rProgress: [%-50s] %d%%" $(head -c $(( progress / 2 )) < /dev/zero | tr '\0' '#') $progress
    # Run the command and redirect the output to the log file
    echo $sample >> $log_file
    ./whisper_streaming_cpp_optimized -m models/$model $data_dir/$sample -kc -dtw $dtw -ac -1 -at audio_tag/${model_type}_0.5s_avg.csv --step $step -ct $cpu_threads -gt $gpu_threads -mdr $mdr >> $log_file 2>&1
    
    # echo $sample >> $log_file
    # ./whisper_streaming_cpp_optimized -m models/$model $data_dir/$sample -kc -dtw $dtw -ac -1 --step $step -ct $cpu_threads -gt $gpu_threads -mdr $mdr >> $log_file_no_audio_tag 2>&1
    # echo "Output has been logged to $log_file_no_audio_tag"

    i=$((i+1))
done

if [[ "$OSTYPE" == "darwin"* ]]; then
    system_profiler SPHardwareDataType > ${result_dir}/00-systeminfo_${model_type}_whisper_streaming_cpp_pipeline.txt
fi
# Append the current git commit hash to the system info file
echo "Current Git Commit:" >> ${result_dir}/00-systeminfo_${model_type}_whisper_streaming_cpp_pipeline.txt
git rev-parse HEAD >> ${result_dir}/00-systeminfo_${model_type}_whisper_streaming_cpp_pipeline.txt
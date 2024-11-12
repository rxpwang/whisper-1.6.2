#/usr/bin/env python3
import os
import subprocess
from argparse import ArgumentParser
from tqdm import tqdm

parser = ArgumentParser()
parser.add_argument('--model_type', '-m', type=str, default="medium")
parser.add_argument('--system', '-sys', type=str, default="whisperflow")
parser.add_argument('--dataset', '-d', type=str, default="ted-long-form-test-cut")
parser.add_argument('--data_dir', '-dd', type=str, default="benchmarking_data")
parser.add_argument('--result_dir', '-rd', type=str, default="benchmarking_results")
parser.add_argument('--max-decoding-round', '-mdr', type=float, default=0.5)
parser.add_argument('--step', '-s', type=int, default=100)
parser.add_argument('--thread', '-t', type=int, default=-1)
parser.add_argument('--cpu-thread', '-ct', type=int, default=5)
parser.add_argument('--gpu-thread', '-gt', type=int, default=5)

if __name__ == '__main__':

    args = parser.parse_args()
    print(f"Evaluating system {args.system} with model {args.model_type} on dataset {args.dataset}")
 
    if args.thread == -1:
        print(f"Using {args.cpu_thread} CPU threads and {args.gpu_thread} GPU threads")
    else:
        print(f"Using {args.thread} threads")
    
    # make the folder to save the results
    result_dir = os.path.join(args.result_dir, args.system, args.model_type, args.dataset)
    if not os.path.exists(result_dir):
        os.makedirs(result_dir)
    
    # run experiments
    data_dir = os.path.join(args.data_dir, args.dataset)
    for path in tqdm(os.listdir(data_dir)):
        if path.endswith(".wav"):
            data_path = os.path.join(data_dir, path)
            result_path = os.path.join(result_dir, f"{os.path.splitext(os.path.basename(path))[0]}.log")
            if args.system == "whisperflow":
                program = "./whisper_streaming_cpp_optimized"
                arg_list = [
                    # ./whisper_streaming_cpp_optimized -m models/$model $data_dir/$sample -kc -dtw $dtw -ac -1 -at audio_tag/${model_type}_0.5s_avg.csv --step $step -ct $cpu_threads -gt $gpu_threads -mdr $mdr >> $log_file 2>&1
                    program,
                    "-m", f"models/ggml-{args.model_type}.bin",
                    data_path,
                    "-kc",
                    "-dtw", args.model_type,
                    "-ac", "-1",
                    "-at", f"audio_tag/{args.model_type}_0.5s_avg.csv",
                    "--step", f"{args.step}",
                    "-ct", f"{args.cpu_thread}",
                    "-gt", f"{args.gpu_thread}",
                    "-mdr", f"{args.max_decoding_round}",
                    ">>", result_path,
                    "2>&1",
                ]
            elif args.system == "baseline":
                program = "./whisper_streaming_cpp"
                arg_list = [
                    # ./whisper_streaming_cpp_optimized -m models/$model $data_dir/$sample -kc -dtw $dtw -ac -1 -at audio_tag/${model_type}_0.5s_avg.csv --step $step -ct $cpu_threads -gt $gpu_threads -mdr $mdr >> $log_file 2>&1
                    program,
                    "-m", f"models/ggml-{args.model_type}.bin",
                    data_path,
                    "-kc",
                    "-dtw", args.model_type,
                    "--step", f"{args.step}",
                    "-t", f"{args.thread}",
                    ">>", result_path,
                    "2>&1",
                ]
                
            for i in range(3):
                subprocess.run(" ".join(arg_list), shell=True)
                with open(result_path, "r", errors="ignore") as f:
                    if "ggml_assert" in f.read().lower():
                        print(f"Error in {result_path}, retrying, attempt {i+1}")
                    else:
                        break
#/usr/bin/env python3
import os
import subprocess
from argparse import ArgumentParser
from tqdm import tqdm

parser = ArgumentParser()
parser.add_argument('--dataset', '-d', type=str, default="ted-long-form-test-cut")
parser.add_argument('--system', '-sys', type=str, default="serialized")
parser.add_argument('--data_dir', '-dd', type=str, default="benchmarking_data")
parser.add_argument('--result_dir', '-rd', type=str, default="benchmarking_results")
parser.add_argument('--max-decoding-round', '-mdr', type=float, default=0.5)
parser.add_argument('--step', '-s', type=int, default=100)
parser.add_argument('--thread', '-t', type=int, default=5)


if __name__ == "__main__":
    # for ablation, we need
    # 1. serialized with tag and beam pruning
    # 2. serialized with tag but without beam pruning
    exp_name = "ablation"
    args = parser.parse_args()
 
    print(f"Using {args.thread} threads")

    data_dir = os.path.join(args.data_dir, args.dataset)
    # run experiments
    for model_type in ["medium", "small", "base"]:
        # make the folder to save the results
        print(f"Ablation study for model type: {model_type}")
        result_dir = os.path.join(args.result_dir, exp_name, args.system, model_type, args.dataset)
        if not os.path.exists(result_dir):
            os.makedirs(result_dir)
        for path in tqdm(os.listdir(data_dir)):
            if path.endswith(".wav"):
                data_path = os.path.join(data_dir, path)
                result_path = os.path.join(result_dir, f"{os.path.splitext(os.path.basename(path))[0]}.log")
                # check if the result file already exists
                try:
                    with open(result_path, "r", errors="ignore") as f:
                        if "Average latency:" in f.read():
                            print(f"Existing result for {result_path}, skipping this sample")
                            continue
                except FileNotFoundError:
                    print(f"Running {args.system} for {path}")

                if args.system == "serialized":
                    program = "./whisper_streaming_cpp_optimized"
                    arg_list = [
                        # ./whisper_streaming_cpp_optimized -m models/$model $data_dir/$sample -kc -dtw $dtw -ac -1 -at audio_tag/${model_type}_0.5s_avg.csv --step $step -ct $cpu_threads -gt $gpu_threads -mdr $mdr >> $log_file 2>&1
                        program,
                        "-m", f"models/ggml-{model_type}.bin",
                        data_path,
                        "-kc",
                        "-dtw", model_type,
                        "-ac", "-1",
                        "-at", f"audio_tag/{model_type}_0.5s_avg.csv",
                        "--step", f"{args.step}",
                        "-t", f"{args.thread}",
                        ">", result_path,
                        "2>&1",
                    ]
                elif args.system == "serialized_no_pruning":
                    program = "./whisper_streaming_cpp_optimized"
                    arg_list = [
                        # ./whisper_streaming_cpp_optimized -m models/$model $data_dir/$sample -kc -dtw $dtw -ac -1 -at audio_tag/${model_type}_0.5s_avg.csv --step $step -ct $cpu_threads -gt $gpu_threads -mdr $mdr >> $log_file 2>&1
                        program,
                        "-m", f"models/ggml-{model_type}.bin",
                        data_path,
                        "-kc",
                        "-dtw", model_type,
                        "-ac", "-1",
                        "-at", f"audio_tag/{model_type}_0.5s_avg.csv",
                        "--step", f"{args.step}",
                        "-t", f"{args.thread}",
                        ">", result_path,
                        "2>&1",
                    ]
                
                i = 0 
                while True:
                    subprocess.run(" ".join(arg_list), shell=True)
                    with open(result_path, "r", errors="ignore") as f:
                        if "Average latency:" not in f.read():
                            print(f"Error in {result_path}, retrying, attempt {i+1}")
                        else:
                            break
                    i += 1

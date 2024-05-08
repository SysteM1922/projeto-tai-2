# AI Generated vs Human Written Text (Group 2)
## Build Project

To build our project you need to download the dataset provided in `dataset/dataset_url.txt`, place the .csv file in `dataset/` folder and run the build.sh script that was created in the root group_2 which will compile the necessary programs for the project, namely was_chatted and benchmark, and generate the necessary files to train and test the model

```bash
./build.sh
```

## Was_Chatted

To run __Was_Chatted__, you must run the following program with the following arguments:

- `-a` to set the value of smoothing alpha
- `-i` flag to indicate whether you want to start the program in interactive mode
- `-d` to indicate whether you want the program to analyze a complete directory and what its path is
- `-f` to indicate whether you want the program to analyze a single file and what its path is
- `-h` to specify the path to the human-written text training file
- `-c` to specify the path to the ChatGPT generated text training file
- `-n` to specify the size of the context we want to consider

### Example of usage

```bash
./bin/was_chatted -a 1 -h dataset/train/train_human.txt -c dataset/train/train_ai.txt -f dataset/test/test_ai_0.txt -n 4
```

## Benchmark

To run __Benchmark__, you must run the following program with the following arguments:

- `-a` to set the value of smoothing alpha
- `-d` to indicate the path to the test directory
- `-h` to specify the path to the human-written text training file
- `-c` to specify the path to the ChatGPT generated text training file
- `-n` to specify the size of the context we want to consider


### Example of usage

```bash
./bin/benchmark -a 1 -h dataset/train/train_human.txt -c dataset/train/train_ai.txt -d dataset/test_tiny/ -n 4
```

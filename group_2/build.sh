if [ ! -f dataset/AI_Human.csv ]; then
    echo "dataset/AI_Human.csv not found. Please download it from https://www.kaggle.com/datasets/shanegerami/ai-vs-human-text/data and place it in the dataset folder."
    exit 1
fi
python3 dataset/dataset.py
c++ -Wall -O3 src/progress_bar.cpp src/table.cpp src/was_chatted.cpp -o bin/was_chatted -lm
c++ -Wall -O3 src/progress_bar.cpp src/table.cpp src/benchmark.cpp -o bin/benchmark -lm
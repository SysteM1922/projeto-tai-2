import csv
import shutil
import os
import random

human = []
ai = []

random.seed(0)

with open('AI_Human.csv', 'r') as file:
    reader = csv.reader(file, delimiter=',')
    reader.__next__()
    c=1
    for row in reader:
        c+=1
        if row[1] == "1.0":
            ai.append(row[0])
        else:
            human.append(row[0])

if os.path.exists('dataset'):
    shutil.rmtree('dataset')
os.makedirs('dataset')

test_size = 0.1

if len(human) < len(ai):
    test_human = random.sample(human, int(len(human)*test_size))
    train_human = list(set(human) - set(test_human))

    human_character_count = sum([len(h) for h in train_human])
    random.shuffle(ai)

    train_ai = []
    ai_character_count = 0
    while ai_character_count < human_character_count:
        train_ai.append(ai.pop())
        ai_character_count += len(train_ai[-1])

    test_ai = random.sample(ai, int(len(train_ai)*test_size))

else:
    test_ai = random.sample(ai, int(len(ai)*test_size))
    train_ai = list(set(ai) - set(test_ai))

    ai_character_count = sum([len(a) for a in train_ai])
    random.shuffle(human)

    train_human = []
    human_character_count = 0
    while human_character_count < ai_character_count:
        train_human.append(human.pop())
        human_character_count += len(train_human[-1])

    test_human = random.sample(human, int(len(train_human)*test_size))

with open('dataset/train_ai.txt', 'w') as file:
    file.writelines(train_ai)

with open('dataset/train_human.txt', 'w') as file:
    file.writelines(train_human)

with open('dataset/test_ai.txt', 'w') as file:
    file.writelines(test_ai)

with open('dataset/test_human.txt', 'w') as file:
    file.writelines(test_human)


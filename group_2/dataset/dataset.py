import csv
import shutil
import os
import random

DATASET_SIZE = 1

human = []
ai = []

random.seed(0)

current_directory = os.path.dirname(__file__)
os.chdir(current_directory)

with open('AI_Human.csv', 'r') as file:
    reader = csv.reader(file, delimiter=',')
    reader.__next__()
    c=1
    for row in reader:
        c+=1
        if row[1] == "1.0":
            ai.append(row[0].replace('\n\n', '\n'))
        else:
            human.append(row[0].replace('\n\n', '\n'))

ai = ai[:int(len(ai)*DATASET_SIZE)]
human = human[:int(len(human)*DATASET_SIZE)]

if os.path.exists('test'):
    shutil.rmtree('test')
os.mkdir('test')

if os.path.exists('train'):
    shutil.rmtree('train')
os.mkdir('train')

if os.path.exists('test_tiny'):
    shutil.rmtree('test_tiny')
os.mkdir('test_tiny')

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

with open('train/train_ai.txt', 'w') as file:
    for line in train_ai:
        file.write(line + '\0')

with open('train/train_human.txt', 'w') as file:
    for line in train_human:
        file.write(line + '\0')

for idx, text in enumerate(test_ai):
    with open('test/test_ai_{}.txt'.format(idx), 'w') as file:
        if idx < 1500:
            with open('test_tiny/test_ai_{}.txt'.format(idx), 'w') as file_tiny:
                file_tiny.write(text)
        file.write(text)

for idx, text in enumerate(test_human):
    with open('test/test_human_{}.txt'.format(idx), 'w') as file:
        if idx < 1500:
            with open('test_tiny/test_human_{}.txt'.format(idx), 'w') as file_tiny:
                file_tiny.write(text)
        file.write(text)


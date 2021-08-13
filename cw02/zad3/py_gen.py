from random import randrange


def generate_number():
    return randrange(1, 1000000)       


file = open("dane.txt", "w")
for i in range(1000000):
    file.write(str(generate_number()) + '\n')

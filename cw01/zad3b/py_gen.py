def gen(name, line_length, number_of_lines, char):
    file_new = open(name, "w+")
    for i in range(number_of_lines):
        string = char * line_length
        string = string + "\n"
        file_new.write(string)
    file_new.close()




for i in range(2):
    filename = "g" + str(i) + ".txt"
    gen(filename, 100, 1000000, chr(65 + i % (ord('Z') - ord('A'))))


for i in range(100):
    filename = "h" + str(i) + ".txt"
    gen(filename, 100, 1000, chr(65 + i % (ord('Z') - ord('A'))))

for i in range(500):
    filename = "i" + str(i) + ".txt"
    gen(filename, 100, 50, chr(65 + i % (ord('Z') - ord('A'))))

#
# nf = open("man.txt", "w+")
#
# mann = "g0.txt"
#
# for i in range(1,4):
#     if i % 2 == 0:
#         mann = mann + " g" + str(i) + ".txt"
#     else:
#         mann = mann + ":g" + str(i) + ".txt"
# nf.write(mann + "\n")
#
# mann = "h0.txt"
#
# for i in range(1,50):
#     if i % 2 == 0:
#         mann = mann + " h" + str(i) + ".txt"
#     else:
#         mann = mann + ":h" + str(i) + ".txt"
# nf.write(mann + "\n")
#
# mann = "i0.txt"
#
# for i in range(1,500):
#     if i % 2 == 0:
#         mann = mann + " i" + str(i) + ".txt"
#     else:
#         mann = mann + ":i" + str(i) + ".txt"
# nf.write(mann + "\n")


# nf.close()



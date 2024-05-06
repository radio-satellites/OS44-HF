"""

CLI app for generating OS-44 config commands

"""

i = input("""Command options:
a) Change TX sentence
b) Change default TX sentence
c) Change chirp duration
d) Change default chirp duration
e) Remove chirp

>>>""")

if i == "a":
    sentence = input("New sentence: ")
    print("CMD code is:")
    print("SETTX "+sentence)
elif i == "b":
    sentence = input("New default sentence: ")
    print("CMD code is:")
    print("SETDEFTX "+sentence)


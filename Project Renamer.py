#!/usr/bin/env python
import os
import glob

print("Make sure you did NOT change any of the file/folder names.")
keyword = input("\nInput Old Project Name: ")
print("\nKeyword this script will look for and change is: " + keyword)
name = input("\nInput New Project Name: ")
folderToLook = [ "*", name + "/*", name + "/" + name + "/*" ]
filesToRead = [ keyword + ".sln",
                keyword + ".rc",
                keyword + ".vcxproj",
                keyword + ".vcxproj.filters",
                keyword + ".vcxproj.user" ]

for folder in folderToLook:
    for file in glob.glob(folder):
        if os.path.basename(file) in filesToRead:
            f = open(file, 'r')
            text = f.read()
            text = text.replace(keyword, name)
            f.close()
            f = open(file, 'w')
            f.write(text)
            f.close()
        if keyword in os.path.basename(file):
            os.rename(file, file.replace(keyword, name))

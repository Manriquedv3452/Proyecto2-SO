import os
commitMessage = "git commit -m " + str(input("Commit Message: "))

os.system('git config --global user.name "randroxcr"')
os.system('git config --global user.email "randymoralesg@gmail.com"')
os.system('git add .')
os.system(commitMessage)
os.system('git push origin master')
print("Done!")
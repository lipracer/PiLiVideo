import os
all_files = os.listdir("./")

def fun(name):
    if -1 != name.find(".lib"):
        return True
    else:
        return False



for i in filter(fun, all_files):
    print(i)

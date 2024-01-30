from git import Repo, exc
import os

MAX_DIR = 4

#Recurse upwards to find repo root
repo = None
path = '.'
for i in range(MAX_DIR):
    try:
        repo = Repo(path)
        break
    except exc.InvalidGitRepositoryError:
        path = os.path.join(path, '..')

head = repo.head.commit
ver = str(head)
dStr = ""
if repo.is_dirty():
    dStr = "-dirty"
    
verStr = ver+dStr
print(verStr)
with open("./gitVersion.h", 'w') as fob:
    fob.write("#define GIT_VERSION \"%s\"\n"%verStr)
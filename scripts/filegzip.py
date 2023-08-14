import gzip
import sys
import os
import shutil
from pathlib import Path

def gz(filepath):
    raw_filename = filepath
    gz_filename=os.path.join(Path(filepath).parent,Path(filepath).name + ".tgz")
    with open(raw_filename, 'rb') as f_in:
        with gzip.open(gz_filename, 'wb') as f_out:
            shutil.copyfileobj(f_in, f_out)
    return gz_filename

if __name__=="__main__":
    if len(sys.argv) !=2:
        print("gzip.py [file path]")
        exit()
    gz(sys.argv[1])


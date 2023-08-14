import os
import filegzip
import filttoarray
path = os.path.join(os.getcwd(),"main/web")
result=os.listdir(path)
for file in result:
 if file.endswith(".html"):
    gzfile = filegzip.gz(os.path.join(path,file))
    filttoarray.file_array(gzfile,file.split(".")[0])
    os.remove(gzfile)

import os
import sys
import time
from pathlib import Path
def hex2str(filepath):
    target =""
    fileinfo = os.stat(filepath)
    line = fileinfo.st_size / 16
    f = open(filepath, "rb")
    for l in range(int(line)):
        data = f.read(16)
        he = ""
        for da in data:
            he = he + hex(da) + ","
        target = target + he + "\n"
    f.close()
    return target

def save2header(path_parent,filename,dat):
    path=os.path.join(path_parent,filename + ".h")
    print("write to: ",path)
    file = open(path,"w")
    file.write(dat)
    file.close()

def file_array(filepath,outfile):
    path_str = Path(filepath)
    if len(outfile) == 0:
        filename = path_str.name.split(".")[0]
    else:
        filename = outfile
    if os.path.exists(filepath) is False:
        print("file not found!")
        exit()
    filename_upper = filename.upper()
    header = "#ifndef __"+filename_upper + "_H__\n"
    header = header+ "#define __" + filename_upper +"_H__\n\n"
    header = header+ "const uint8_t " +"__"+ filename+"__" +"[] = {\n"
    header = header+ hex2str(filepath) +"};" + "\n#endif\n" 
    save2header(path_str.parent,filename,header)

if __name__=="__main__":
    if len(sys.argv) !=2:
        print("filetoarray [file path]")
        exit()
    filepath = sys.argv[1]
    path_str = Path(filepath)
    filename = path_str.name.split(".")[0]
    if os.path.exists(filepath) is False:
        print("file not found!")
        exit()
    start_time = time.time()
    filename_upper = filename.upper()
    header = "#ifndef __"+filename_upper + "_H__\n"
    header = header+ "#define __" + filename_upper +"_H__\n\n"
    header = header+ "const uint8_t " +"__"+ filename+"__" +"[] = {\n"
    header = header+ hex2str(filepath) +"};" + "\n#endif\n" 
    save2header(path_str.parent,filename,header)
    end_time = time.time()
    print("Use time:",end="")
    print(end_time-start_time)

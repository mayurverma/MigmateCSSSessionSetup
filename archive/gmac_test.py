##!/bin/bash
#set -e
from __future__ import print_function   #for making changes in inbuild print function like: print('.', end='') #print the line ending without newline character
import os, time, argparse, subprocess
import signal, fileinput, sys, platform
print(sys.version_info)
if int(sys.version_info[0]) < 3:
    raise Exception("Current Python version is {}, Must use Python 3 and above".format(sys.version_info[0-1]))

#from termcolor import cprint

ANSI_COLOR_RESET = "\x1b[0m"

#iv = "ae15b69f13926ff9b6431957"
#fc = 15
#key = "76af73dcff80f3b52d53620cfd4015eb"
#file = "gmac_capture0109.tab"
#mc_row = 3096
#embd = 0
script = "./mac_rx"

parser = argparse.ArgumentParser(description='Running mac_rx utility')
parser.add_argument('-l', '--loop', default=15, type=int, help='loop to run for that number of iterations')
parser.add_argument('-i', '--iv', required=True, help='session iv')
parser.add_argument('-k', '--key', required=True, help='session key')
parser.add_argument('-f', '--tab_file_name', nargs='+', required=True, help='tab separated filename')
parser.add_argument('-s', '--mac_row', type=int, default=3096, help='mac row number')
parser.add_argument('-e', '--embedded_data', default=4, type=int, help='embedded rows')
parser.add_argument('-fc', '--frame_count', default=0, help='frame count')
parser.add_argument('-x', '--colroi', help='column roi')
parser.add_argument('-y', '--rowroi', help='row roi')
parser.add_argument('-d', '--dataformat',help='supported data format [bits]: 8, 12, 16, 20, 24 (default: NULL)')
parser.add_argument('-m', '--packmode', default=0, help='packing/padding MAC mode: 0|1 (default:0)')
parser.add_argument('-r', '--paddingreg', default="0x42", help='0x value of padding register (LSB byte is relevant) eg: 0x55 (default: NULL)')
parser.add_argument('-p', '--compand', default=14, help='companded output data-format (if different from raw data-format)')
parser.add_argument('-sc', '--script', default=0, help='Script to run like mac_rx_mayur utility, set this value to 1')
parser.add_argument('-L', '--livedata', default=1, help='For Image with Live data & embedded data, set flag to 1(default) else set it to 0(for test pattern, embedded data (\'-E\') need to set either 0 or 1)')
parser.add_argument('-E', '--embddata', default=1, help='For Image with embedded data aplicable only for test pattern image (set \'-L\' to 0), set flag to 1(default) else set it to 0 (w/o embedded data)')
parser.add_argument('-v', '--verbose', default=1, help='For Debug prints')
parser.add_argument('-s2t', '--space2tab', nargs="?", default=1, help='set it to 1 for Space 2 Tab conversion')
parser.add_argument('-inhex', '--inhexval', nargs="?", default=0, help='set it to 1 for Hex to integer conversion')

#parser.add_argument('-c', '--cmac', help='to run for CMAC')

args = parser.parse_args()

iv = str(args.iv)
fc = int(args.frame_count)
key = str(args.key)
d = int(args.dataformat)
v = ""
if int(args.verbose):
    v = " -v "
p = int(args.compand)
paddingreg = str(args.paddingreg)
packmode = int(args.packmode)


list = False
#print("total tab files: ", len(args.tab_file_name))
#if len(args.tab_file_name[1]) > 1:
#   list = True
#else:
#   file = str(args.tab_file_name)
mc_row = str(args.mac_row)
embd = str(args.embedded_data)
cmac = False
if args.rowroi or args.colroi:
    print("CMAC enabled")
    rowRoi = str(args.rowroi)
    colRoi = str(args.colroi)
    cmac = True

if int(embd) == 0:
    embd = "0 " + "-g "   #for 0 embedded lines ignoring ignore Frame count addition to iv in mac_rx with option -g
    
EL_args = ""
if int(args.script):
    script = "./mac_rx_mayur"
    E = int(args.embddata)
    L = int(args.livedata)
    EL_args = " -E "+str(E)+" -L "+str(L)

#hexiv2 = "0"
#cmd = "bsub -Is -q urgent ./mac_rx -d 16 -m 0 -p 14 -r paddingreg -e "+ embd +" -k "+ key +" -i "+ hexiv2 +" -f " + file +" -s " + mc_row +" -v"
#print (cmd)

for item in args.tab_file_name:
    file = str(item)
    filepath_out = filepath_in = file
    if int(args.space2tab):
        line_w_tabs = ""
        filepath_out = os.path.splitext(os.path.basename(file))[0]+"_Tab.txt"
        print("out file name: ", filepath_out)
        with open(filepath_in, 'r') as file_in, open(filepath_out, 'w') as file_out:
            for line in file_in:
                data = line.split()  # splits the content removing spaces and final newline
                if int(args.inhexval):
                    for each in data:
                        line_w_tabs +=  str(int(each, 16)) +"\t"
                    line_w_tabs += '\n'
                else:
                    line_w_tabs = "\t".join(data) + '\n'
                file_out.write(line_w_tabs)
                line_w_tabs = ""
    '''
    with open("outfile.txt", "w") as out:
        with open(file) as f:
            for line_no, line in enumerate(f):
                if (line_no==3):
                    out.write(line.rsplit("\t", 8)[0])
                    #out.write(line[0])
                else:
                    out.write(line)
    '''
    print("mac row at ", int(mc_row))
    flag = 0
    fileHandle = open (filepath_out,"r" )
    lineList = fileHandle.readlines()
    fileHandle.close()
    print(str(len(lineList)) +"; "+ str(len(lineList[-1])))
    if (len(lineList[-1]) > 500):
        flag = 1
    with fileinput.input(filepath_out, inplace=True, backup='.bak', mode = 'rU') as f:
        for line in f:
            #print("line= ",f.filelineno(), end='')
            #if (fileinput.lineno() == 4):
            #   if line.endswith("\t\r\n"): line = line.replace("\t\r\n", "\r\n")
                #sline = line.split('\t')
                #print("******* :1:{} 2:{} 3:{}".format(sline[len(sline)-1], sline[len(sline)-2], sline[len(sline)-3]))
                #if not "469" in sline[len(sline)-1] and not "469" in sline[len(sline)-2]:
                #   print(line.rsplit("\t", 8)[0], end='\r\n')
                #else:
                #   print(line, end ='')
            if (f.filelineno() == int(mc_row)+2 and flag == 1):
                #print("gline= {} {}".format(f.filelineno(), line+1), end='')
                if not "[CSV_FOOTERS]" in line:
                    #print('[CSV_FOOTERS]\r')
                    #print("[CSV_FOOTERS]\r\n", end='')
                    if line.endswith("\n"):
                        sys.stdout.write(line.replace(line, line+"[CSV_FOOTERS]\r\n"))
                    else:
                        sys.stdout.write(line.replace(line, line+"\n[CSV_FOOTERS]\r\n"))
                    #line = line.replace("\r\n", "[CSV_FOOTERS]\r\n")
                    #flag = 1
            elif line.endswith("\t\n"):
                #print(line.replace("\t\n", "\r\n"), end='')
                sys.stdout.write(line.replace("\t\n", "\r\n"))
            else: #required this else print statement otherwise empty line will be copied in same file if the 'if' statement fails.
                #print(line, end ='') 
                sys.stdout.write(line)
    
    #input()
    fc = int(args.frame_count)
    for index in range(0,args.loop):
        #y=$(echo "obase=16;ibase=16;$x + $index" | bc)     
        hexiv = int(iv,16)+fc
        #print("int(iv): {}, {} ", int(iv,16), fc)
        print("frame count: ", fc)
        print("hexiv: ", hexiv)
        #time.sleep(6)
        #fc=fc+1
        hexiv1 = str(hex(hexiv)) #[2:26]
        hexiv2 = hexiv1[2:26]
        print("hexiv2: ", hexiv2)
        if cmac:
            cmd = str(script)+ " -d "+ str(d) + " -m "+ str(packmode) +" -p " + str(p) +" -r " + paddingreg + " -e "+ embd +" -k "+ key +" -i "+ hexiv2 +" -f " + filepath_out +" -s " + mc_row + " -c " + " -y " + rowRoi + " -x " + colRoi + " " + v + " " + EL_args
            #for 16 to 14 bit "bsub -Is -q urgent ./mac_rx -d 16 -m 0 -p 14 -r paddingreg -e "+ embd +" -k "+ key +" -i "+ hexiv2 +" -f " + filepath_out +" -s " + mc_row + " -c " + " -y " + rowRoi + " -x " + colRoi +" -v"
        else:
            cmd = str(script)+ " -d "+ str(d) +" -m "+ str(packmode) +" -p " + str(p) +" -r " + paddingreg + " -e "+ embd +" -k "+ key +" -i "+ hexiv2 +" -f " + filepath_out +" -s " + mc_row + " " + v + " " + EL_args
        print("cmd = ", cmd)
        #cmd = "bsub -Is -q urgent ./mac_rx -d 16 -m 0 -p 14 -r paddingreg -e 0 -g -k 76af73dcff80f3b52d53620cfd4015eb -i "+ hexiv2 +" -f gmac_capture0109.tab -s 3096 -v"
        #os.system("bsub -Is -q urgent ./mac_rx -d 16 -m 0 -p 14 -r paddingreg -e 0 -g -k 76af73dcff80f3b52d53620cfd4015eb -i "+ hexiv2 +" -f gmac_capture0109.tab -s 3096 -v")
        os.system(cmd)
        # The os.setsid() is passed in the argument preexec_fn so it's run after the fork() and before  exec() to run the shell.
        fc=fc+1
        '''
        process = subprocess.Popen([cmd], stdout=subprocess.PIPE, shell=True, preexec_fn=os.setpgrp) #, stderr=subprocess.STDOUT)
        for line in process.stdout:
            print(line)
            #if "MATCH :-)" in line:
            #   os.killpg(os.getpgid(process.pid), signal.SIGTERM)  # Send the signal to all the process groups
            #   os.remove(file+".bak")
            #   #cprint("-------DONE-------", "black")
            #   exit()
        '''
if os.path.exists(filepath_out+".bak"):
    os.remove(filepath_out+".bak")
    #if "Tab" in filepath_out:
    #    import shutil
    #    shutil.move(filepath_out, os.path.splitext(os.path.basename(file))[0]+"_Tab.txt")
#cprint("-------DONE-------", "black")

        





*****************************************************************************************
*******************Readme For Migmate System CSS Session setup***************************
*****************************************************************************************
Section-1:
Requirements for Windows/Linux System:-
1. Python3.6 and above is required for python script execution and confirm while invoking python in shell command python3.6.x version should get called.
Example:
[ inasdjad on innd02lw078 - dir= /home/inasdjad ]
bash % python
Python 3.6.5 (default, Apr 17 2018, 21:36:03)
[GCC 4.4.7 20120313 (Red Hat 4.4.7-18)] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>>

Note: (On Windows)
a. Python3.6 should be installed at C drive. Ex. C:\Python36
b. 7z zip should be installed for extracting of zip files while running the make command.

2. (On Windows) MinGW & MSYS is needed for Windows machine, and should be installed at C: drive. PATH environment need to be set on windows machine.
Windows Environment PATH for MinGW bin folder need to be set "C:\MinGW\bin"
Windows Environment PATH for msys bin folder need to be set "C:\MinGW\msys\1.0\bin"

MinGW "mingw-get-setup.exe" can be downloaded from https://sourceforge.net/projects/mingw/
Install basic MinGW and MSYS package for GCC libraries & Makefile compilation.


3. OpenSSL libraries are required, prefably 1.1.1 version. 
On Linux:
PATH for SSL library is already configured on LSF server, verify that this path exist on your LSF system "/tools/icad/src/builds/openssl/1.1.1-pre7-dev/lib"


On Windows:
OpenSSL-1.1.1i-dev is required on windows machine.
Precompiled 32bit executables and libraries can be downloaded from https://kb.firedaemon.com/support/solutions/articles/4000121705
Extract the folder and place it in C: drive.
PATH for SSL DLL library should be configured as "C:\openssl-1.1.1i-dev\openssl-1.1\x86\bin" in windows machine.

4. On Windows: For building ar1212 Patch. DS5 is required.
Install use installation package DS500-BN-00022-r5p0-27rel1.zip found in \\gbbr01cfs1\projects\AR0820\contrib\ARM\
Follow default installation instructions, installation should be in
C:\Program Files\DS-5 v5.27.1


Section-2:
Steps to perform on Migmate Windows machine:-
1. Checkout "ar1212_rev2_maintenance" branch under your user directory.
2. Open the command shell and cd to C:\Users\fg64dw\ar1212_rev2_maintenance
3. run setenv.bat to setup Python3.6, MSYS PATH for command shell. Ex. C:\Users\fg64dw\ar1212_rev2_maintenance>setenv.bat
4. run command to unpack ROM: make -r -C src/Rom BUILD_CONFIG=barebones_armcc clean dist_unpack
5. run command to build Patch: make -r -C src/Patch BUILD_CONFIG=barebones_armcc clean dist

File used for creating testcases of GMAC & CMAC session setup is "test_depu_validation.py" which resides at location:
C:\Users\fg64dw\ar1212_rev2_maintenance\test\src\test_depu_validation.py

To skip to get connected with crypto server and directly calling set session, comment the following code in your test cases:
rsa_key_vendor = self.crypto_cm.get_public_key(KeyName.VENDOR_PAIR)
to
#rsa_key_vendor = self.crypto_cm.get_public_key(KeyName.VENDOR_PAIR)

And session.connect: 
session.connect(mode = SessionMode.EPHEMERAL_MODE, ca_rsa_key = rsa_key_vendor)
to
session.connect(mode = SessionMode.EPHEMERAL_MODE)

For displaying all session parameters in command window after successfull command run, edit the function "HostCommandSetVideoAuthROI" in host_command_set_video_auth_roi.py under C:\Users\fg64dw\ar1212_rev2_maintenance\test\scripts:
Add below logger.info command in the file before "params = HostCommandSetVideoAuthROIParams()" call.

self.__logger.info('videoAuthConfigParams: \nvideoAuthMode:{}\npixelPackMode:{}\npixelPackValue:{}\nROI Row:{}_{}_{}, Col:{}_{}_{}'.format(videoAuthMode.value,pixelPackMode.value,hex(pixelPackValue),firstRow,lastRow,rowSkip,firstColumn,lastColumn,columnSkip ))

This will display the additional session parameters in command window after successfull session setup needed for GMAC & CMAC validation like:
HostCommandSetVideoAuthROI [INFO] videoAuthConfigParams:
videoAuthMode:0
pixelPackMode:0
pixelPackValue:0x42
ROI Row:0_0_0, Col:0_0_0


6. Command to call testcase (Python3 is required on Migmate system):
python test\scripts\test_runner.py -o <output_folder> -t test_depu_validation.TestDepuValidation.<test_case_name>  migmate

Example for GMAC session setup:
python test\scripts\test_runner.py -o C:\temp_ar1212_depu -t test_depu_validation.TestDepuValidation.test_depu_session_gmac  migmate

Example for CMAC session setup:
python test\scripts\test_runner.py -o C:\temp_ar1212_depu -t test_depu_validation.TestDepuValidation.test_depu_session_cmac  migmate

7. Please note before running the above session setup command, first reset the sensor on migmate system. then run the session setup command in step3 above. Once the session is setup sucessfully then run the INI file setting to setup and bring up the stream.
Change the sensor output to 4104(cols) & 3104(rows) on migmate tool or as per INI preset configuration.

8. After stream on. Right-click on the image frame and save the image as .txt format. The image will get save with decimal values.
say: GMAC_16-14bit.txt



*****************************************************************************************
*****************Readme For C-Utility Compilation & MAC Validation***********************
*****************************************************************************************
Section-3:
After Installation and PATH configuration of Section-1, follow below steps on Windows or Linux:-
1. Extract all the 3 files Makefile, mac_rx.c & mac_validation.py into one folder.

2. Open the command shell and run "make compile" & "make clean" for compiling mac_rx.c and cleaning existing mac_rx binariy if already present in that folder.

3. After successfull compilation of mac_rx tool. Now Python script mac_validation.py can be run for captured image.

Steps for Python script (mac_validation.py):-
4. Run python script with the image saved in same location. Example:-

For GMAC:
python mac_validation.py -i <iv> -k <key> -f <image_filename>  -s <mac_at_row_no> -e <no_of_embedded_rows> -d <raw_format_bit> -m <pixe_pack_mode> -p <companded_format_bits> -r <pixel_pack_value>

Example for GMAC 16-14bit mode (below information extracted from Migmate after session setup):
image file: g16-14_m0p0.txt
video   key         : e96fddf1507c59d5675b961f44dd4682
video_auth_gmac_iv  : 951c06ad89b753e710ee7bd8
videoAuthMode:0
pixelPackMode:0
pixelPackValue:0x0

command:
python mac_validation.py -i 951c06ad89b753e710ee7bd8  -k e96fddf1507c59d5675b961f44dd4682 -f g16-14_m0p0.txt -s 3102 -e 4 -d 16 -m 0 -p 14 -r 0x0


For CMAC:-
additional parameter are required along with above GMAC command: -x <Row_ROI_values> -y <Col_ROI_values>
if -x & -y is not provided then image will be validated based on GMAC algorithm.

Example for CMAC 20-14bit mode (below information extracted from Migmate after session setup):
image file: c20-14.txt
video   key         : 4ba688c1899f59dbc8c9932d9e8c4a3d
video_auth_gmac_iv  : 44003b3b51959624cb0ccf28
videoAuthMode:1
pixelPackMode:1
pixelPackValue:0x42
ROI Row:22_316_2, Col:72_468_4

command:
python mac_validation.py -i 44003b3b51959624cb0ccf28 -k 4ba688c1899f59dbc8c9932d9e8c4a3d -f c20-14.txt  -s 3106 -e 4 -d 20 -m 1 -p 14 -r 0x42 -x 22_316_2 -y 72_468_4


*****************************************************************************************
*****************************************END*********************************************
*****************************************************************************************
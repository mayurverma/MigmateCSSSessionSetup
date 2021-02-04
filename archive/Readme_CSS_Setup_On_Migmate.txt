Readme For Migmate System CSS Session setup:-

Setting Private & Public key to Crypto server: gbbr01-css-crypt
On New machine of Migmate, setup the public and private SSH key to crypto server using putty key generator:-
1. Follow steps "Generate SSH Keys Using PuTTY" on URL: https://phoenixnap.com/kb/generate-ssh-key-windows-10
or 
https://www.ssh.com/ssh/putty/windows/puttygen
2. Save public key to crypto server as authorized keys and private key to your migmate system.

3. command to test ssh working on crypto server:
(Linux) ssh -o BatchMode=yes inasdjad@gbbr01-css-crypt sudo -u gbasdcss-admin /local/css_crypt/bin/create_debug_entitlement.sh CSS test cm cm 0000000000000000000000000000000000000000000000000000000000000000 5 65536 16 16 16

or 

(windows) ssh inasdjad@gbbr01-css-crypt dir
#############################################################################
For Windows:
#############################################################################
You need to have PuTTY installed 

• Download and install PuTTY 0.71 - \\gbbr01cfs1\ukdc\Firmware\putty-64bit-0.71-installer.msi
• Run PuTTYgen.exe on Windows (is a graphical tool)
• Generate a key (Suggestion that you use a Key comment of user@host e.g. jtm6zj@JTM6ZJ-L1 rather than the default timestamp because
  it is easy to recognize where it came from in your authorized_keys file)
• Do not store your keys in My Documents but rather create the personal folder C:\Users\<userid>\.putty which you will need 
  a DOS prompt to create (Explorer won’t let you create dot folders). Save the file as “id_rsa.ppk” as it is used by the internal scripts.
• Append your PuTTY public key to the existing authorized_keys file (the easiest way is Log into the server, edit the authorized_keys file
  with your favorite editor, and cut-and-paste the public key from the GUI to the authorized_keys file)
• More info in https://www.ssh.com/ssh/putty/windows/puttygen

Quick test (ls command in the server) - should not ask for the password:
plink -batch -ssh -i c:\Users\<userid>\.putty\id_rsa.ppk <userid>@gbbr01-css-crypt ls




Step to perform on migmate system:-
checkout AR1212 maintenance branch. 
cd to C:\Users\fg64dw\ar1212_rev2_maintenance

File for creating or editing GMAC & CMAC session setup:-
C:\Users\fg64dw\ar1212_rev2_maintenance\test\src\test_depu_validation.py


python test\scripts\test_runner.py -o C:\temo_ar1212_depu -t test_depu_validation.TestDepuValidation.test_depu_starting_session migmate


Command to call testcase for GMAC session setup:-
python test\scripts\test_runner.py -o C:\temo_ar1212_depu -t test_depu_validation.TestDepuValidation.test_depu_session_gmac  migmate


New TestCase created:-
test_depu_session_cmac_20bit_pack42  ==> for 20bit mode and pack 0x42
test_depu_session_cmac_20bit_pack0 ==> pack mode 0x00
test_depu_session_gmac_pack42 ==> pack mode 0x42
test_depu_session_gmac_pack0 ==> pack mode 0x00

First reset the sensor on migmate system. then run the session setup on command prompt. Once the session is setup sucessfully then run the INI file setting to setup and bring up the stream.
Change the sensor output to 4104(cols) & 3104(rows) on migmate tool.


To Copy files from local to remote location, use Robocopy:-
robocopy "C:\Users\fg64dw\Images\17sep" "\\innd02cifs1\AR1212_FE\ashutosh\Mac_Validation\mayur" /Z g16_14.txt



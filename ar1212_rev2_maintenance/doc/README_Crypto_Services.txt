#############################################################################
For AR0820 there is a CSS server to provide some crypto operations, for example:
- encrypting assets
- creating debug entitlement
The clients (i.e FW team members) use SSH key authentication to access the server without a passphrase.
#############################################################################
This guide helps to setup SSH keys (without passphrase) for your Linux and Windows accounts.
#############################################################################

#############################################################################
For Linux
#############################################################################
Please pay particular attention to the Linux file/directory permissions to secure your keys.
If you don’t have an RSA ID key pair (~/.ssh/id_rsa and ~/.ssh/id_rsa.pub) then do the following:
• ssh-keygen
• chmod 700 ~/.ssh
• chmod 600 ~/.ssh/id_rsa
• chmod 644 ~/.ssh/id_rsa.pub

EITHER create the ~/.ssh/authorzed_keys file
• cp ~/.ssh/id_rsa.pub  ~/.ssh/authorized_keys
• chmod 600 ~/.ssh/authorized_keys

OR append your key to your existing ~/.ssh/authorized_keys file
• cat  ~/.ssh/id_rsa.pub >> ~/.ssh/authorized_keys

Quick test (ls command in the server) - should not ask for the password:
ssh gbbr01-css-crypt ls

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
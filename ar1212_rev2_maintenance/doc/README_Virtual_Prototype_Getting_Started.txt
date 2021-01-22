#############################################################################
# Assumption is that you are in an Xterminal after make a connection using your
# preferred X session application i.e. VNC, eXceed etc.
# MobaXterm works fine too.
# Dont run on the login server, use bs bash to start a new session
#############################################################################


#############################################################################
# Initial unix environment setup (can only be done from Unix; windows only has RO access) # Make a workarea in /proj
cd /proj/AR0820/workareas/
mkdir <username>
cd <username>
# Setup link to support environment configuration (one time operation)
ln -s /proj/AR0820/REV/.cde_setup .
# Initialise environment
init_cde -silent

############################################################################
# Install the Virtual Prototype model in /proj/AR0820/workareas/<username>/model_build
# Keep updating it as Martin Abrahams releases new versions.
#
# This is held in svn so check it out and then use svn update command to sync with newer releases.
#
cd /proj/AR0820/workareas/${USER}
init_cde -silent
svn co  http://sjca-svn01.aptina.com/svn/MIG/SE/SOC_FW_sandbox/virtual_platform_support/virtual_platforms/trunk/ar0820/crypto_sys/model_build model_build

############################################################################
# Run this every time before you start DS5 Eclipse to debug  source code.
#

cd /proj/AR0820/workareas/${USER}
source model_build/vp_setup.sh
# At this stage you will be asked to create ar0820_vp_setup.sh in your home directory and define CADI_TARGET_MACHINE
# see README.tx in /proj/AR0820/workareas/${USER}/model_build you should define this as
# export CADI_TARGET_MACHINE=127.0.0.1:4<4 digit unique number>
# NOTE: Unique number could be last four digits of employee number so there are no clashes
#
ar0820_vp.exe --cadi-server --print-port-number &
# The above last line runs the model in background and wait for connection from Eclipse.
# Remember to kill this process to free license when you stop debugging or before going home.

# Run DS5 Eclipse to debug your code
# Only works with Linux version of Eclipse at the moment
# If you run this from a new terminal you must first set up the environment vars
#   using:   source model_build/vp_setup.sh

/devel/edatools/arm/DS5/5.27.1/bin/eclipse &

############################################################################
# First time in Eclipse, you need to import the VP model.
# You only need to do this once.
# Refer the blog: See https://community.arm.com/tools/b/blog/posts/using-arm-ds-5-debugging-with-custom-arm-fast-model-systems
#

The first step in importing a Fast Model system is to create a new DS-5 configuration database. Start from the C/C++ perspective.

New Configuration Database:

File -> New -> Other and then select Configuration Database under DS-5 Configuration Database.
Enter a database name (CryptoCore) and click Finish.


New Model Configuration:
File -> New -> Other and then select Model Configuration under DS-5 Configuration Database.
Select Method for Connection to Model and choose "Browse for model running on local host". ->Next; Choose Browse, wait for a little while and the running model will appear; Select it then ->Finish.
Click Import to import the database.

Create a debug configuration:
From the DS5 Eclipse menu, Run -> Debug configurations. Select DS-5 Debugger and click New button.
In the "Connection" tab, click to expand and select Imported -> CryptoCcore -> Bare Metal Debug -> ARM_Cortext-M0+
In the "Files" tab, Click "File System" to browse your AXF file in a Linux server.
In the "Debugger" tab, click "Debug from entry point". Also tick "Host working directory : Use default".

Then you are ready to debug like you have connected a hardware eval board. You can set breakpoints, single step in your familiar ways.

############################################################################
# Next time in Eclipse, you only need to click "Connect to target" symbol to load and run your program.

Use Eclipse File menu, File -> Open file, to open your file that contains main(). Set a breakpoint there and start source code debugging.

############################################################################
# Note: You need to build your test program in a Linux server, with BUILD_CONFIG=semihosted_armcc  (or cpputest_armcc or barebones_armcc)
# To set the environment (license servers etc) you need to run the following:
cd /proj/AR0820/workareas/${USER}
init_cde -silent

############################################################################
############################################################################
# HOWTO Run a Python Firmware regression test
############################################################################
############################################################################
#### Step 1: Setup environment
cd /proj/AR0820/workareas/${USER}
init_cde -silent

#### Step 2: Checkout or update model the model and setup its parameters
mkdir model_build
svn co  http://sjca-svn01.aptina.com/svn/MIG/SE/SOC_FW_sandbox/virtual_platform_support/virtual_platforms/trunk/ar0820/crypto_sys/model_build model_build
# NOTE: if you already have a checkout you could just update using "svn up model_build"
source model_build/vp_setup.sh

#### Step 3: Checkout FW trunk
mkdir trunk
svn co http://sjca-svn01.aptina.com/svn/MIG/SE/AR0820_firmware/trunk trunk

#### Step 4: Create model output directory (run from here to avoid the snapshot retention for temporal files)
cd /sim/tmp/${USER}
mkdir model
cd model

#### Step 5: Create some path variables to make command lines shorter
DVFWPATH=~/AR0820FW/trunk                               // or wherever your FW is stored. Tip: If you use your home directory (like here) you can access it via windows at \\gbbr01cfs1\<your id>$\AR0820FW\trunk
MODELROOT=/proj/AR0820/workareas/${USER}/model_build    // or wherever your model is stored.

#### Step 6: Build the FW and create symbolic links
make -r -f ${MODELROOT}/Makefile DVFWPATH=${DVFWPATH} MODELROOT=${MODELROOT} link_code
# or if the FW is already built you can just create the symbolic links using NOBUILD
make -r -f ${MODELROOT}/Makefile DVFWPATH=${DVFWPATH} MODELROOT=${MODELROOT} link_code NOBUILD=1

#### Step 6a: Build the correct patch if your test requires one
# Patches are encrypted and you need to be able to use SSH key authentication to access the server without a passphrase.
# If the patch builder demands a password for the encryption, it means you have to set up SSH keys (without passphrase) for your Linux account, once only.
# Setting up your SSH keys from your home directory: Important: Be sure to protect your keys with the chmod commands.
ssh-keygen
chmod 700 ~/.ssh
chmod 600 ~/.ssh/id_rsa
chmod 644 ~/.ssh/id_rsa.pub
# create the ~/.ssh/authorzed_keys file
cp ~/.ssh/id_rsa.pub  ~/.ssh/authorized_keys
chmod 600 ~/.ssh/authorized_keys

# Build the appropriate patch. This example is for Patch, the maintenance patch
make -r -C $DVFWPATH/src/Patch/ BUILD_CONFIG=barebones_armcc clean all

#### Step 7: Edit model parameters, first create a local copy rather than editing the checked-out file in the model. Make will look for this local copy first and use that if it exists
cp /proj/AR0820/workareas/${USER}/model_build/parameters.json .
nedit parameters.json &
i) Set code_file_in0..3 and nvm_file_in parameters to point to your symbolic links created in step 6
      eg      "code_file_in0" : "/sim/tmp/jtmpzj/model/AR0820_NVIC_ROM_16384x32_0.vpm",

ii) Set NVM source file in u_top0.u_nvm_memory0
      eg      "nvm_file_in" : "/sim/tmp/jtmpzj/model/AR0820_OTPM_2048x32.vpm",

iii)  Set the u_top0.u_cyber_icore0.message_level to WARNING to reduce debug output

iv)  Optional: To capture FW DiagPrintf output:   set u_top0.uart_enable to true, set u_top0.out_file to "./uart.log", set u_top0.semihost_enable to true

#### Step 8: Run tests specified by TESTLIST as comma separated list of fully qualified Python method names (If you didn't do step 7, you can expand ${DVFWPATH} inline)
make -f ${DVFWPATH}/test/pyfwtest/model.mk DVFWPATH=${DVFWPATH} MODELROOT=${MODELROOT} TESTLIST=test_get_status.TestGetStatus.test_get_status_cm

Add extras to the command line in the usual way e.g.:
OTPMIMAGE=dm
UVMTIMEOUT=30000000000    // model timeout in ps

#### Step 8: Inspect logs (if necessary)
# The dut.log file contains the model output.
# The pyfwtest.log file contains the DEBUG level Python FW test output (INFO was displayed on the console while running).
# The uart.log file contains the Firmware's DiagPrintf() output if you've set the json params as in step 6.iii above.

#################################################################################################################
# Interactive debug of Python tests with Eclipse
#
# NOTE: the eclipse session, virtual prototype and test execution need all to be on the same machine the steps below
# assume that the basic environment has been setup and used and you are starting again from scratch
#
# Assumption is that you are in an Xterminal after make a connection using your preferred X session application
# i.e. VNC, eXceed etc.
#
#################################################################################################################
#
#### Step 1 - Create terminal session on interactive queue
bsub -Is -q interactive -J fwAR0820 dbus-launch gnome-terminal &
# Note: you can add a job name using -J <job-name> option in the above command

#### Step 2 - Initalize the terminal to setup the correct working environment
cd /proj/AR0820/workareas/${USER}/
init_cde -silent
source model_build/vp_setup.sh

#### Step 3 - Go to model output directory
cd /sim/tmp/${USER}/model/

#### Step 4 - Start eclipse
eclipse &

#### Step 5 To make command lines shorter, create variable pointing to your FW trunk.
# Tip: If you use your home directory (like here) you can access it via windows at \\gbbr01cfs1\<your id>$\AR0820FW\trunk
DVFWPATH=~/AR0820FW/trunk             // or wherever your FW is stored.

#### Step 6 - Run one test specified by TESTLIST as a fully qualified Python method name. (If you didn't do step 5, you can expand ${DVFWPATH} inline)
make -f ${DVFWPATH}/test/pyfwtest/model.mk pyfwdebug TESTLIST=test_get_status.TestGetStatus.test_get_status_dm DVFWPATH=${DVFWPATH} MODELROOT=${MODELROOT}

Add extras to the command line in the usual way e.g.:
OTPMIMAGE=dm
UVMTIMEOUT=30000000000

#### Step 7 - Eclipse configuration
#
# Create a new model_configuration: Open the menu File, option New->other.
#
# In the NEW pop-up window, select Model Configuration. select the DS-5 Configuration Database, press next. In the New Model window,
# select method "Browse for model running on local host", press next. Click the Browse button. You should now see the name and the
# port number of your running VP. Select it. Press finish button.
#
# You will see the System Generator tab open in the centre of the eclipse window. Press the Import button on this window. Close the System Generator tab.
#
# Open the run menu item "Debug Configurations". Create a new debug configuration for the new database and new model configuration that you created in this session.
#
# In the "Files" tab, select the "File System..." button and use the sub-window to select the AXF file for your firmware. Select "load symbols from file".
# NOTE: do not put anything in the "Application on host to download" field. Go to the "Debugger" tab and select either "Debug from entry point" or "Debug from Symbol":"main".
# Press the Apply button, Press the Debug button.
#
# Eclipse will change over to the debug view, and connect to your running VP. From here you can step, set breakpoints, continue, and eventually detach from target.
#
# Note, your running VP will send standard output and standard error messages to the terminal window where the VP is running.

#### Known Restrictions
# We cannot restart the python script, except by terminating the running VP and re-running the make pfyfwdebug command.
#
# When the python script terminates, it kills the running VP.


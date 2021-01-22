#############################################################################
# Assumption is that you are in an Xterminal after make a connection using your
# preferred X session application i.e. VNC, eXceed etc.
#############################################################################


#############################################################################
# Initial unix environment setup (can only be done from Unix; windows only has RO access) # Make a workarea in /proj
cd /proj/AR0820/workareas/
mkdir <username>
cd <username>
# Setup link to support environment configuration (one time operation)
ln -s /proj/AR0820/REV/.cde_setup .

############################################################################
# Initilise workspace
# Initialise environment (whilst in /proj/AR0820/workareas/<username>). Each time you open new session/terminal
init_cde -silent

############################################################################
# Create Software workspace (assuming you are in /proj/AR0820/workareas/<username>/<repo_name>)
# Checkout dv_firmware into <repo_name>
svn co http://sjca-svn01.aptina.com/svn/MIG/SE/AR0820_firmware/branches/dv_firmware  dv_firmware

# Build helloworld  (as example)
cd dv_firmware/test/helloworld
bsub -Is make -r BUILD_CONFIG=barebones_armcc QUIET= clean all readmemh
# Warning: Ensure your code builds. The RTL sim will also build the code but will still continue through to the simvision GUI whether or not it loaded good code objects.

############################################################################
# Create RTL sim workspace
# Make directory which will contain RTL and simulation files (whilst in /proj/AR0820/workareas/<username>)
mkdir aim_platform
mkdir aim_platform/rev2             // this could also be trunk HW variant
cd  aim_platform/rev2
# Setup the environment
svn co http://sjca-svn01.aptina.com/svn/MIG/Sensor/aim_platform/branches/C_AR0820_REV2 .

############################################################################
# To update your environment (whilst in /proj/AR0820/workareas/<username>/aim_platform)
svn up

##########################################################################
# Execute simulation (instructions assume a ‘fresh’ terminal # Initialise environment (whilst in /proj/AR0820/workareas/<username>). Each time you open new session/terminal
# don't try to run a sim on the login server, it will appear to work but not produce any output. Instead, start a new shell first
bs bash
cd /proj/AR0820/workareas/<username>/
init_cde -silent
# Navigate to work area
cd aim_platform/rev2/asic/logic/security/secure-tb/
# Execute HelloWorld (Non-interactive)
make irun TESTNAME=helloworld DVFWPATH=/proj/AR0820/workareas/<username>/<repo_name>/branches/dv_firmware &
# Execute Helloworld (Interactive)
make gui TESTNAME=helloworld DVFWPATH=/proj/AR0820/workareas/<username>/<repo_name>/branches/dv_firmware &

# Optionally define NOBUILD to avoid rebuilding the code e.g.
make gui NOBUILD=1 TESTNAME=helloworld DVFWPATH=/proj/AR0820/workareas/<username>/<repo_name>/branches/dv_firmware &

##########################################################################
# For some basic information regarding operation of RTL simulation (SimVision)
# see Appendix B Basic RTL Sim Operation Notes in 
# http://sjca-svn01.aptina.com/svn/MIG/SE/AR0820_firmware/trunk/doc/AR0820_Software_Units_Integration_Plan.docx

##########################################################################
# Configure and add new test
- Edit /proj/AR0820/workareas/<username>/aim_platform/rev2/asic/logic/security/secure-tb/uvm_tests/on_uvm_secure_test_pkg.svh adding `include "<newtestname>.svh"
- Copy /proj/AR0820/workareas/<username>/aim_platform/rev2/asic/logic/security/secure-tb/uvm_tests/patchtest.svh to /proj/AR0820/workareas/<username>/aim_platform/rev2/asic/logic/security/secure-tb/uvm_tests/<newtestname>.svh
- Edit <newtestname>.svh replacing occurances of “patchtest” with <newtestname>

############################################################################
############################################################################
# HOWTO Run a Python Firmware regression test
############################################################################
############################################################################
#### Step 1: Setup environment
cd /proj/AR0820/workareas/<username>
init_cde -silent

#### Step 2: Checkout Sensor AIM platform and create symbolic links to it. MAKE SURE YOU USE THE CORRECT HW VARIANT, typically trunk or branch Rev2
# Sometimes it's useful to have both variants, in case of needing to easily test against different HW versions
mkdir aim_platform
mkdir aim_platform/trunk
mldir aim_platform/rev2
# create a sybolic link to the HW you want to use
SECTBROOT=/proj/AR0820/workareas/${USER}/aim_platform/trunk/asic/logic/security/secure-tb/     // or use rev2 instead of trunk

svn co http://sjca-svn01.aptina.com/svn/MIG/Sensor/aim_platform/trunk aim_platform/trunk                    // trunk HW checkout
svn co http://sjca-svn01.aptina.com/svn/MIG/Sensor/aim_platform/branches/C_AR0820_REV2 aim_platform/rev2    // Rev2 branch HW checkout

#### Step 3: Checkout FW trunk
mkdir trunk
svn co http://sjca-svn01.aptina.com/svn/MIG/SE/AR0820_firmware/trunk trunk

#### Step 4: Create simulation output directory (run from here to avoid the snapshot retention for temporal files)
# Caution: If you run sims from another terminal window, you must first perform init_cde -silent from /proj/AR0820/workareas/<username> before cd'ing to /sim....
cd /sim/tmp/<username>
mkdir sim
cd sim

#### Step 5: Build  FW and create symbolic links. The default OTPM image used is blank; To specify another LC state use the optional OTPMIMAGE
DVFWPATH=~/AR0820FW/trunk       // or wherever your FW trunk is located. Tip: Use your Linux home directory and you can also access that from windows as \\gbbr01cfs1\<username>$\AR0820FW\trunk (usually S:)

# If you're planning to build and run development code (that you've just edited), then build it and create the ROMs
make -r -C $DVFWPATH/src/Rom/ BUILD_CONFIG=barebones_armcc BUILD_TYPE=release  all readmemh
# you will then have to rebuild any patch that your test(s) requires
make -r -C $DVFWPATH/src/Patch/ BUILD_CONFIG=barebones_armcc clean dist

make -r -f ${SECTBROOT}/Makefile DVFWPATH=${DVFWPATH} SECTBROOT=${SECTBROOT} TESTNAME=pyfwtest link_code
Optionally add    OTPMIMAGE=dm  (or blank or secure_boot. The default is blank )

# or if the FW is already built you can just create the ROM's symbolic links using NOBUILD
make -r -f ${SECTBROOT}/Makefile DVFWPATH=${DVFWPATH} SECTBROOT=${SECTBROOT} TESTNAME=pyfwtest link_code NOBUILD=1

#### Step 6: Run tests specified by TESTLIST as comma separated list of fully qualified Python method names
make -f  ${DVFWPATH}/test/pyfwtest/sim.mk DVFWPATH=${DVFWPATH} SECTBROOT=${SECTBROOT} TESTLIST=test_get_status.TestGetStatus.test_get_status_cm

Add extras in the usual way e.g.:
OTPMIMAGE=dm  (or blank or secure_boot)
UVMTIMEOUT=30000000000    // in picoseconds

#### Step 7: Inspect log (if necessary)
# The dut.log file contains the simulation output.
# The pyfwtest.log file contains the DEBUG level Python FW test output (INFO was displayed on the console while running).
# The uart.log file will the Firmware's DiagPrintf() output if you've built a semihosting build

#### Step 8: Post analysis using Simvision (if necessary)
bsub -Is simvision
File->Open Database security_tb.trn
# Press Open & Dismiss
# Then load your prefered signal file
press the button (top left) "Load Signals from a file" - a useful one to load is in /trunk/scripts/signals.svwf
 
#### Annotating and sharing your SIM
For important moments in time that you wish to save, you can make a copy of the time cursor and rename it - right-click the cursor for options
When you are happy with the look of your sim (signals, annotated markers etc) and wish to share it with someone, in simvision:
File -> Save Command Script.  Tick the 'Save all database names' box and enter a name, like my_sim.svcf
Now, anyone can see the (completed) sim, with all your signals and markers, by navigating to the original sim directory and entering:
Bsub -Is simvision -input my_sim.svcf            note: this views the original sim, not a rerun
 


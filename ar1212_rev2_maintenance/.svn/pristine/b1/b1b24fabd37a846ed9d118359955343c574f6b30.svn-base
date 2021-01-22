####################################################################################
# Copyright 2018 ON Semiconductor.  All rights reserved.
#
# This software and/or documentation is licensed by ON Semiconductor under limited
# terms and conditions. The terms and conditions pertaining to the software and/or
# documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
# ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
# Do not use this software and/or documentation unless you have carefully read and
# you agree to the limited terms and conditions. By using this software and/or
# documentation, you agree to the limited terms and conditions.
#
####################################################################################
# LSF helper classes for reading job information.
####################################################################################
import argparse
import subprocess

from enum import Enum

class LSBJobStatus(Enum):
    DONE  = 0  # Job terminated with 0 status
    EXIT  = 1  # Job terminated with non-zero status
    PEND  = 2  # Job is pending (not started yet)
    PSUSP = 3  # Job suspended while pending
    RUN   = 4  # Job is running
    SSUSP = 5  # Job suspended while running because of system load conditions
    UNKWN = 6  # Slave batch daemon lost contact with master batch daemon
    USUSP = 7  # Job suspended while running
    
class LSBJobHelper:
    """Parser that reads the status of a Linux Server Batch Job."""
    def __init__(self, job, is_name=False):
        self.job = job
        self.is_name = is_name
        # Construct the status query command line
        self.command = ['bjobs', '-a']
        if is_name:
            self.command.append('-J')
        self.command.append(job)
    
    def get_status(self):
        # Execute the status query and capture the output
        result = subprocess.run(self.command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, encoding='utf-8', check=True)
        # Discard first line containing column headers
        text = result.stdout.splitlines()[1]
        # Split the inforamation into fields
        jobid, user, stat, queue, from_host, exec_host, job_name, submit_time = text.split(maxsplit=7)
        # Convert the status string to an enumeration value
        return LSBJobStatus[stat]

        
if __name__ == "__main__":
    # Create the argument parser
    parser = argparse.ArgumentParser(prog='bjob_helper',
        description='Get the status of a Linux Server Batch Job')
    parser.add_argument('-J', '--is_name', action='store_true', help='Find job by name')
    parser.add_argument('job', help='Job number or name')

    # Parse the command line arguments
    args = parser.parse_args()

    # Read the job status and print
    status = LSBJobHelper(args.job, args.is_name).get_status()
    print(status.name)

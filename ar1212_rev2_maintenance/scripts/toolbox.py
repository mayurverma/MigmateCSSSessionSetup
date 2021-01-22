#######################################################################################
# Copyright 2018 ON Semiconductor.  All rights reserved.
#
# This software and/or documentation is licensed by ON Semiconductor under limited
# terms and conditions.  The terms and conditions pertaining to the software and/or
# documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
# ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Softwar").
# Do not use this software and/or documentation unless you have carefully read and
# you agree to the limited terms and conditions.  By using this software and/or
# documentation, you agree to the limited terms and conditions.
########################################################################################
#
# Library for toolbox algorithms used in (AR0820)
#
########################################################################################

class Toolbox():

    def calculate_checksum(self, data):
        sum = 0
        for i in range(0, len(data), 2):
            w = data[i] + (data[i+1] << 8)
            sum += w

        while (sum > 0xffff):
            sum = (sum & 0xffff) + (sum >> 16)

        return (~sum & 0xffff)

if __name__ == "__main__":
    print("Just testing...")




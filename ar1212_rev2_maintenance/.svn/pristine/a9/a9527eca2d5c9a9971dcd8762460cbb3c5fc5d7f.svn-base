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
# Us to extract information from MAP file
#
########################################################################################
import re

class ProcessMap(object):
    "Create LoadPatchChunk command"

    mapfile = None
    file = None

    def __init__(self, mapFile):
        ""
        self.mapfile = mapFile
        with open(mapFile, 'r') as istream:
            self.file = istream.read()

    def patch_base(self):

        patchStartString = re.compile(r'.*Load Region PATCH_CODE_BLOCK \(Base:\s+(0x[a-fA-F0-9]+).*');

        start = self.file.find("Load Region PATCH_CODE_BLOCK")
        substring = self.file[start:start+100]
        return int(float.fromhex(patchStartString.match(substring).group(1)))

    def patch_entry(self):

        patchStartString = re.compile(r'.*Image Entry point :\s+(0x[a-fA-F0-9]+).*');

        start = self.file.find("Image Entry point :")
        substring = self.file[start:start+100]
        return int(float.fromhex(patchStartString.match(substring).group(1)))

    def patch_size(self):

        patchStartString = re.compile(r'.*Execution Region PATCH_DATA_ZI \(Base:\s+(0x[a-fA-F0-9]+),\s+Size:\s+(0x[a-fA-F0-9]+).*');

        start = self.file.find("Execution Region PATCH_DATA_ZI")
        substring = self.file[start:start+100]
        m = patchStartString.match(substring)

        patch_end = int(float.fromhex(m.group(1))) +  int(float.fromhex(m.group(2)))

        return (patch_end - self.patch_base())

    def patch_max_size(self):

        patchStartString = re.compile(r'.*Load Region PATCH_CODE_BLOCK \(Base:\s+(0x[a-fA-F0-9]+),\s+Size:\s+(0x[a-fA-F0-9]+),\s+Max:\s+(0x[a-fA-F0-9]+).*');

        start = self.file.find("Load Region PATCH_CODE_BLOCK")
        substring = self.file[start:start+100]
        m = patchStartString.match(substring)

        patch_max_size = int(float.fromhex(m.group(3)))

        return patch_max_size

if __name__ == "__main__":
    print("Just testing...")

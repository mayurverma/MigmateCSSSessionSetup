#######################################################################################
# Copyright 2019 ON Semiconductor.  All rights reserved.
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
# Public Certificate with OnSemi format
#
########################################################################################
from enum import Enum, unique

@unique
class AssetType(Enum):
    RSA_PUBLIC_CERTIFICATE  = 1
    RSA_PRIVATE_CERTIFICATE = 2
    PSK_MASTER_SECRET       = 3
    TRNG_CHARACTERIZATION   = 4
    OTPM_CONFIGURATION      = 5
    PROVISIONING            = 6

class Header():

    def __init__(self, header):

        self.__id = AssetType(header & 0xffff)
        self.__length = (header >> 16) & 0xffff

    @property
    def id(self):
        return self.__id

    @property
    def length(self):
        return self.__length

class ProcessOTPM():
    """Helper class to extract data from provided image"""

    def __init__(self, otpm_image):

        self.otpm = otpm_image
        self.headers = self.__assets()

        for i in self.headers:
            print(AssetType(Header(self.otpm[i]).id))

    def __assets(self):
        """Walk OTPM image locating asset headers"""

        index = []
        i = 0
        while i < len(self.otpm) and self.otpm[i] != 0:
            header = Header(self.otpm[i])
            index.append(i)
            i += header.length + 1

        return index

    def find_asset(self, asset):
        """Locate specified assets and return index"""

        index = []
        for i in self.headers:
            if asset == Header(self.otpm[i]).id:
                index.append(i)
        return index

    def find_latest_asset(self, asset):
        """Locate latest asset of specified type"""

        index = self.find_asset(asset)

        if len(index) > 0:
            return index[-1]
        else:
            return None

    def extract_asset(self, location):
        """Extract asset from specified location"""

        length = Header(self.otpm[location]).length

        return self.otpm[location:location+length+1]




if __name__ == "__main__":

    user = [\
            0x00010006,0x00009001,0x00040004,0x00001388,0x000003e8,0x000001f4,0x00000000,0x00410002,
            0x00010000,0x41f52a19,0x17d51985,0xd0f0f55c,0x563eadca,0xc4c085fa,0xdc80a49d,0x54e9ee97,
            0x7907df4f,0x784f6a93,0x1c125473,0xe9e12741,0x025a8b55,0x19a91847,0x9af33aa6,0xd01d2205,
            0xf1d19f18,0xbed75326,0x74786543,0x6c3d1ded,0x87738ea0,0x4bfab998,0x01e70c50,0xc709eb31,
            0x9ea9cb50,0x1a704e4e,0x44873460,0x4513bf5a,0xf5c508fd,0x260f9959,0xdc4a2946,0xd2fefd4b,
            0x7bfdc6df,0x90da2a05,0xf0aaa2d7,0x53b51753,0x277fc09e,0x5b5c0b19,0xc2f38e15,0xf6775ee5,
            0x3a71fdc7,0xa4c75b39,0x52631a68,0xbca72cb3,0x131d06b3,0x8ec2bc67,0x8ab0dd94,0xba5ea1d6,
            0xf1867f39,0xc8e58ae8,0x65f1e014,0xc2daf93f,0x047be9ce,0xe7e2b750,0xc83103d7,0x3d2b2e9f,
            0x038eb4eb,0xb43cc00d,0xc43df84d,0x72488bf7,0x3de7ace0,0xb43e8f6f,0x78c1c6c5,0x015aabf1,
            0x31396c3c,0x00820001,0x00424332,0x00000041,0x5fd5009c,0x17a10487,0xd099ce5d,0xc39180f7,
            0xd560d7ec,0x6889aece,0x5778bdb0,0x23dbf694,0x7d4e9d8a,0x377cfed3,0xf0ae38f6,0xf876ccff,
            0x622685c9,0xb28caa6e,0xd64b8ee6,0x9c63fbcc,0xe7665827,0xb6ce05fc,0x5f76b574,0x2cb3e64c,
            0xdbe5b0ad,0xf14de77e,0xe5a47859,0x8d254780,0xd3f5c091,0x783a3281,0x4f75787b,0x5fd7bbc9,
            0x61e16fcc,0xa7da3b0d,0x7386a079,0xf825aa12,0x89f47dc3,0xae4c2d58,0x97859066,0x56e5a30d,
            0x4f948076,0x481b6bec,0x0ff44f09,0x9c6df872,0x75c3f198,0x7430a185,0xf31644fe,0x4dd4541a,
            0xe0088009,0x0ba5c9b4,0xf05821fa,0x2fa56c95,0xf7f67f3c,0x894d7127,0x19e07527,0xe92c5885,
            0x56d79019,0x6061e0b5,0x16940467,0x54dcbde8,0x817b5d6c,0x64760d0a,0x2b5e684c,0xed9ddcef,
            0x219c564a,0x6832d886,0x3d9d99eb,0x9b5bb898,0x160dd34f,0xc688c23c,0x43257c91,0x273d8ad7,
            0xc4df24e4,0x58f9bbd1,0xb3a5bd7c,0xb8e67ebf,0x9a206550,0xa76f0e08,0x9c6a1caf,0x026fc855,
            0x3f912d50,0x87e0a752,0xc5181f4c,0x534f377c,0x6e208061,0x3b47a58f,0xdeb2ec68,0x7bd1c746,
            0xdfbfeaee,0x2ff9313f,0xb3c26285,0x507d860c,0x619323be,0xa401b20e,0xe02b234e,0xf02b06f1,
            0x90ebf50d,0xd74ce4ff,0x93910f16,0x2fb086de,0xfd148ef5,0x67d8430e,0xd18b8ee3,0x13bf172f,
            0xf7e926a4,0x508d662d,0x2670837f,0xee7461ca,0xf29f713e,0x5a137f44,0x8a6e341f,0x3a07fe46,
            0xc8020fda,0xede85c9f,0xe67a6b7f,0x1d9da156,0x756a4e80,0x62203816,0x46be2a70,0xf8e6b3d2,
            0x37d015ac,0x1a059acf,0xe613b4a4,0xa74f6c0d,0xf80ebca1,0x1c3c67fa,0x38aee6ed,0xa5b72d04,
            0x03812959,0x9db51c24,0x033c0117,0x10c8f095,0x00820001,0x00424332,0x00000040,0x30cae9ee,
            0xf147c96a,0xf6a7e879,0x58853ef9,0x153fc5f9,0x47925782,0x28008393,0xab93a5d3,0x1a319833,
            0x23abfcfb,0xb1bc0f6f,0xdea6d4f9,0x4eb0ed96,0x4a61274b,0x45c44634,0x57d50375,0x3db8ccc6,
            0xe60eb659,0xe66395a8,0xc1caf511,0xf3c822b0,0x07774bd4,0x4a84ed59,0xe26ce2bd,0xb1279e9c,
            0x690b8be4,0x4097b9a1,0xd27c4d5e,0x21ad669d,0x4955ca60,0x712d1a8b,0xdd414a74,0xe561d3db,
            0x8a96247f,0x045a49f0,0x2111e11a,0x4befd5b9,0xafeaedb3,0x0a209d9e,0xac27d7a5,0xf7d03c84,
            0x3cf042e4,0x7aed25a3,0xe38c301e,0x0dd55f39,0x9c82c2de,0x14f7d7c7,0x8c1fc1d2,0xa0681489,
            0xc0c7c11c,0x005d8f41,0x4f096a2c,0x79f3171a,0x818a77ce,0x08305b23,0x3688fcf8,0xf525e42d,
            0x20639c8a,0x4347f14e,0xf66c7bf8,0x23ac3d12,0x1fb1f5c7,0xcdfdd631,0x6b4ae493,0x4a6321b7,
            0xcfe5a55b,0xb200ca30,0x03124a0e,0xe7b1f82f,0x9d3a44f6,0x4b34978a,0x14bdcca9,0xcf7e4df9,
            0x90a89c31,0xd5d42fea,0xeca7836e,0x9589aa14,0x47fce4e1,0x9d42386f,0x9729a4b1,0x0bcd6de1,
            0x7a998d10,0x5ce5cfc8,0xadc888f5,0x7813b304,0xcfa7889f,0x3b15977b,0x9e064742,0x113036a4,
            0x16389cd7,0x0aeae5b5,0x41dd21aa,0xed94048e,0xd5b96598,0x5ec33220,0x5a6be57d,0xa61d9972,
            0x0a85dfa2,0x15961b0c,0xebc59e59,0x3b1f40a2,0xafa14ac3,0x5ecd4464,0xf3aa5da8,0xd440c5d6,
            0x9b5dd21d,0x2156b8f6,0x10b42544,0xaa960e34,0xbb2eecbf,0xc231d8aa,0x46d723f0,0x7b83a5d0,
            0x1ce7f2e2,0x224176ff,0x8d46376d,0x39706183,0xcb9647c0,0x0648f91d,0x8915f76f,0xb1239184,
            0x73d8b444,0x29763da3,0x15d7ed5a,0x21966609,0xf08c9e61,0x454e9fc7,0xb8d8ee42,
            0x00010006,0x00009001,
            0x00000000,
            0x00000000,0x00000000,0x00000000,0x00000000,0x00000000]


    process = ProcessOTPM(user)
    a = process.find_asset(AssetType(6))
    print(a)
    assert(len(a)==2),"Expected 2 records of this type"

    a = process.find_latest_asset(AssetType(6))
    print(a)
    assert(a==335),"Expected location to be 335"

    a = process.find_latest_asset(AssetType(5))
    print(a)
    assert(a==None),"No asset of this type"

    location = 0
    a = process.extract_asset(location)
    for i in range(len(a)):
        print("0x{:04x}: 0x{:08x}".format(location+i, a[i]))

    # Invalid location
    location = 2
    a = process.extract_asset(location)
    for i in range(len(a)):
        print("0x{:04x}: 0x{:08x}".format(location+i, a[i]))


    print("Just testing...")


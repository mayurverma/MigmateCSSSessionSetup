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
# Patching support helper class
#
########################################################################################
import os
import sys
import math
from math import ceil
from struct import pack
from crypto_helper import CryptoHelper
from process_map import ProcessMap

class AssetStructure(object):
    " Asset structure plus helper files"
    asset_id = 0xAAAAAAAA
    token = 0xCCCCCCCC
    version = 0xBBBBBBBB
    chunk_length_bytes = 0x00000000
    reserved = [0xDD]*8
    nonce = [0xEE]*12
    chunk = None
    asset_tag = [0xFF]*16

    def __str__(self):
        result = super().__str__()
        result =  "assetId              : 0x{:08x}\n".format(self.asset_id)
        result += "token                : 0x{:08x}\n".format(self.token)
        result += "version              : 0x{:08x}\n".format(self.version)
        result += "chunkLengthBytes     : 0x{:08x} ({:d})\n".format(self.chunk_length_bytes, self.chunk_length_bytes)
        result += "reserved[8]          : [{}]\n".format(', '.join(hex(x) for x in self.reserved))
        result += "nonce[12]            : [{}]\n".format(', '.join(hex(x) for x in self.nonce))
        result += ">>> Chunk            : \n{}".format(image_to_string(self.chunk))
        result += "assetTag[16]         : [{}]\n".format(', '.join(hex(x) for x in self.asset_tag))
        return result

    def image(self):

        image =b""
        image += pack("<L",self.asset_id)
        image += pack("<L",self.token)
        image += pack("<L",self.version)
        image += pack("<L",self.chunk_length_bytes)
        image += pack("B"*8, *self.reserved)
        image += pack("B"*12, *self.nonce)
        image += self.chunk
        image += pack("B"*16, *self.asset_tag)
        return image

class ChunkStructure(object):
    " Chunk structure plus helpr functions"
    rom_version = None
    patch_version = None
    last_chunk = None
    this_chunk = None
    load_address = None
    loader_address = None
    data_length = None
    data = None

    def __str__(self):
        result = super().__str__()
        result =  "romVersion           : 0x{:04x}\n".format(self.rom_version)
        result += "patchVersion         : 0x{:04x}\n".format(self.patch_version)
        result += "lastChunk            : {}\n".format(self.last_chunk)
        result += "thisChunk            : {}\n".format(self.this_chunk)
        result += "loadAddress          : 0x{:08x}\n".format(self.load_address)
        result += "loaderAddress        : 0x{:08x}\n".format(self.loader_address)
        result += "dataLengthWords      : 0x{:08x}\n".format(self.data_length)
        if self.data_length > 1:
            result += "data                 : [ 0x{:02x}, 0x{:02x}, ...]\n".format(self.data[0], self.data[1])
        result += "Complete image : \n"
        result += image_to_string(self.image())
        return result

    def image(self):

        image =b""
        image += pack("<H",self.rom_version)
        image += pack("<H",self.patch_version)
        image += pack("<H",self.last_chunk)
        image += pack("<H",self.this_chunk)
        image += pack("<L",self.load_address)
        image += pack("<L",self.loader_address)
        image += pack("<L",self.data_length)
        image += self.data
        return image

    def asset_id(self):
        ""
        return ((self.rom_version & 0xFFFF) << 16)  + ((self.patch_version & 0xFF) << 8) + (self.this_chunk & 0xFF)

class Command(object):
    "Create LoadPatchChunk command"

    def __init__(self):
        ""
        pass

    def generate(self, asset_id, chunk_bin):
        "Generate asset"

        structure = AssetStructure()
        structure.chunk = chunk_bin
        structure.asset_id =  asset_id

        if (0 != len(structure.chunk)%16):
            structure.chunk = CryptoHelper().pkcs7_padding(structure.chunk)
        structure.chunk_length_bytes = len(structure.chunk)

        return structure


class Chunk(object):
    " Takes input patch image and breaks it up into chunks, creating a list of chunks"

    def __init__(self, rom_version, patch_version, load_address, loader_address, max_chunk_size, data):
        ""
        self.rom_version = rom_version
        self.patch_version = patch_version
        self.load_address = load_address
        self.loader_address = loader_address
        self.max_chunk_size = max_chunk_size
        self.data = data
        #
        image_size_bytes = len(data)
        if 0 != image_size_bytes%2:
            raise Exception("Image size must be a multiple of 2 bytes")

        total_chunks = math.ceil(image_size_bytes/max_chunk_size)
        self.last_chunk = total_chunks - 1

        self.chunks = []
        for i in range(total_chunks):
            self.chunks.append(self._generate_chunk(i))

    def _generate_chunk(self, number):
        "Generate chunk"

        if (number > self.last_chunk):
            raise Exception("Max chunk index is {:d}".format(self.last_chunk))

        data_start = number*self.max_chunk_size
        data = self.data[data_start:data_start+self.max_chunk_size]

        structure = ChunkStructure()
        structure.rom_version = self.rom_version
        structure.patch_version = self.patch_version
        structure.last_chunk = self.last_chunk
        structure.this_chunk = number
        structure.load_address =  self.load_address + data_start
        if (number == self.last_chunk):
            structure.loader_address = self.loader_address
        else:
            structure.loader_address = 0

        structure.data_length = len(data)//2
        structure.data = data
        return structure

def auto_int(x):
    return int(x, 0)

def image_to_string(image, length=16):
    " Formats byte array into string for printing"
    result = ""
    for x in  range(int(len(image)/length)):
        result += "[{}]\n".format(''.join(' 0x{:02x},'.format(x) for x in image[x*length:(x+1)*length]))
    if (0 != len(image)%length):
        result += "[{}]".format(''.join(' 0x{:02x},'.format(x) for x in image[(int(len(image)/length))*length:]))

    return result

def image_to_csv(image):
    " Formats byte array into csv format as a set 0f 16-bit writes"
    length = 2
    address = 0xF100
    result = ""
    # Write to page register, selecting first page of shared memory
    result += "WRITE,0xF00C,0x0000,#Write to page register, selecting first page of shared memory\n"
    for x in  range(int(len(image))//length):
        value = int(image[x*length]) + int((image[(x*length)+1] << 8))
        result += "WRITE,0x{:04X},0x{:04X}\n".format(address,value)
        address += 2
    if (0 != len(image)%length):
        value = image[x*length]
        result += "WRITE,0x{:04X},0x{:04X}\n".format(address,value)

    # Execute LoadPatchChunk command
    result += "WRITE,0xF000,0x8003,#Write to command register to execute LoadPatchChunk command\n"
    return result

def get_chunks(rom_version, patch_version, bin_file, map_file, chunksize = (512-20)):
    '''Note: function used for testing'''

    # Read the image data from the binary input file
    with open(bin_file, 'rb') as istream:
        image = istream.read()

    processed = ProcessMap(map_file)

    # The message buffer is 4kBytes long, this needs to hold a LoadPatchChunk command which consists of
    #
    # Asset header (36 Bytes)
    #     Chunk Header (20 Bytes)
    #     ChunkData (must be a multiple of 16, pad as required)
    # Asset Tag (16 Bytes)
    #
    # The Maximum Chunk Header + ChunkData is 512 as per ARM library restriction. This means the maximum chunksize is 512-20
    #
    chunks = Chunk(rom_version, patch_version, processed.patch_base(), processed.patch_entry(), chunksize, image)

    return chunks

if __name__ == "__main__":
    print("Just testing...")


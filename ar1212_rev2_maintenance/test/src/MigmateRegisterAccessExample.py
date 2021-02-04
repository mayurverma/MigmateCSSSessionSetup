####################################################################################
# Copyright 2018 ON Semiconductor.  All rights reserved.
#
# This software and/or documentation is licensed by ON Semiconductor under limited
# terms and conditions.  The terms and conditions pertaining to the software and/or
# documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
# ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
# Do not use this software and/or documentation unless you have carefully read and
# you agree to the limited terms and conditions.  By using this software and/or
# documentation, you agree to the limited terms and conditions.
#
####################################################################################
import logging
import os
import sys
import struct

# work out the project and working copy root directories
__projroot = os.path.dirname(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))

# prepend library paths to the search path
sys.path.append(os.path.join(__projroot, 'contrib', 'pyfwtest'))
sys.path.append(os.path.join(__projroot, 'test', 'scripts'))
sys.path.append(os.path.join(__projroot, 'test', 'src'))

from pyfwtest import Properties

from test_fixture import MigmateTestFixture

def dump_message_buffer(reg, length_in_words = 1, page_number = 0):
    """Read 16-bit wide location(s) in shared RAM"""

    page_size_in_words = 512
    number_of_pages = 4

    # test access range, not allowed to read over a page boundary
    assert (page_number < number_of_pages)
    assert ((length_in_words) <= page_size_in_words)

    reg.CSS_PAGE = page_number
    payload = bytearray()
    payload.extend(reg.CSS_PARAMS_0.burst_read(length_in_words*2))       # 16 bit icb access, so x2 bytes
    return struct.unpack('<' + 'H'*length_in_words, payload)

if __name__ == "__main__":

    # Parse the properties
    properties = Properties({
         'PROJECT_SENSOR_FILE' : os.path.join(__projroot, 'contrib', 'target', '2v0', 'AR0825AT-REV2-DEV.xsdat'),
    })

    # Set root logger level
    logger = logging.getLogger(__name__)
    logger.setLevel(logging.DEBUG)

    # Configure the logger to output info level to the console
    console = logging.StreamHandler(sys.stdout)
    console.setFormatter(logging.Formatter('%(name)s [%(levelname)s] %(message)s'))
    console.setLevel(logging.INFO)
    logger.addHandler(console)

    fixture = MigmateTestFixture(properties)
    camera = fixture.Camera(0)
    reg = camera.sensor.reg

    # Connect fixture to Migmate
    fixture.startTestRun()
    reset_register_stream = reg.RESET_REGISTER.RESET_REGISTER_STREAM.uncached_value
    logger.info("RESET_REGISTER_STREAM = {}".format(reset_register_stream))
    reg.RESET_REGISTER.RESET_REGISTER_STREAM = ~reset_register_stream
    reset_register_stream = reg.RESET_REGISTER.RESET_REGISTER_STREAM.uncached_value
    logger.info("RESET_REGISTER_STREAM = {}".format(reset_register_stream))
    reg.RESET_REGISTER.RESET_REGISTER_STREAM = ~reset_register_stream
    reset_register_stream = reg.RESET_REGISTER.RESET_REGISTER_STREAM.uncached_value
    logger.info("RESET_REGISTER_STREAM = {}".format(reset_register_stream))

    # Read out firxt 5 location of shared RAM (Message buffer)
    logger.info("Message buffer : = {}".format(dump_message_buffer(reg,5)))

    # Close snd remove streamhandler
    console.close()
    logger.removeHandler(console)



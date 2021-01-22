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
# OTPM Configuration Class
#
########################################################################################

class OtpmConfig():

    def __init__(self,
                     tvpps_ns=        10000,   # Charge Pump Warm-up time in nSec
                     tpas_ns=         1,       # Program Address Setup time in nSec
                     tpp_ns=          200000,  # Program pulse width in nSec
                     tpr_ns=          1,       # Program recovery time in nSec
                     tspp_ns=         600000,  # Soak pulse width in nSec
                     tpsr_ns=         300000,  # Power Supply Recovery time in nSec
                     tds_ns=          1,       # Data setup time in nSec
                     twr_ns=          2,       # Write Recovery time in nSec
                     twp_ns=          5,       # Write Pulse width in nSec
                     tras_ns=         4,       # Read address setup time in nSec
                     trr_ns=          17,      # Read recovery time in nSec
                     soak_count_max=  15,      # Maximum soak counter (retry)
                     mr_pgm=          0x1024,  # MR value for OTPM programming
                     mr_pgm_soak=     0x08a4,  # MR value for OTPM program soak
                     mr_read=         0x0000,  # MR value for OTPM read
                     mra_pgm=         0x1200,  # MRA value for OTPM programming
                     mra_prg_soak=    0x1200,  # MRA value for OTPM program soak
                     mra_read=        0x0210,  # MRA value for OTPM read
                     mrb_pgm=         0x103e,  # MRB value for OTPM programming
                     mrb_prg_soak=    0x2034,  # MRB value for OTPM program soak
                     mrb_read=        0x2000,  # MRB value for OTPM read
                 ):


        self.tvpps_ns = tvpps_ns;
        self.tpas_ns = tpas_ns;
        self.tpp_ns = tpp_ns;
        self.tpr_ns = tpr_ns;
        self.tspp_ns = tspp_ns;
        self.tpsr_ns = tpsr_ns;
        self.tds_ns = tds_ns;
        self.twr_ns = twr_ns;
        self.twp_ns = twp_ns;
        self.tras_ns = tras_ns;
        self.trr_ns = trr_ns;
        self.soak_count_max = soak_count_max;
        self.mr_pgm = mr_pgm;
        self.mr_pgm_soak = mr_pgm_soak;
        self.mr_read = mr_read;
        self.mra_pgm = mra_pgm;
        self.mra_prg_soak = mra_prg_soak;
        self.mra_read = mra_read;
        self.mrb_pgm = mrb_pgm;
        self.mrb_prg_soak = mrb_prg_soak;
        self.mrb_read = mrb_read;


    def __bytes__(self):
        result = bytes(0)

        result += self.tvpps_ns.to_bytes(4, byteorder='little')
        result += self.tpas_ns.to_bytes(4, byteorder='little')
        result += self.tpp_ns.to_bytes(4, byteorder='little')
        result += self.tpr_ns.to_bytes(4, byteorder='little')
        result += self.tspp_ns.to_bytes(4, byteorder='little')
        result += self.tpsr_ns.to_bytes(4, byteorder='little')
        result += self.tds_ns.to_bytes(2, byteorder='little')
        result += self.twr_ns.to_bytes(2, byteorder='little')
        result += self.twp_ns.to_bytes(2, byteorder='little')
        result += self.tras_ns.to_bytes(2, byteorder='little')
        result += self.trr_ns.to_bytes(2, byteorder='little')
        result += self.soak_count_max.to_bytes(2, byteorder='little')
        result += self.mr_pgm.to_bytes(2, byteorder='little')
        result += self.mr_pgm_soak.to_bytes(2, byteorder='little')
        result += self.mr_read.to_bytes(2, byteorder='little')
        result += self.mra_pgm.to_bytes(2, byteorder='little')
        result += self.mra_prg_soak.to_bytes(2, byteorder='little')
        result += self.mra_read.to_bytes(2, byteorder='little')
        result += self.mrb_pgm.to_bytes(2, byteorder='little')
        result += self.mrb_prg_soak.to_bytes(2, byteorder='little')
        result += self.mrb_read.to_bytes(2, byteorder='little')
        result += bytes(2) # padding AR0820FW-271
        return result

    def __str__(self):
        result = bytes(self).hex()
        return result

if __name__ == "__main__":
    print("Just testing...")


Video auth GMAC IV:    MSB->LSB:    5f9159808c047ad9dd455b5e
	Control channel nonce: MSB->LSB:    1e1f9ecc0260e313dac55e29
	Video auth key:        MSB->LSB:    d26b0e896e138e9afc07b78753496c38



export LD_LIBRARY_PATH=/tools/icad/src/builds/openssl/1.1.1-pre7-dev/lib:/custom/tools/it/lsf/9.1.3/9.1/linux2.6-glibc2.3-x86_64/lib


bsub -Is -q urgent ./mac_rx -d 20 -m 0 -p 14 -r 0x42 -e 0 -k 1220224c5334f801781a01253cc6e99e -i ced2df00a2cd896e704cbb71 -f newfile10036.tab -s 3096 -c  -y 2_3044_26 -x 0_936_8 -v




cd /proj/AR1212_FE/ashutosh/Mac_Validation/mayur




bsub -Is -q urgent ./mac_rx -d 20 -m 0 -p 14 -r 0x42 -e 0 -k 1220224c5334f801781a01253cc6e99e -i ced2df00a2cd896e704cbb71 -f newfile10036.tab -s 3096 -c  -y 2_3044_26 -x 0_936_8 -v



python gmac_test.py -k d26b0e896e138e9afc07b78753496c38 -i 5f9159808c047ad9dd455b5e -f gmac_capture0030.tab -s 3102 -e 4 -fc 0 -l 1



make compile




bsub -Is -q urgent ./mac_rx -d 20 -m 0 -p 14 -r 0x42 -e 0 -k d58a8cb13f9083ffce7eab2c2ff416d9 -i 2d331df7b28dca9c6ceaa563 -f newfile10023.tab -s 3096 -c  -y 8_14_2 -x 22_974_8 -v






bsub -Is -q urgent ./mac_rx -d 16 -m 0 -p 16 -r 0x42 -e 0 -g  -k ca47248ac0b6f8372a97ac43508308ed -i ffd2b598feabc9019262d2c2 -f stream_tiny_1exp_uc1_mac16x14.tab -s 76 -v



python gmac_test.py -i 2338d960ab9c6caea0793cf7 -k cb5899d2998865f8a2a1bf1653262ea7 -f gmac_capture0019.tab -s 3102 -e 4 -fc 0 -l 1 -d 16 -m 0 -p 16 -r 0x42 -g





python gmac_test.py -i e0057b2ea3d5ba628bc0f3f2 -k 6b642b1079815052243ebc910ce1ff62 -f GMAC_Live_Embd.txt -s 3102 -e 4 -fc 0 -l 1




key : 
ca47248ac0b6f8372a97ac43508308ed
IV: 
ffd2b598feabc9019262d2be
no embedded data there right now use frame count = 0


bsub -Is -q urgent ./mac_rx -d 16 -m 0 -p 16 -r 0x42 -e 0 -g  -k ca47248ac0b6f8372a97ac43508308ed -i ffd2b598feabc9019262d2c2 -f stream_tiny_1exp_uc1_mac16x14.tab -s 76 -v






python gmac_test.py -i 0d84924af1faaadf1cda66f9  -k 7e378a66ded63c5fcbf114cd70786586 -f frame-06.txt -s 3102 -e 4 -d 16 -m 0 -p 14 -r 0x42 -l 15 -fc -4

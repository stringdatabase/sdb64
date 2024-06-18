bbcmp.py - BootStrap Basic Compiler 

This program's purpose is to boot strap build an sd system without use of the original op code binary.

06/15/2024 - first attempt a install package

    Basic setup:
    No program object in GPL.BP.OUT / BP.OUT / gcat
    No binaries in bin, including pcode file
    No object files in gplobj
    
    In short everything is built at install time
    
    Steps:
     install runs bbcmp.py to create program objects for:
     BCOMP
     BBPROC
     PATHTKN
     install runs pcode_bld.py to build pcode file
     
     Please note source file names are hardcoded into BBPROC and pcode_bld.py
     eg)
      sudo python3 <path to >/gplbld/bbcmp.py /usr/local/sdsys GPL.BP/BBPROC GPL.BP.OUT/BBPROC
      sudo python3 <path to >/gplbld/bbcmp.py /usr/local/sdsys GPL.BP/BCOMP GPL.BP.OUT/BCOMP
      sudo python3 <path to >/gplbld/bbcmp.py /usr/local/sdsys GPL.BP/PATHTKN GPL.BP.OUT/PATHTKN
      sudo python3 <path to >/gplbld/pcode_bld.py
	  
	install runs sd in install mode
	  sudo /usr/local/sdsys/bin/sd -start
	  sudo /usr/local/sdsys/bin/sd -i
	    This step runs the BBPROC command processor, currently requires a <CR> to continue
		
	install runs second compile
	  sudo /usr/local/sdsys/bin/sd -asdsys -internal SECOND.COMPILE   

#!/usr/bin/python
#
# (C) 2001-2012 Marmalade. All Rights Reserved.
#
# This document is protected by copyright, and contains information
# proprietary to Marmalade.
#
# This file consists of source code released by Marmalade under
# the terms of the accompanying End User License Agreement (EULA).
# Please do not use this program/source code before you have read the
# EULA and have agreed to be bound by its terms.
#

from os.path import join, isdir
from os import listdir

# Re-generate the RacingResources-gles2-dxt.dcl file and include shader binaries

file = open("RacingResources-gles2-dxt.dcl", "w")
file.write("""
#!/usr/bin/env dzip
archive ..\\RacingResources-gles2-dxt.dz
basedir ..\\..\\data-ram\\data-gles2-dxt

file car\\gokart\\gokart.group.bin 0 dz
file environment\\track_01\\track_01.group.bin 0 dz
file tracks\\track_01\\track_01.group.bin 0 dz
file ui\\ui.group.bin 0 dz
""")

# Include shader bins for windows phone 8
shader_bin_dir = join("..", "..", "data-ram", "shader_bin_wp8")
if isdir(shader_bin_dir):
    # Include binaries from shader_bin
    for f in listdir(shader_bin_dir):
        file.write("file %s\\%s 0 dz\n" % (shader_bin_dir, f))

file.close()

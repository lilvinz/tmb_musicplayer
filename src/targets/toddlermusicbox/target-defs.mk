
# add board to target lists
BL_BOARDS += toddlermusicbox
FW_BOARDS += toddlermusicbox
EF_BOARDS += toddlermusicbox
FT_BOARDS += toddlermusicbox
SIM_BOARDS += toddlermusicbox

# add dependencies for top level makefile
ef_toddlermusicbox_all: fw_toddlermusicbox_all bl_toddlermusicbox_all
fw_toddlermusicbox_all: bl_toddlermusicbox_all
ft_toddlermusicbox_all: ef_toddlermusicbox_all

fw_toddlermusicbox_program: fw_toddlermusicbox_all
bl_toddlermusicbox_program: bl_toddlermusicbox_all
ef_toddlermusicbox_program: ef_toddlermusicbox_all

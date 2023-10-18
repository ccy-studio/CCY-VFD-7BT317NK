#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

COMPONENT_SRCDIRS += lib/Src fragment/Src
COMPONENT_ADD_INCLUDEDIRS += lib/Inc/ fragment/Inc/


COMPONENT_EMBED_TXTFILES := index.html
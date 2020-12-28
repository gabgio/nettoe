# Makefile.am file for nettoe
# Edited by Gabriele Giorgetti <ggdev@users.sourceforge.net>
# Continued by Mats Erik Andersson

# This file is processed by automake

DEFS = -DNETTOE_PORT=$(NETTOE_PORT) @DEFS@

bin_PROGRAMS    = nettoe

nettoe_SOURCES  = 			\
		ai.c board.c board.h	\
		nettoe.c game.h 	\
		matrices.c matrices.h	\
		terminal.c terminal.h	\
		misc.c misc.h 		\
		network.c network.h

nettoe_LDADD = $(NETTOE_LDADD)

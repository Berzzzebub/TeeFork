# Author: Bohdan Turkynewych, 2009-2010, <tb0hdan@gmail.com>
# Complies with modified BSD license.
# See COPYRIGHT file.
# Thanks very much to: http://www.advancedlinuxprogramming.com/

SUBDIRS		= src

.PHONY:		all clean $(SUBDIRS)

all:		$(SUBDIRS)

$(SUBDIRS):
	cd $@; $(MAKE)

clean:
	for subdir in $(SUBDIRS); do \
	  (cd $${subdir}; $(MAKE) $@); \
	done

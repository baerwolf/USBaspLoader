# Name: Makefile
# Project: USBaspLoader (updater)
# Author: Stephan Bärwolf
# Creation Date: 2012-09-01
# Tabsize: 4
# License: GNU GPL v2 (see License.txt)

include Makefile.inc


all: do_firmware do_updater

firmware: do_firmware
updater: do_updater

do_firmware:
	$(MAKE) -C firmware all

do_updater:
	$(MAKE) -C updater all

deepclean: clean
	$(RM) *~
	$(MAKE) -C updater  deepclean
	$(MAKE) -C firmware deepclean

clean:
	$(MAKE) -C updater  clean
	$(MAKE) -C firmware clean

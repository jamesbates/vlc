# faad2

FAAD2_VERSION := 2.7
FAAD2_URL := $(SF)/faac/faad2-$(FAAD2_VERSION).tar.gz

ifeq ($(findstring $(ARCH),arm),)
# FAAD is a lot slower than lavc on ARM. Skip it.
PKGS += faad2
endif

$(TARBALLS)/faad2-$(FAAD2_VERSION).tar.gz:
	$(call download,$(FAAD2_URL))

.sum-faad2: faad2-$(FAAD2_VERSION).tar.gz

faad2: faad2-$(FAAD2_VERSION).tar.gz .sum-faad2
	$(UNPACK)
ifndef HAVE_FPU
	$(APPLY) $(SRC)/faad2/faad2-fixed.patch
endif
	cd $(UNPACK_DIR) && $(CC) -iquote . -E - </dev/null || sed -i 's/-iquote /-I/' libfaad/Makefile.am
	$(MOVE)

.faad2: faad2
	$(RECONF)
	cd $< && $(HOSTVARS) ./configure --without-drm $(HOSTCONF)
	cd $< && sed -i.orig "s/shrext_cmds/shrext/g" libtool
	cd $</libfaad && $(MAKE) install
	touch $@

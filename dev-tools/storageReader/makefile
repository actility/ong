

SUBDIRS=sqliteReader

.PHONY: subdirs $(SUBDIRS)
.PHONY: all $(foreach subdir,$(SUBDIRS),_suball_$(subdir))
.PHONY: clean $(foreach subdir,$(SUBDIRS),_subclean_$(subdir))

subdirs: $(SUBDIRS)

all: $(foreach subdir,$(SUBDIRS),_suball_$(subdir))

$(foreach subdir,$(SUBDIRS),_suball_$(subdir)):
	@$(MAKE) -C $(subst _suball_,,$@) all

clean: $(foreach subdir,$(SUBDIRS),_subclean_$(subdir))

$(foreach subdir,$(SUBDIRS),_subclean_$(subdir)):
	@$(MAKE) -C $(subst _subclean_,,$@) clean

$(SUBDIRS):
	@$(MAKE) -C $@ $+



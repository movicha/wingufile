SUBDIRS = 16x16 22x22 24x24 32x32 48x48 128x128 scalable

icons = ccnet_daemon_up.png ccnet_daemon_down.png \
	wingufile_transfer_1.png wingufile_transfer_2.png \
	wingufile_transfer_3.png wingufile_transfer_4.png \
	wingufile_auto_sync_disabled.png

pkgdata_DATA = $(icons)

EXTRA_DIST = $(icons)

install-data-hook: update-icon-cache
uninstall-hook: update-icon-cache
update-icon-cache:
	@-if test -z "$(DESTDIR)"; then \
		echo "Updating Gtk icon cache."; \
		gtk-update-icon-cache || true; \
	else \
		echo "*** Icon cache not updated.  After (un)install, run this:" \
		echo "***   gtk-update-icon-cache"; \
	fi

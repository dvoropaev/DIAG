TESTS = tests

diagnostic_tool = domain-client
name = diag-$(diagnostic_tool)
version != sed -n 's/^Version: \(.*\)/\1/p' < $(name).spec

alterator_datadir = $(datadir)/alterator

build: diag-domain-client diag-domain-client.spec
	# set current version in script and man
	sed --in-place "0,/^VERSION=.*/{s//VERSION=$(version)/}" $(name)
	sed --in-place 's/@VERSION@/$(version)/g' $(name).man

install:
	install --preserve-timestamps -D $(name) $(bindir)/$(name)
	install --preserve-timestamps -D $(name).man $(datadir)/man/man1/$(name).1
	install --preserve-timestamps -D -m644 alterator/$(name).backend $(alterator_datadir)/backends/$(name).backend
	install --preserve-timestamps -D -m644 alterator/$(diagnostic_tool).diag $(alterator_datadir)/diagnostictools/$(diagnostic_tool).diag
	install --preserve-timestamps -D -m644 $(name).svg $(datadir)/icons/hicolor/scalable/apps/$(name).svg

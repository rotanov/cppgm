TEMPLATE = subdirs
SUBDIRS = common pptoken posttoken
pptoken.depends = common
posttoken.depends = common
#!/bin/sh

rpmbuild --define "_topdir `pwd`/rpm" --target x86_64 -bb rpmbuild-libjboot-1.4.0.spec
mv rpm/RPMS/x86_64/*.rpm .
rm -rf rpm

#!@PATH_PERL@
eval 'exec @PATH_PERL@ -S $0 ${1+"$@"}'
    if $running_under_some_shell;

use strict;
use warnings;

#   bootstrapping private installed modules
use lib '@INSTALLPRIVLIB@';
use lib '@INSTALLARCHLIB@';
use TheWML::Backends::Slice::Main ();

TheWML::Backends::Slice::Main->new( argv => [@ARGV] )->main;



###############################################################################
##                                                                           ##
##    Copyright (c) 1995 - 2000 by Steffen Beyer.                            ##
##    All rights reserved.                                                   ##
##                                                                           ##
##    This package is free software; you can redistribute it                 ##
##    and/or modify it under the same terms as Perl itself.                  ##
##                                                                           ##
###############################################################################

use ExtUtils::MakeMaker;

use Config;

WriteMakefile(
    'NAME'              => 'Bit::Vector',
    'VERSION_FROM'      => 'Vector.pm',
    'OBJECT'            => '$(O_FILES)',
    'LIBS'              => [''],   # e.g., '-lm'
    'DEFINE'            => '',     # e.g., '-DHAVE_SOMETHING'
    'INC'               => '',     # e.g., '-I/usr/include/other'
#   ($] >= 5.005 ?
#       ('ABSTRACT'     => 'Efficient base class implementing bit vectors',
#        'AUTHOR'       => 'Steffen Beyer (sb@engelschall.com)') : ()),
#   ($] >= 5.005 && $ eq 'MSWin32' && $Config{archname} =~ /-object\b/i ?
#       ('CAPI'         => 'TRUE') : ()),
    'dist'              => { COMPRESS => "gzip -9", SUFFIX => "gz" }
);


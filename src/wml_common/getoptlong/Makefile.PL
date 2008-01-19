# $Id: Makefile.PL,v 1.9 1999-08-04 10:33:07+02 jv Exp $

use ExtUtils::MakeMaker 5.0;

require "./GetoptLong.pm";

WriteMakefile(
	NAME      => "Getopt::Long",
	VERSION   => $Getopt::Long::VERSION,

	INSTALLDIRS => 'perl',
	PM 	  => { "GetoptLong.pm" => '$(INST_LIB)/Getopt/Long.pm'
		     },
	dist      => { COMPRESS => 'gzip', SUFFIX => 'gz',
		     },
 );

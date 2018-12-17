use strict;
use warnings;
use Test::More;
use Test::Code::TidyAll qw/ tidyall_ok /;
if ( !$ENV{'WML_TEST_TIDY'} )
{
    plan skip_all => "Skipping because WML_TEST_TIDY is not set";
}
tidyall_ok();

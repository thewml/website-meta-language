use strict;
use warnings;
use Test::More;
if ( !$ENV{'WML_TEST_TIDY'} )
{
    plan skip_all => "Skipping because WML_TEST_TIDY is not set";
}
require Test::Code::TidyAll;
Test::Code::TidyAll::tidyall_ok();

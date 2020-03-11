#!/usr/bin/perl

use strict;
use warnings;

use Getopt::Long qw / GetOptions /;

my @params = (
    qw(
        INSTALLARCHLIB
        INSTALLPRIVLIB
        PATH_PERL
        WML_CONFIG_ARGS
        WML_VERSION
        bindir
        built_date
        built_system
        built_user
        libdir
        mandir
        perlprog
        perlvers
        prefix
        )
);

my %substitutions;

my $input_fn;
my $output_fn;

my $DISCARD_POD = 1;
GetOptions(
    "subst=s"      => \%substitutions,
    "input=s"      => \$input_fn,
    "output=s"     => \$output_fn,
    "discardpod=i" => \$DISCARD_POD,
);

if ( !defined($input_fn) )
{
    die "Input filename not specified!";
}

if ( !defined($output_fn) )
{
    die "Output filename not specified!";
}

verify_all_keys( [ sort { $a cmp $b } @params ],
    [ sort { $a cmp $b } keys(%substitutions) ] );

my $subst_keys_re = join( "|", map { quotemeta($_) } @params );

open my $in_fh, "<", $input_fn
    or die "Could not open '$input_fn'";

open my $out_fh, ">", $output_fn
    or die "Could not open '$output_fn'";

LINES:
while ( my $line = <$in_fh> )
{
    if ( $DISCARD_POD and $line =~ /\A__END__/ )
    {
        last LINES;
    }
    elsif ( $DISCARD_POD and $line =~ /^=head1/ )
    {
    DISCARD_POD:
        while ( $line = <$in_fh> )
        {
            if ( $line =~ /^=cut/ )
            {
                last DISCARD_POD;
            }
        }
    }
    else
    {
        $line =~ s{\@($subst_keys_re)\@}[$substitutions{$1}]eg;
        print {$out_fh} $line;
    }
}
close($in_fh);
close($out_fh);

chmod( 0755, $output_fn );

exit(0);

sub verify_all_keys
{
    my ( $want, $have ) = @_;

    for my $idx ( 0 .. $#$want )
    {
        if ( $want->[$idx] ne $have->[$idx] )
        {
            die "Substitution $want->[$idx] is missing!";
        }
    }

    if ( @$have != @$want )
    {
        die "Extra keys in substitution: "
            . join( ",", @{$have}[ @$want .. $#$have ] ) . " !";
    }

    return;
}

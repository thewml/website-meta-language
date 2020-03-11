#!/usr/bin/perl

use strict;
use warnings;

sub is_newer
{
    my $fn1   = shift;
    my $fn2   = shift;
    my @stat1 = stat($fn1);
    my @stat2 = stat($fn2);
    if ( !@stat2 )
    {
        return 1;
    }
    return ( $stat1[9] >= $stat2[9] );
}

FILES:
while (@ARGV)
{
    my $input_fn  = shift;
    my $output_fn = shift;

    next FILES if not( is_newer( $input_fn, $output_fn ) );

    open my $in_fh, "<", $input_fn
        or die "Could not open '$input_fn'";

    open my $out_fh, ">", $output_fn
        or die "Could not open '$output_fn'";

LINES:
    while ( my $line = <$in_fh> )
    {
        if ( $line =~ /\A__END__/ )
        {
            last LINES;
        }
        elsif ( $line =~ /^=head1/ )
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
            print {$out_fh} $line;
        }
    }
    close($in_fh);
    close($out_fh);

    chmod( 0755, $output_fn );
}

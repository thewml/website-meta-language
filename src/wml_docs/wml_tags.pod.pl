##
##  wml_tags.pod.pl -- create wml_tags.pod file
##  Copyright (c) 1998,1999 Ralf S. Engelschall, All Rights Reserved.
##

use strict;
use warnings;

use Getopt::Long qw / GetOptions /;

my ( $src, $dest, $main, $incl );
GetOptions(
    'src=s'  => \$src,
    'dest=s' => \$dest,
    'main=s' => \$main,
    'incl=s' => \$incl,
) or die "Wrong parameters!";

open my $out_fh, '>', $dest
    or die "Cannot open output_file '$dest'. $!";
open my $in_fh, '<', $src
    or die "Cannot open input_file '$src'. $!";

my (@L);
while ( my $line = <$in_fh> )
{
    if ( $line =~ m|^%%CORE%%| )
    {
        open( my $tmp_fh, '<', $main )
            or die "Cannot open main file - '$main' - $!";
        @L = ();
    TMP_LINES:
        while ( $line = <$tmp_fh> )
        {
            next TMP_LINES if ( $line =~ m|^\s*$| );
            push( @L, $line );
        }
        close($tmp_fh);
        @L = sort(@L);
        my $n = 0;
        foreach my $l (@L)
        {
            print {$out_fh} " " . $l;
            $n++;
            if ( ( $n % 10 ) == 0 )
            {
                $n = 0;
                print {$out_fh} "\n";
            }
        }
    }
    if ( defined($line) && ( $line =~ m|^%%INCL%%| ) )
    {
        open( my $tmp_fh, '<', $incl )
            or die "Cannot open incl file - '$incl' - $!";
        @L = ();
    TMP2_LINES:
        while ( $line = <$tmp_fh> )
        {
            next TMP2_LINES if ( $line =~ m|^\s*$| );
            push( @L, $line );
        }
        close($tmp_fh);
        @L = sort(@L);
        my $n = 0;
        foreach my $l (@L)
        {
            print {$out_fh} " " . $l;
            $n++;
            if ( ( $n % 10 ) == 0 )
            {
                $n = 0;
                print {$out_fh} "\n";
            }
        }
    }
    elsif ( defined($line) )
    {
        print {$out_fh} $line;
    }
}
close($in_fh);
close($out_fh);

##EOF##

#!/usr/bin/perl

use strict;
use warnings;

my ($MYPROGRAM, $MYFLAGS, $FORMAT, $CMAKE_CURRENT_SOURCE_DIR, $TARGET) = @ARGV;
my $out = `${MYPROGRAM} ${MYFLAGS} -D format=${FORMAT} ${CMAKE_CURRENT_SOURCE_DIR}/mp4h.mp4h`;

# print STDERR  "FOOBARBAZQUUX === ${MYPROGRAM} ${MYFLAGS} -D format=${FORMAT} ${CMAKE_CURRENT_SOURCE_DIR}/mp4h.mp4h";

$out =~ s/_LT_/</g;
$out =~ s/_GT_/>/g;

open my $fh, '>', $TARGET or die qq/Cannot open ${TARGET} - $!/ ;
print {$fh} $out;
close($fh);

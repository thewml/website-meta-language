##
##  slice_setup.pl -- Command line parsing and CFG setup
##  Copyright (c) 1997-2002 Ralf S. Engelschall. 
##  Copyright (c) 1999-2002 Denis Barbier.
##

package main;

sub usage {
    print STDERR "Usage: slice [options] [file]\n";
    print STDERR "\n";
    print STDERR "Options:\n";
    print STDERR "  -o, --outputfile=FILESPEC  create output file(s)\n";
    print STDERR "  -y, --output-policy=STRING set default output policy\n";
    print STDERR "  -v, --verbose              enable verbose mode\n";
    print STDERR "  -V, --version              display version string\n";
    print STDERR "  -h, --help                 display usage page\n";
    print STDERR "\n";
    print STDERR "FILESPEC format:\n";
    print STDERR "\n";
    print STDERR "  [SLICETERM:]PATH[\@CHMODOPT]\n";
    print STDERR "\n";
    print STDERR "  SLICETERM ..... a set-theory term describing the slices\n";
    print STDERR "  PATH .......... a filesystem path to the outputfile\n";
    print STDERR "  CHMODOPT ...... permission change options for 'chmod'\n";
    print STDERR "\n";
    exit(1);
}

sub hello {
    print STDERR "$slice_version->{v_tex}\n";
    print STDERR <<'EOT';
Copyright (c) 1997-2002 Ralf S. Engelschall. 
Copyright (c) 1999-2002 Denis Barbier.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
EOT
    exit(0);
}

sub setup {
    my ($CFG) = @_;

    #   parse command line options
    $opt_h = 0;
    $opt_V = 0;
    $opt_v = 0;
    $opt_y = '';
    @opt_o = ();
    my (@options_list) = (
        "v|verbose",
        "V|version",
        "h|help",
        "o|outputfile=s@",
        "y|output-policy=s",
    );
    $SIG{'__WARN__'} = sub { 
        print STDERR "Slice:Error: $_[0]";
    };

    $Getopt::Long::bundling = 1;
    $Getopt::Long::getopt_compat = 0;
    if (not Getopt::Long::GetOptions(@options_list)) {
        print STDERR "Try `$0 --help' for more information.\n";
        exit(0);
    }
    $SIG{'__WARN__'} = undef;
    usage($0) if ($opt_h);
    hello() if ($opt_V);

    #   process command line arguments and
    #   read input file
    if (($#ARGV == 0 and $ARGV[0] eq '-') or $#ARGV == -1) {
        $fp = new IO::Handle;
        $fp->fdopen(fileno(STDIN), "r")
            || error("Unable to load STDIN: $!\n");
        local ($/) = undef;
        $INPUT = <$fp>;
        $fp->close()
            || error("Unable to close STDIN: $!\n");
    }
    elsif ($#ARGV == 0) {
        $fp = new IO::File;
        $fp->open($ARGV[0])
            || error("Unable to load $ARGV[0]: $!\n");
        local ($/) = undef;
        $INPUT = <$fp>;
        $fp->close()
            || error("Unable to close $ARGV[0]: $!\n");
    }
    else {
        usage();
    }

    #   add additional options
    $INPUT =~ s|^%!slice\s+(.*?)\n|push(@ARGV, split(' ', $1)), ''|egim;
    if (not Getopt::Long::GetOptions(@options_list)) {
        usage();
    }
    if ($#opt_o == -1) {
        @opt_o = ( "ALL:-" ); # default is all on stdout
    }

    #   setup the $CFG hash
    $CFG->{INPUT} = {};
    $CFG->{INPUT}->{SRC}   = $INPUT;  # original source
    $CFG->{INPUT}->{PLAIN} = '';      # source without slice delimiters
    $CFG->{OPT} = {};    
    $CFG->{OPT}->{X} = $opt_v;        # option -v
    $CFG->{OPT}->{O} = [ @opt_o ];    # options -o
    $CFG->{OPT}->{Y} = {
        'u' => 0, 'w' => 0,
        'z' => 0, 's' => 0,
    };
    $modifier = $opt_y;
    foreach (qw(u w z s)) {
        ($modifier =~ m/$_(\d+)/) and $CFG->{OPT}->{Y}->{$_} = $1;
    }
    $CFG->{SLICE} = {};
    $CFG->{SLICE}->{SET} = {};       
    $CFG->{SLICE}->{SET}->{ASC} = {}; # slice set, represented in ASCII
    $CFG->{SLICE}->{SET}->{OBJ} = {}; # slice set, represented as Bit::Vector object
    $CFG->{SLICE}->{MINLEVELS}  = {}; # slice min levels
    $CFG->{SLICE}->{MAXLEVEL}   = 0;  # maximum slice level
}

1;
##EOF##

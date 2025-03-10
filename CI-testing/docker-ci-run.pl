#! /usr/bin/env perl

use strict;
use warnings;
use 5.014;
use autodie;

use Carp::Always;

use Docker::CLI::Wrapper::Container v0.0.4 ();

package Docker::CLI::Wrapper::Container;

sub commit
{
    my ( $self, $args ) = @_;

    $self->docker(
        { cmd => [ 'commit', $self->container(), $args->{label}, ] } );

    return;
}

sub run_docker_commit
{
    my ( $self, $args ) = @_;

    $self->docker(
        {
            cmd => [ 'run', "-d", $args->{label}, ],
        }
    );

    return;
}

package Docker::CLI::Wrapper::Container::Config;

use Moo;

has [
    qw/ container install_langpack package_manager_install_cmd pip_options setup_package_manager setup_script_cmd snapshot_names_base sys_deps /
] => ( is => 'ro', required => 1 );

package main;

use Cwd            qw/ getcwd /;
use File::Basename qw/ basename /;
use YAML::XS       qw/ LoadFile /;

my $YAML_FN = ".travis.yml";
if ( !-f $YAML_FN )
{
    die "No \"${YAML_FN}\"";
}
my ($yaml_data) = LoadFile($YAML_FN);

my $debian_sys_deps =
    [ @{ $yaml_data->{addons}{apt}{packages} }, 'liblocal-lib-perl', ];
my $git_cmds = [
    "git clone https://github.com/thewml/website-meta-language/",
    "cd website-meta-language",
];
my $trav_cmds = [
    ( grep { $_ ne "cpanm local::lib" } @{ $yaml_data->{install} } ),
    @$git_cmds, @{ $yaml_data->{script} },
];

my $NOSYNC  = "LD_PRELOAD=/usr/lib64/nosync/nosync.so";
my $EN      = "export $NOSYNC";
my $configs = {
    'archlinux' => Docker::CLI::Wrapper::Container::Config->new(
        {
            container                   => "wml_archlinux",
            install_langpack            => "true",
            package_manager_install_cmd => "pacman -Syu",

            # pip_options                 => "--break-system-packages",
            pip_options           => "",
            setup_package_manager => "pacman -Syu sudo ;",
            setup_script_cmd      => "true ;",
            snapshot_names_base   => "wml/itself_archlinu",
            sys_deps              => [
                map { s/\-devel\z//mrs }
                    qw/
                    diffutils
                    gd-devel
                    gdbm-devel
                    gmp-devel
                    hostname
                    html401-dtds
                    libdb-devel
                    libxml2-devel
                    libxslt
                    libxslt-devel
                    ncurses-devel
                    pcre-devel
                    perl-LWP-Protocol-https
                    perl-XML-Parser
                    perl-generators
                    python3
                    python3-devel
                    sgml-common
                    vim
                    which
                    xhtml1-dtds
                    xz
                    /,
            ],
        }
    ),
    'debian:12' => Docker::CLI::Wrapper::Container::Config->new(
        {
            container                   => "wml_ci_debian",
            install_langpack            => "false",
            package_manager_install_cmd =>
                "sudo eatmydata apt-get --no-install-recommends install -y",
            pip_options           => "--break-system-packages",
            setup_package_manager => <<'EOF',
su -c "apt-get update"
su -c "apt-get -y install eatmydata locales netselect-apt sudo"
printf "%s\n%s\n" "en_US.UTF-8 UTF-8" "C.UTF-8 UTF-8" > /etc/locale.gen
sudo dpkg-reconfigure --frontend=noninteractive locales
sudo apt-get update -qq
sudo eatmydata apt-get -y full-upgrade
EOF
            setup_script_cmd    => "true",
            snapshot_names_base => "wml/hpage_debian",
            sys_deps            => [

                @$debian_sys_deps,
                qw/
                    build-essential
                    libdb5.3-dev
                    libexpat1-dev
                    libgd-dev
                    libgdbm-compat-dev
                    libgdbm-dev
                    libncurses-dev
                    libpcre2-dev
                    libpcre3-dev
                    libperl-dev
                    libpython3-dev
                    libssl-dev
                    libxml2-dev
                    libxml2-utils
                    libxslt1-dev
                    lynx
                    openssl
                    perl
                    python3
                    python3-all
                    python3-dev
                    txt2html
                    vim
                    xsltproc
                    xz-utils
                    zip
                    /,
            ],
        }
    ),
    'fedora:41' => Docker::CLI::Wrapper::Container::Config->new(
        {
            container                   => "wml_fedora",
            install_langpack            => "true",
            package_manager_install_cmd => "$NOSYNC sudo dnf -y install",

            # pip_options                 => "--break-system-packages",
            pip_options           => "",
            setup_package_manager =>
"sudo dnf -y install nosync ; $EN ; sudo dnf -y update --refresh ; ",
            setup_script_cmd    => "$EN",
            snapshot_names_base => "wml/hpage_fedora",
            sys_deps            => [
                qw/
                    diffutils
                    gd-devel
                    gdbm-devel
                    gmp-devel
                    hostname
                    html401-dtds
                    libdb-devel
                    libxml2-devel
                    libxslt
                    libxslt-devel
                    ncurses-devel
                    pcre-devel
                    perl-LWP-Protocol-https
                    perl-XML-Parser
                    perl-generators
                    python3
                    python3-devel
                    sgml-common
                    vim
                    which
                    xhtml1-dtds
                    xz
                    /,
            ],
        }
    ),
};

my $COPY_CLONES_DIR = 0;

sub run_config
{
    my ( $self, $args ) = @_;

    my $cleanrun   = $args->{cleanrun};
    my $cleanup    = $args->{cleanup};
    my $force_load = $args->{force_load};
    my $sys        = $args->{sys};

    my $cfg = $configs->{$sys}
        or die "no $sys config";

    my $container                   = $cfg->container();
    my $install_langpack            = $cfg->install_langpack();
    my $package_manager_install_cmd = $cfg->package_manager_install_cmd();
    my $pip_options                 = $cfg->pip_options();
    my $setup_package_manager       = $cfg->setup_package_manager();
    my $setup_script_cmd            = $cfg->setup_script_cmd();
    my $sys_deps                    = $cfg->sys_deps();
    my $snapshot_names_base         = $cfg->snapshot_names_base();

    my $obj = Docker::CLI::Wrapper::Container->new(
        { container => $container, sys => $sys, }, );

    my @deps = (
        sort { $a cmp $b } (
            qw/
                cmake
                cmake-data
                cpanminus
                expat
                g++
                gcc
                git
                lynx
                make
                m4
                python3
                python3-pip
                python3-setuptools
                rsync
                tidy
                zip
                /,
            @$sys_deps,
        )
    );
    my @cpan_deps = (
        qw/
            Carp::Always
            File::Which
            List::MoreUtils
            MooX
            MooX::late
            Path::Tiny
            String::ShellQuote
            Test::Code::TidyAll
            Test::Differences
            Test::File::IsSorted
            Test::PerlTidy
            Test::TrailingSpace
            /
    );

    if (
        not(    -d "./.git"
            and -d "./CI-testing" )
        )
    {
        die "Must be run as \"$^X CI-testing/docker-ci-run.pl\"!";
    }

    my $commit    = $snapshot_names_base . "_1";
    my $from_snap = 0;
    if ($cleanrun)
    {
        $obj->clean_up();
        if ($cleanup)
        {
            warn "doing only --cleanup!";
            return;
        }
        $obj->run_docker();
    }
    else
    {
        eval {
            my $snap_obj = Docker::CLI::Wrapper::Container->new(

                # { container => $commit, sys => $sys, },
                { container => $container, sys => $sys, },
            );
            $snap_obj->run_docker_commit( { label => $commit, } );
            $obj = $snap_obj;
        };
        if ($@)
        {
            if ($force_load)
            {
                die qq#could not load sys='$sys'!#;
            }

            $obj->clean_up();
            $obj->run_docker();
        }
        else
        {
            $from_snap = 1;
        }
    }
    my $temp_git_repo_path = "../temp-git";
    if ($from_snap)
    {
        # body...
    }
    else
    {
        # else...
        $obj->do_system( { cmd => [ "rm", "-fr", $temp_git_repo_path ] } );
        $obj->do_system(
            {
                cmd => [
                    'git',                  'clone',
                    '--recurse-submodules', '.',
                    $temp_git_repo_path
                ]
            }
        );
        $obj->do_system(
            {
                cmd => [
qq#find lib -name .git | xargs dirname | perl -lnE 'system(qq[d=../temp-git/\$_ ; if test -d \\\$d ; then exit 0 ; fi ; mkdir -p `dirname \\\$d` ;cp -a \$_/ ../temp-git/\$_]);'
#,
                ]
            }
        );

        $obj->docker(
            {
                cmd => [
                    'cp',
                    ( $temp_git_repo_path . "" ),
                    ( $obj->container() . ":/temp-git" ),
                ]
            }
        );
        if ($COPY_CLONES_DIR)
        {
            my $trunkbn = basename( getcwd() );
            my $suf     = '--clones';
            $obj->docker(
                {
                    cmd => [
                        'cp',
                        ( "../$trunkbn" . $suf . "" ),
                        ( $obj->container() . ":/temp-git" . $suf ),
                    ]
                }
            );
        }
    }
    $obj->exe_bash_code( { code => "mkdir -p /temp-git", } );
    my $locale = <<"EOSCRIPTTTTTTT";
export LC_ALL=en_US.UTF-8
export LANG="\$LC_ALL"
export LANGUAGE="en_US:en"
EOSCRIPTTTTTTT

    my $script = <<"EOSCRIPTTTTTTT";
set -e -x
$locale
mv /temp-git ~/source
if test "$COPY_CLONES_DIR" != "0"
then
    mv /temp-git--clones ~/source--clones
fi
true || ls -lR /root
$setup_package_manager
cd ~/source
if $install_langpack
then
    $package_manager_install_cmd glibc-langpack-en glibc-locale-source
fi
$package_manager_install_cmd @deps
sudo ln -sf /usr/bin/make /usr/bin/gmake
EOSCRIPTTTTTTT

    if ($from_snap)
    {
        # body...
    }
    else
    {
        $obj->exe_bash_code( { code => $script, } );
        if ( not $cleanrun )
        {
            $obj->commit( { label => $commit, } );
        }
    }
    my @nl_trav_cmds = map { "$_\n" } @$trav_cmds;

    $script = <<"EOSCRIPTTTTTTT";
set -e -x
$locale
$setup_script_cmd
which cmp
pydeps="WebTest appdirs beautifulsoup4 bottle bs4 cssselect lxml numpy pycotap rebookmaker scour soupsieve vnu_validator weasyprint webtest zenfilter"
sudo -H bash -c "$setup_script_cmd ; `which python3` -m pip install $pip_options \$pydeps"
cpanm --notest Pod::Xhtml
# For wml
cpanm --notest Bit::Vector Carp::Always Class::XSAccessor GD Getopt::Long Image::Size List::MoreUtils Path::Tiny Term::ReadKey
@nl_trav_cmds
EOSCRIPTTTTTTT
    $obj->exe_bash_code( { code => $script, } );

    # Shutting down is important as otherwise the VM continues to run
    # in the background, and consume CPU and RAM, and slow down the subsequent
    # runs.
    $obj->clean_up();

    return;
}

use Getopt::Long qw/ GetOptions /;

my $output_fn;
my $force_load;
my $cleanrun;
my $cleanup;
my $regex_filter     = '.';
my $regex_filter_out = '________NOTHING';

GetOptions(
    "cleanrun!"          => \$cleanrun,
    "cleanup!"           => \$cleanup,
    "force-load!"        => \$force_load,
    "output|o=s"         => \$output_fn,
    "regex-filter=s"     => \$regex_filter,
    "regex-filter-out=s" => \$regex_filter_out,
) or die $!;

# enable hires wallclock timing if possible
use Benchmark ':hireswallclock';

my %times;

my @systems_names = (
    grep { /$regex_filter/ms and ( not /$regex_filter_out/ms ) }
    sort { $a cmp $b } ( keys %$configs )
);
SYSTEMS:
foreach my $sys (@systems_names)
{
    $times{$sys} = timeit(
        1,
        sub {
            __PACKAGE__->run_config(
                {
                    cleanrun   => $cleanrun,
                    cleanup    => $cleanup,
                    force_load => $force_load,
                    sys        => $sys,
                }
            );
            return;
        }
    );
}

TIMES:
foreach my $sys (@systems_names)
{
    print $sys, ": ", timestr( $times{$sys} ), "\n";
}

print "Success!\n";
exit(0);

__END__

=head1 COPYRIGHT & LICENSE

Copyright 2019 by Shlomi Fish

This program is distributed under the MIT / Expat License:
L<http://www.opensource.org/licenses/mit-license.php>

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

=cut

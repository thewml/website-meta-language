use ExtUtils::MakeMaker;

WriteMakefile(
    'NAME'	   => 'HTML::Clean',
    'VERSION_FROM' => 'Clean.pm',
    'dist'         => { COMPRESS => 'gzip', SUFFIX => 'gz' },
);

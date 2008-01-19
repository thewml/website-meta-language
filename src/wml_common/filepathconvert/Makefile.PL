use ExtUtils::MakeMaker;
WriteMakefile(
    'NAME' => 'File::PathConvert',
    'VERSION_FROM' => 'PathConvert.pm',
    'clean' => {FILES => 'LOG ./test'},
    'dist' => {TARFLAGS => 'cvf', COMPRESS => 'gzip', SUFFIX => 'gz'},
);

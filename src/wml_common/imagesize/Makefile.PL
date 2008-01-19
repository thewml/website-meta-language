use ExtUtils::MakeMaker;
WriteMakefile(NAME         => 'Image::Size',
              ($] >= 5.005 ?
               (ABSTRACT   => 'A library to extract height/width from images',
                AUTHOR     => 'Randy J. Ray (rjray@blackperl.com)') : ()),
              dist         => { COMPRESS => 'gzip -9f', SUFFIX => 'gz' },
              PREREQ_PM    => { File::Spec => 0 },
              clean        => { FILES => 'Image-Size-*' },
              VERSION_FROM => 'Size.pm');

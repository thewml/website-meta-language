SET (CPAN_MODULES_TO_CHECK
    "Bit::Vector"
    "Carp::Always"
    "Class::XSAccessor"
    "File::Which"
    "GD"
    "Getopt::Long"
    "Image::Size"
    "Term::ReadKey"
    )

CHECK_FOR_MULTIPLE_PERL_MODULES(${CPAN_MODULES_TO_CHECK})

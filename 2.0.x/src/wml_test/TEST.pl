##
##  TEST.pl -- WML Test Suite utility functions
##  Copyright (c) 1997 Ralf S. Engelschall, All Rights Reserved. 
##

package TEST;

@TMPFILES = ();
$TMPFILECNT = 0;

sub init {
    return;
}

sub tmpfile {
    local (*FP, $file);

    $file = "tmp." . sprintf("%02d", $TMPFILECNT++);
    push(@TMPFILES, $file);

    if (@_ != -1) {
        open(FP, ">$file");
        print FP @_;
        close(FP);
    }

    return $file;
}

sub tmpfile_with_name {
    local ($name) = shift @_;
    local (*FP, $file);

    $file = $name;
    push(@TMPFILES, $file);

    if (@_ != -1) {
        open(FP, ">$file");
        print FP @_;
        close(FP);
    }

    return $file;
}

sub system {
    local ($cmd) = @_;
    local ($rc);

    $rc = system($cmd);
    return $rc;
}

sub generic {
    local ($pass, $in, $out, $opt) = @_;
    local($tmpfile1, $tmpfile2, $tmpfile3, $rc);
    $tmpfile1 = &tmpfile(qq#$in#);
    $tmpfile2 = &tmpfile(qq#$out#);
    $tmpfile3 = &tmpfile;
    $rc = &system("$ENV{WML} -p$pass $opt $tmpfile1 >$tmpfile3");
    print ($rc == 0 ? "ok\n" : "not ok\n");
    $rc = &system("cmp $tmpfile2 $tmpfile3");
    print ($rc == 0 ? "ok\n" : "not ok\n");
}

sub cleanup {
    foreach $file (@TMPFILES) {
        unlink($file);
    }
}

1;
##EOF##

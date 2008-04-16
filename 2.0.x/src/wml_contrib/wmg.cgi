:
eval 'exec perl -S $0 ${1+"$@"}'
    if $running_under_some_shell;
##
##  wmg.cgi -- webdesign magnifying glass
##  Copyright (c) 1998,1999 Ralf S. Engelschall, All Rights Reserved. 
##
##  This script is a webdesigner's tool to explore how
##  real-life webpages are constructed. It works by acting
##  as a filtering proxy which converts the webpage and
##  its inlined graphics on-the-fly. The amount of
##  conversion is controlled by a exploration level.
##
##  Disclaimer: I didn't use the CGI.pm module because
##              it leads to unexpected errors under the
##              combination Apache 1.2.[45] / Solaris 2.6 :-(
##


require 5.004;

#   import used third-party modules
use URI::URL;
use HTTP::Headers;
use HTTP::Request;
use LWP::UserAgent;
use Image::Size;
use GD;

#   switch to unbuffered I/O
$|++;

#   let us catch runtime errors...
eval {

##
##  configuration
##  [THIS SHOULD BE THE ONLY PLACE WHERE YOU EDIT SOMETHING]
##

#   determine our name
$server_name = $ENV{'SERVER_NAME'};

#   the URL to an optionally used proxy ('none' for no proxy)
$proxy_url = 'none';
$proxy_url = 'http://en1.engelschall.com:8080/' if ($server_name =~ m|^en1|);
$proxy_url = 'http://www-proxy.de.uu.net:3128/' if ($server_name =~ m|^www\.engelschall\.com$|);
$proxy_url = 'http://proxy.ee.ethz.ch:3128/'    if ($server_name =~ m|^www\.ch\.engelschall\.com$|);

#   comma seperated list of domains for 
#   which no proxy is used
$no_proxy_domains = 'none';

#   the URL to ourself for URL rewriting
#   
#   Note: I use it with a directory style URL through
#          the use of the following Apache/mod_rewrite
#          ruleset. Just index.cgi will not work! You
#          then also have to set $my_url to ../index.cgi!
#
#   RewriteEngine on
#   RewriteRule   ^$        wmg.cgi    [L]
#   RewriteRule   ^wmg\.cgi  -         [L]
#   RewriteRule   ^(.+)     wmg.cgi/$1 [T=application/x-httpd-cgi,L]
#
$my_url = 'http://'.$server_name.'/sw/wml/wmg/';
$my_url_sep = '/' if ($my_url !~ m|/$|);

#   the inital URL for the form
$init_url = 'http://'.$server_name.'/sw/wml/';

#   the name of this service
$our_name = 'WMG';
$our_vers = '1.1.0';


##
##  import of parameters
##

#   PATH_INFO
$path_info = $ENV{'PATH_INFO'};

#   QUERY_STRING
$query_string = $ENV{'QUERY_STRING'};
if ($ENV{'REQUEST_METHOD'} eq 'POST') {
    $query_string = '';
    while (<STDIN>) { $query_string .= $_; }
}
%qs = ();
@pairs = split(/&/, $query_string);
foreach $pair (@pairs) {
    my ($name, $value) = split(/=/, $pair);
    $value =~ s/%([a-fA-F0-9][a-fA-F0-9])/pack('C', hex($1))/eg;
    $qs{$name} = $value;
}


##
##  helper functions
##

#   send a HTTP response for a complete page
sub send_page {
    my ($type, $data) = @_;
    print "Content-type: $type\n";
    print "Content-length: ".sprintf("%d", length($data))."\n";
    print "Connection: close\n";
    print "Server: $our_name/$our_vers\n";
    print "\n";
    print $data;
}

#   send a HTTP redirect
sub send_redirect {
    my ($url) = @_;
    print "Status: 302\n";
    print "Location: $url\n";
    print "URI: $url\n";
    print "Server: $our_name/$our_vers\n";
    print "Content-type: text/html\n";
    print "\n";
    print "New URL: <a href=\"$url\">$url</a>\n";
}

#   decode a UU buffer
sub uudecode {
    my($in) = @_;
    my($result,$file,$mode);

    $result = $mode = $file = '';
    while ($in =~ s/(.*?\n)//s) {
        my $line = $1;
        if ($file eq '' and !$mode){
            ($mode,$file) = $line =~ /^begin\s+(\d+)\s+(\S+)/ ;
            next;
        }
        next if $file eq '' and !$mode;
        last if $line =~ /^end/;
        $result .= uudecode_chunk($line);
    }
    wantarray ? ($result,$file,$mode) : $result;
}
sub uudecode_chunk {
    my($chunk) = @_;

    return '' if $chunk =~ /^(--|\#|CREATED)/;
    my $string = substr($chunk,0,int((((ord($chunk) - 32) & 077) + 2) / 3)*4+1);
    return unpack("u", $string);
}

##
##  processing
##

#
#   display input form on no parameters
#
if ($path_info eq '' and $query_string eq '') {
    $data = <<"EOT";
<html>
<head>
<title>webdesign magnifying glass</title>
</head>
<body bgcolor="#c0c0c0">
  <p>
  <br>
  <center>
    <p>
    <table bgcolor="#ffffff" cellspacing=0 cellpadding=10 border=2>
    <tr>
    <td align=center>
    <form method="POST" action="$my_url" 
          enctype="application/x-www-form-urlencoded">
      <table cellspacing=0 cellpadding=0 border=0>
      <tr>
        <td4>
          <img src="${my_url}${my_url_sep}head.gif" 
               alt="webdesign magnifying glass" width="511" height="219">
        </td>
      </tr>
      <tr>
        <td>
          <font size=+1 face="Arial,Helvetica">
          Copyright (c) 1998,1999 Ralf S. Engelschall, All Rights Reserved.
          </font>
        </td>
      </tr>
      </table>
      <br>
      <table cellspacing=0 cellpadding=0 border=0>
      <tr>
         <td><font size=+2 face="Arial,Helvetica"><b>URL:&nbsp;&nbsp;</b></font></td>
         <td colspan=2><input type="text" size=40 name="url" value="$init_url"></td>
      </tr>
      <tr>
         <td><font size=+2 face="Arial,Helvetica"><b>Level:&nbsp;&nbsp;</b></font></td>
         <td>
           <select name="level">
             <option value="0">0 [+just URL rewriting]
             <option value="1">1 [+visible 1pt-images]
             <option value="2">2 [+visible borders]
             <option value="3">3 [+substituted images]
             <option value="4" selected>4 [+removed colors]
           </select>
         </td>
         <td><font face="Arial,Helvetica" size=+2 color="#cc0000">
             <b><input type="submit" name="submit" value="magnify!"></b></font>
         </td>
      </tr>
      </table>
    </form>
    </td>
    </tr>
    </table>
  </center>
</body>
</html>
EOT
    &send_page('text/html', $data);
    exit(0);
}

#
#   send out the header image
#
if ($path_info eq '/head.gif') {
    $head_gif = '';
    while (<DATA>) {
        $head_gif .= $_;
    }
    &send_page('image/gif', &uudecode($head_gif));
    exit(0);
}

#
#   do a redirection for the form data to
#   get the inital canonical form of the URL
#
if ($path_info eq '' and $qs{'url'} ne '' and $qs{'level'} ne '') {
    $path = $qs{'url'};
    $path =~ s|^http://||;
    $level = $qs{'level'};
    &send_redirect($my_url.$my_url_sep."l=$level/$path");
    exit(0);
}

#
#   do the actual filtering for a particular URL
#   (this URL can be either a webpage itself or
#   some of the inlined images, etc.)
#

#   define an own user-agent which has the 
#   resolving of HTTP redirections disabled
#   because redirects have to go through us, too.
package LWP::MyUA;
@ISA = qw(LWP::UserAgent);
sub redirect_ok { return 0; }
package main;

#   create an user-agent object
$useragent = new LWP::MyUA;
$useragent->agent("$our_name/$our_vers");
$useragent->proxy('http', $proxy_url) if ($proxy_url ne 'none');
$useragent->no_proxy(split(/,/, $no_proxy_domains)) if ($no_proxy_domains ne 'none');

#   calculate the URL which has ourself as the prefix
if ($path_info =~ m|^/l=(\d)/(.+)$|) {
    ($level, $path_info) = ($1, $2);
}
$alt = '';
if ($path_info =~ m|^a=([^/]+)/(.+)$|) {
    ($alt, $path_info) = ($1, $2);
    $alt =~ s/X\(([a-fA-F0-9][a-fA-F0-9])\)/pack('C', hex($1))/eg;
}
$url = 'http://'.$path_info;
$url .= "?$query_string" if ($query_string ne '');
$file = $url;
$file =~ s|^.+/([^/]+)$|$1|;
$url = new URI::URL($url);

#   create the HTTP request
$headers = new HTTP::Headers;
$request = new HTTP::Request('GET', $url, $headers);

#   perfrom the HTTP request
$response = $useragent->request($request, undef, undef);

#   parse the HTTP response
$urlbase  = $response->base;
$contents = $response->content;
$type     = $response->content_type;

#   if a redirect was forced we perform it
#   but with an adjusted URL which again has
#   ourself as the prefix
if ($response->is_redirect) {
    $path = $response->header("Location");
    $path =~ s|^http://||;
    &send_redirect($my_url.$my_url_sep."l=$level/".$path);
    exit(0);
}

#
#   now do the actual filtering
#   (Note 1: Level 0 is always needed!)
#   (Note 2: It's an `if' for each level, not an `elsif'!)
#
$isnewimage = 0;
if ($level >= 0) {
    #
    #   rewrite all URLs in HTML files
    #   to make sure we are always the prefix
    #
    if ($type eq 'text/html') {
        $contents =~ s/(<img\s+)(.+?)(>)/$1.&fixattr_imgsrc($2).$3/isge;
        $contents =~ s/(<frame\s+)(.+?)(>)/$1.&fixattr_framesrc($2).$3/isge;
        $contents =~ s/(<a\s+)(.+?)(>)/$1.&fixattr_ahref($2).$3/isge;
        $contents =~ s/(<script\s+[^>]+?javascript.+?>)(.+?)(<\/script>)/$1.&fixattr_js($2).$3/isge;
        sub fixattr_imgsrc {
            my ($attr) = @_;
            my $alt = ''; 
            $attr =~ s|(alt\s*=\s*")([^"]+)(")|$alt = $2, $1.$2.$3|isge;
            $attr =~ s|(alt\s*=\s*)([^"]\S+)|$alt = $2, $1.$2.$3|isge;
            if ($alt ne '') {
                $alt =~ s/([^a-zA-Z0-9])/sprintf("X(%02x)", ord($1))/eg;
                $alt = "a=$alt/";
            }
            $attr =~ s|(src\s*=\s*")([^"]+)(")|$1.&fixurl($2, $alt).$3|isge;
            $attr =~ s|(src\s*=\s*)([^"]\S+)|$1.&fixurl($2, $alt)|isge;
            return $attr;
        }
        sub fixattr_framesrc {
            my ($attr) = @_;
            $attr =~ s|(src\s*=\s*")([^"]+)(")|$1.&fixurl($2, '').$3|isge;
            $attr =~ s|(src\s*=\s*)([^"]\S+)|$1.&fixurl($2, '')|isge;
            return $attr;
        }
        sub fixattr_ahref {
            my ($attr) = @_;
            $attr =~ s|(href\s*=\s*")([^"]+)(")|$1.&fixurl($2, '').$3|isge;
            $attr =~ s|(href\s*=\s*)([^"]\S+)|$1.&fixurl($2, '')|isge;
            return $attr;
        }
        sub fixattr_js {
            my ($attr) = @_;
            $attr =~ s/(\.src\s*=\s*')([^']+\.(?:gif|jpg))(')/$1.&fixurl($2, '').$3/isge;
            $attr =~ s/(\.src\s*=\s*")([^"]+\.(?:gif|jpg))(")/$1.&fixurl($2, '').$3/isge;
            return $attr;
        }
        sub fixurl {
            my ($url, $more) = @_;
            my $u = new URI::URL $url, $urlbase;
            $url = $u->abs->as_string;
            $url =~ s|^http://||;
            $url = $my_url.$my_url_sep."l=$level/".$more.$url;
            return $url;
        }
    }
}
if ($level >= 1) {
    #
    #   replace all transparent 1pt dot-images with a red image
    #
    if ($type eq 'image/gif') {
        ($w, $h, $t) = Image::Size::imgsize(\$contents);
        if ($w*$h == 1) {
            #   read image into GD
            $tmpimg = newFromGif GD::Image($contents);
            unlink($tmpfile);
            if ($tmpimg->transparent != -1) {
                my $im = new GD::Image($w, $h);
                ($r1, $g1, $b1) = (255, 0, 0);
                ($r2, $g2, $b2) = (255, 0, 0);
                my $col1 = $im->colorAllocate($r1, $g1, $b1);
                my $col2 = $im->colorAllocate($r2, $r2, $b2);
                $contents = $im->gif;
                $isnewimage = 1;
            }
        }
    }
}
if ($level >= 2) {
    #   
    #   replace all border=0 attributes with border=1
    #
    if ($type eq 'text/html') {
        $contents =~ s|border\s*=\s*"?0"?|border=1|sgi;
        sub fix_table_border {
            my ($str) = @_;
            if ($str !~ m|border\s*=|) {
                $str = $str."border=1";
            }
            return $str;
        }
        $contents =~ s/(<table\s+)(.+?)(>)/$1.&fix_table_border($2).$3/isge;
    }
}
if ($level >= 3) {
    #
    #   remove any background images
    #
    if ($type eq 'text/html') {
        sub fix_bg {
            my ($str) = @_;
            $str =~ s/(background\s*=\s*')([^']+\.(?:gif|jpg))(')/''/isge;
            $str =~ s/(background\s*=\s*")([^"]+\.(?:gif|jpg))(")/''/isge;
            return $str;
        }
        $contents =~ s/(<body\s+)(.+?)(>)/$1.&fix_bg($2).$3/isge;
    }
    #
    #   replace all non-transparent images with a blank one
    #
    if ($type =~ m|^image/.*| and not $isnewimage) {
        ($w, $h, $t) = Image::Size::imgsize(\$contents);
        my $im = new GD::Image($w, $h);
        ($r1, $g1, $b1) = (230, 230, 230);
        ($r2, $g2, $b2) = (100, 100, 100);
        ($r3, $g3, $b3) = (0, 0, 0);
        my $col1 = $im->colorAllocate($r1, $g1, $b1);
        my $col2 = $im->colorAllocate($r2, $g2, $b2);
        my $col3 = $im->colorAllocate($r3, $g3, $b3);
        $im->rectangle(0, 0, $w-1, $h-1, $col2);
        $im->string(gdSmallFont,1,1, sprintf("%s %dx%d", $t, $w, $h), $col3);
        $im->string(gdSmallFont,1,12, "$alt", $col3) if ($alt ne '');
        $im->string(gdSmallFont,1,12, "[$file]", $col3) if ($alt eq '');
        $contents = $im->gif;
        $type = 'image/gif';
    }
}
if ($level >= 4) {
    #   
    #   remove all color attributes
    #
    if ($type eq 'text/html') {
        $contents =~ s|bgcolor="?#?[0-9a-hA-H]{6}"?||isg;
        $contents =~ s/bgcolor="?(black|white|red|yellow)"?//isg;
        $contents =~ s|color="?#?[0-9a-hA-H]{6}"?||isg;
        $contents =~ s/color="?(black|white|red|yellow)"?//isg;
        $contents =~ s|text="?#?[0-9a-hA-H]{6}"?||isg;
        $contents =~ s/text="?(black|white|red|yellow)"?//isg;
        $contents =~ s|link="?#?[0-9a-hA-H]{6}"?||isg;
        $contents =~ s/link="?(black|white|red|yellow)"?//isg;
        $contents =~ s|vlink="?#?[0-9a-hA-H]{6}"?||isg;
        $contents =~ s/vlink="?(black|white|red|yellow)"?//isg;
        $contents =~ s|alink="?#?[0-9a-hA-H]{6}"?||isg;
        $contents =~ s/alink="?(black|white|red|yellow)"?//isg;
        $contents =~ s|<font\s*>(.*?)</font>|$1|isg;
    }
}

#  
#   Puhhh... now the filtering is done.
#   All we now have to do is to send the
#   stuff to the user...
#
&send_page($type, $contents);

#   die gracefully
exit(0);

#   ...the runtime error handler:
};
if ($@) {
    my $text = $@;
    print "Content-type: text/html\n";
    print "Connection: close\n";
    print "\n";
    print "<h1>Internal Error</h1>\n";
    print $text;
}

##EOF##
__DATA__
begin 664 wmg.jpg
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
719^B8PDI*CX([AS]O^_0(!/)_P!#_]D`
`
end

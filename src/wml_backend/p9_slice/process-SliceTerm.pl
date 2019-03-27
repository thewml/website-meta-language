use strict;
use warnings;
s/ +\z//ms;
s/"'\@'"/"'\\\@'"/g;
s%"expr : SLICE '\@'",%"expr : SLICE '\\\@'",%g
